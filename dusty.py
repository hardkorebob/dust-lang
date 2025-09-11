#!/usr/bin/env python3
import tkinter as tk
from tkinter import ttk, scrolledtext, filedialog, messagebox
import re
import subprocess
import os

class DustEditor:
    def __init__(self, root):
        self.root = root
        self.root.title("Dusted")
        self.current_file = None
        self._highlight_job = None

        # -- START: Added Mouse Functionality --
        self.left_pressed = False
        # -- END: Added Mouse Functionality --

        self.highlight_rules = {
            'keyword': r'\b(func|let)\b',
            'suffix':  r'_[a-zA-Z0-9]+'
        }
        self.colors = {
            'bg': '#e8e8d8',
            'fg': '#000000',
            'select_bg': '#eeeea0',
            'status_bg': '#e7e7d7',
            'output_bg': '#ffffea',
            'keyword_fg': '#00008b',  
            'suffix_fg': '#8b0000',   
            'search_highlight_bg': '#bfff00' 
        }

        self._setup_ui()
        self.text.focus()

    def _setup_ui(self):
        """Configure the entire user interface."""
        self.root.configure(bg=self.colors['bg'])

        # --- Menu ---
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)
        
        file_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="File", menu=file_menu)
        file_menu.add_command(label="New", command=self.new_file, accelerator="Ctrl+N")
        file_menu.add_command(label="Open...", command=self.open_file, accelerator="Ctrl+O")
        file_menu.add_command(label="Save", command=self.save_file, accelerator="Ctrl+S")
        file_menu.add_separator()
        file_menu.add_command(label="Exit", command=self.root.quit)

        build_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="Build", menu=build_menu)
        build_menu.add_command(label="Full Build & Run", command=self.full_build_run, accelerator="F5")

        # --- Paned Layout (Editor/Output) ---
        main_paned = tk.PanedWindow(self.root, orient=tk.VERTICAL, bg=self.colors['bg'], sashwidth=8)
        main_paned.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

        # --- Editor Pane (with Line Numbers) ---
        editor_frame = tk.Frame(main_paned, bg=self.colors['bg'])
        self.line_numbers = tk.Text(editor_frame, width=4, padx=3, takefocus=0, highlightbackground=self.colors['bg'],
                                    font=('Iosevka', 10), bg=self.colors['status_bg'], relief="flat",
                                    fg=self.colors['fg'], state='disabled', borderwidth=0)
        self.line_numbers.pack(side=tk.LEFT, fill=tk.Y)
        
        self.text = tk.Text(editor_frame, wrap=tk.NONE, undo=True,
                            font=('Iosevka', 14), bg=self.colors['bg'], fg=self.colors['fg'],
                            insertbackground=self.colors['fg'], selectbackground=self.colors['select_bg'],
                            borderwidth=0, highlightthickness=0)
        self.text.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)
        main_paned.add(editor_frame)
        
        # --- Output Pane ---
        output_frame = tk.Frame(main_paned, bg=self.colors['bg'])
        self.output_text = scrolledtext.ScrolledText(output_frame, wrap=tk.WORD, height=8, takefocus=0,
                                                     font=('Iosevka', 10), bg=self.colors['output_bg'],
                                                     fg=self.colors['fg'], state='disabled')
        self.output_text.pack(fill=tk.BOTH, expand=True)
        main_paned.add(output_frame)
        self.root.update()
        main_paned.sash_place(0, 0, int(self.root.winfo_height() * 0.75))

        # --- Status Bar ---
        self.status = tk.Label(self.root, text="Ready", bg=self.colors['status_bg'], anchor='w', padx=5)
        self.status.pack(side=tk.BOTTOM, fill=tk.X)

        self._setup_highlighting_tags()
        self._setup_bindings_and_scroll()
        self._update_line_numbers()

    def _setup_highlighting_tags(self):
        """Configure the text tags for syntax highlighting."""
        self.text.tag_configure('keyword', foreground=self.colors['keyword_fg'])
        self.text.tag_configure('suffix', foreground=self.colors['suffix_fg'])
        # -- START: Added Mouse Functionality --
        self.text.tag_configure('search_highlight', background=self.colors['search_highlight_bg'])
        # -- END: Added Mouse Functionality --

    def _setup_bindings_and_scroll(self):
        """Set up keyboard bindings and scroll synchronization."""
        self.root.bind('<Control-n>', lambda e: self.new_file())
        self.root.bind('<Control-o>', lambda e: self.open_file())
        self.root.bind('<Control-s>', lambda e: self.save_file())
        self.root.bind('<F5>', lambda e: self.full_build_run())

        self.text.bind('<KeyRelease>', self.on_key_release)
        
        self.text.config(yscrollcommand=self._scroll_text)
        self.line_numbers.config(yscrollcommand=self._scroll_lines)

        # -- START: Added Mouse Functionality --
        self.text.bind('<Button-1>', self.on_left_press)
        self.text.bind('<ButtonRelease-1>', self.on_left_release)
        self.text.bind('<Button-2>', self.on_middle_click)
        self.text.bind('<Button-3>', self.on_right_click)
        # -- END: Added Mouse Functionality --


    def _scroll_text(self, *args):
        self.line_numbers.yview_moveto(args[0])
    
    def _scroll_lines(self, *args):
        self.text.yview_moveto(args[0])

    def on_key_release(self, event=None):
        """Schedule a syntax highlight update."""
        if self._highlight_job:
            self.root.after_cancel(self._highlight_job)
        self._highlight_job = self.root.after(200, self._highlight_syntax)
        self._update_line_numbers()

    def _highlight_syntax(self):
        """Apply syntax highlighting to the text widget."""
        content = self.text.get('1.0', tk.END)
        for tag in self.highlight_rules.keys():
            self.text.tag_remove(tag, '1.0', tk.END)
        for tag, pattern in self.highlight_rules.items():
            for match in re.finditer(pattern, content):
                start, end = match.span()
                self.text.tag_add(tag, f"1.0 + {start}c", f"1.0 + {end}c")

    def _update_line_numbers(self):
        """Update the line number display."""
        self.line_numbers.config(state='normal')
        self.line_numbers.delete('1.0', tk.END)
        line_count = self.text.get('1.0', tk.END).count('\n') + 1
        line_numbers_string = "\n".join(str(i) for i in range(1, line_count))
        self.line_numbers.insert('1.0', line_numbers_string)
        self.line_numbers.config(state='disabled')
        self._scroll_text(self.text.yview()[0])

    def log(self, message):
        self.output_text.config(state='normal')
        self.output_text.insert(tk.END, f"{message}\n")
        self.output_text.see(tk.END)
        self.output_text.config(state='disabled')
        if message:
            self.status.config(text=message.split('\n')[0])

    def clear_log(self):
        self.output_text.config(state='normal')
        self.output_text.delete('1.0', tk.END)
        self.output_text.config(state='disabled')
    
    # -- START: Added Mouse Functionality --
    def on_left_press(self, event):
        self.left_pressed = True

    def on_left_release(self, event):
        self.left_pressed = False

    def on_middle_click(self, event):
        if self.left_pressed: # Chord: Left+Middle means Cut
            try:
                selected_text = self.text.get('sel.first', 'sel.last')
                self.root.clipboard_clear()
                self.root.clipboard_append(selected_text)
                self.text.delete('sel.first', 'sel.last')
                self.log("Cut selection to clipboard")
            except tk.TclError:
                self.log("No selection to cut")
            self.left_pressed = False # Reset after chord
        return "break" # Prevent default middle-click paste

    def on_right_click(self, event):
        if self.left_pressed: # Chord: Left+Right means Paste
            try:
                text_to_paste = self.root.clipboard_get()
                self.text.insert(tk.INSERT, text_to_paste)
                self.log("Pasted from clipboard")
            except tk.TclError:
                self.log("Clipboard is empty")
            self.left_pressed = False # Reset after chord
        else: # Regular right-click means Find Next
            pos = self.text.index(f"@{event.x},{event.y}")
            word = self.get_word_at_position(pos)
            if word:
                self.find_next(word, pos)
        return "break"

    def get_word_at_position(self, pos):
        # Find word boundaries using regex for simplicity
        line_start = self.text.index(f"{pos} linestart")
        line_end = self.text.index(f"{pos} lineend")
        line_text = self.text.get(line_start, line_end)
        current_col = int(pos.split('.')[1])
        
        for match in re.finditer(r'\w+', line_text):
            if match.start() <= current_col < match.end():
                return match.group(0)
        return None
    
    def find_next(self, word, start_pos):
        self.text.tag_remove('search_highlight', '1.0', tk.END)
        pos = self.text.search(word, f"{start_pos}+1c", tk.END, nocase=True)
        if not pos: # Wrap around search
            pos = self.text.search(word, "1.0", start_pos, nocase=True)
        
        if pos:
            end_pos = f"{pos}+{len(word)}c"
            self.text.tag_add('search_highlight', pos, end_pos)
            self.text.see(pos)
            self.text.mark_set(tk.INSERT, pos)
            self.log(f"Found '{word}' at {pos}")
        else:
            self.log(f"'{word}' not found")
    # -- END: Added Mouse Functionality --

    def run_process(self, command, cwd):
        try:
            result = subprocess.run(command, capture_output=True, text=True, cwd=cwd, timeout=10)
            if result.stdout: self.log(result.stdout.strip())
            if result.stderr: self.log(f"[ERROR] {result.stderr.strip()}")
            return result.returncode == 0
        except FileNotFoundError:
            self.log(f"[ERROR] Command not found: {command[0]}. Is it in your PATH?")
        except Exception as e:
            self.log(f"[ERROR] An unexpected error occurred: {e}")
        return False

    def new_file(self):
        self.text.delete('1.0', tk.END)
        self.current_file = None
        self.root.title("Dust Editor - New File")
        self.log("New file created")

    def open_file(self):
        filename = filedialog.askopenfilename(filetypes=[("Dust Files", "*.dust"), ("All Files", "*.*")])
        if filename:
            with open(filename, 'r') as f:
                self.text.delete('1.0', tk.END)
                self.text.insert('1.0', f.read())
            self.current_file = filename
            self.root.title(f"Dust Editor - {os.path.basename(filename)}")
            self.log(f"Opened: {filename}")
            self.on_key_release()

    def save_file(self):
        if self.current_file:
            with open(self.current_file, 'w') as f:
                f.write(self.text.get('1.0', 'end-1c'))
            self.log(f"Saved: {self.current_file}")
            return True
        return self.save_as_file()

    def save_as_file(self):
        filename = filedialog.asksaveasfilename(defaultextension=".dust", filetypes=[("Dust Files", "*.dust")])
        if filename:
            self.current_file = filename
            self.root.title(f"Dust Editor - {os.path.basename(filename)}")
            return self.save_file()
        return False

    def full_build_run(self):
        if not self.current_file or not self.current_file.endswith('.dust'):
            if not self.save_as_file(): return
        
        if not self.save_file(): return

        self.clear_log()
        self.log("=== Starting Full Build & Run ===")
        file_dir = os.path.dirname(self.current_file) or '.'
        base_name = os.path.basename(self.current_file).replace('.dust', '')
        c_file = os.path.join(file_dir, f"{base_name}.c")
        exe_file = os.path.join(file_dir, base_name)
        
        if not self.run_process(['./dustc', self.current_file], file_dir):
            self.log("✗ dustc compilation failed.")
            return

        if not self.run_process(['gcc', c_file, '-o', exe_file, '-Wall'], file_dir):
            self.log("✗ gcc compilation failed.")
            return
            
        self.log(f"\n--- Running ./{base_name} ---")
        self.run_process([f'./{base_name}'], file_dir)
        self.log("=== Build & Run Finished ===")

if __name__ == "__main__":
    root = tk.Tk()
    editor = DustEditor(root)
    root.geometry("1000x700")
    root.mainloop()
