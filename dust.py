#!/usr/bin/env python3
import tkinter as tk
from tkinter import ttk, scrolledtext, filedialog, messagebox
from re import finditer
import subprocess
import os

class DustEditor:
    def __init__(self, root):
        self.root = root
        self.root.title("Dustme")
        
        # Acme editor color scheme
        self.colors = {
            'bg': '#ffffea',           # Pale yellow background
            'fg': '#000000',            # Black text
            'select_bg': '#eeeea0',     # Slightly darker yellow selection
            'status_bg': '#e8e8d8',     # Status bar background
            'listbox_bg': '#ffffea',    # Listbox background
            'listbox_fg': '#000000',    # Listbox text
            'listbox_select': '#eeeea0', # Listbox selection
            'output_bg': '#ffffea'
        }
        
        # Mouse chord tracking
        self.left_pressed = False
        self.left_press_pos = None
        self.selection_start = None
        
        # Configure style
        self.style = ttk.Style()
        self.style.configure('TFrame', background=self.colors['bg'])
        self.style.configure('TLabel', background=self.colors['status_bg'], foreground=self.colors['fg'])
        
        # Dust keywords
        self.keywords = {
            'if', 'else', 'while', 'do', 'for', 'return', 'break', 'continue',
            'func', 'let', 'struct', 'sizeof', 'switch', 'case', 'default',
            'typedef', 'cast', 'null', 'enum', 'static', 'extern', 'union'
        }
        
        # Common Dust suffixes organized by category
        self.suffix_categories = {
            'Primitives': [
                ('i', 'int'),
                ('f', 'float'),
                ('c', 'char'),
                ('s', 'string (char*)'),
                ('v', 'void'),
            ],
            'Pointers': [
                ('ip', 'int pointer'),
                ('cp', 'char pointer'),
                ('vp', 'void pointer'),
                ('fp', 'function pointer'),
                ('sp', 'string pointer'),
                ('b', 'borrowed pointer (const)'),
                ('r', 'reference pointer (const)'),
            ],
            'Arrays': [
                ('ia', 'int array'),
                ('ca', 'char array'),
                ('fa', 'float array'),
                ('u8a', 'uint8_t array'),
                ('u16a', 'uint16_t array'),
                ('u32a', 'uint32_t array'),
            ],
            'Fixed-width': [
                ('u8', 'uint8_t'),
                ('u16', 'uint16_t'),
                ('u32', 'uint32_t'),
                ('u64', 'uint64_t'),
                ('i8', 'int8_t'),
                ('i16', 'int16_t'),
                ('i32', 'int32_t'),
                ('i64', 'int64_t'),
            ],
            'System': [
                ('st', 'size_t'),
                ('ux', 'uintptr_t'),
                ('ix', 'intptr_t'),
                ('off', 'off_t'),
            ]
        }
        
        self.suffix_window = None
        self.current_file = None
        self.setup_ui()
        
    def setup_ui(self):
        # Configure root background
        self.root.configure(bg=self.colors['bg'])
        
        # Menu bar
        menubar = tk.Menu(self.root, bg=self.colors['bg'])
        self.root.config(menu=menubar)
        
        # File menu
        file_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="File", menu=file_menu)
        file_menu.add_command(label="New", command=self.new_file, accelerator="Ctrl+N")
        file_menu.add_command(label="Open...", command=self.open_file, accelerator="Ctrl+O")
        file_menu.add_command(label="Save", command=self.save_file, accelerator="Ctrl+S")
        file_menu.add_command(label="Save As...", command=self.save_as_file)
        file_menu.add_separator()
        file_menu.add_command(label="Exit", command=self.root.quit)
        
        # Build menu
        build_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="Build", menu=build_menu)
        build_menu.add_command(label="Compile to C (dustc)", command=self.compile_dust, accelerator="F5")
        build_menu.add_command(label="Compile to executable (gcc)", command=self.compile_to_exe, accelerator="F6")
        build_menu.add_command(label="Run", command=self.run_program, accelerator="F7")
        build_menu.add_separator()
        build_menu.add_command(label="Full Build & Run", command=self.full_build_run, accelerator="F8")
        
        # Info frame
        info_frame = ttk.Frame(self.root)
        info_frame.pack(fill=tk.X, padx=5, pady=2)
        
        info_label = ttk.Label(info_frame, text="Mouse: L+M=cut, L+R=paste, R=find next | Keys: Shift+Space=suffix, F5=dustc, F6=gcc, F7=run, F8=all")
        info_label.pack(side=tk.LEFT)
        
        # Main paned window for editor and output
        main_paned = tk.PanedWindow(self.root, orient=tk.VERTICAL, bg=self.colors['bg'])
        main_paned.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Editor pane
        editor_frame = tk.Frame(main_paned, bg=self.colors['bg'])
        
        # Line numbers
        self.line_numbers = tk.Text(
            editor_frame,
            width=4,
            padx=3,
            takefocus=0,
            font=('Iosevka', 12),
            bg=self.colors['status_bg'],
            fg=self.colors['fg'],
            state='disabled'
        )
        self.line_numbers.pack(side=tk.LEFT, fill=tk.Y)
        
        # Main text editor
        self.text = scrolledtext.ScrolledText(
            editor_frame, 
            wrap=tk.NONE,
            width=80,
            height=20,
            font=('Iosevka', 14),
            undo=True,
            bg=self.colors['bg'],
            fg=self.colors['fg'],
            insertbackground=self.colors['fg'],
            selectbackground=self.colors['select_bg']
        )
        self.text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        
        main_paned.add(editor_frame)
        
        # Output pane
        output_frame = tk.Frame(main_paned, bg=self.colors['bg'])
        
        output_label = tk.Label(output_frame, text="Output:", bg=self.colors['bg'], fg=self.colors['fg'], anchor='w')
        output_label.pack(fill=tk.X)
        
        self.output_text = scrolledtext.ScrolledText(
            output_frame,
            wrap=tk.WORD,
            width=80,
            height=10,
            font=('Iosevka', 9),
            bg=self.colors['output_bg'],
            fg=self.colors['fg'],
            state='disabled'
        )
        self.output_text.pack(fill=tk.BOTH, expand=True)
        
        main_paned.add(output_frame)
        
        # Set initial sash position (70% for editor, 30% for output)
        self.root.update()
        main_paned.sash_place(0, 0, 400)
        
        # Bind keys
        self.text.bind('<Shift-space>', self.show_completions)
        self.text.bind('<KeyRelease>', self.on_text_change)
        
        # Mouse chord bindings
        self.text.bind('<Button-1>', self.on_left_press)
        self.text.bind('<B1-Motion>', self.on_left_drag)
        self.text.bind('<ButtonRelease-1>', self.on_left_release)
        self.text.bind('<Button-2>', self.on_middle_click)  # Middle button
        self.text.bind('<Button-3>', self.on_right_click)   # Right button
        
        # Keyboard shortcuts
        self.root.bind('<Control-n>', lambda e: self.new_file())
        self.root.bind('<Control-o>', lambda e: self.open_file())
        self.root.bind('<Control-s>', lambda e: self.save_file())
        self.root.bind('<F5>', lambda e: self.compile_dust())
        self.root.bind('<F6>', lambda e: self.compile_to_exe())
        self.root.bind('<F7>', lambda e: self.run_program())
        self.root.bind('<F8>', lambda e: self.full_build_run())
        self.root.bind('<Escape>', self.hide_completions)
        
        # Status bar
        self.status = ttk.Label(self.root, text="Ready", relief=tk.SUNKEN)
        self.status.pack(fill=tk.X, side=tk.BOTTOM)
       
        
        # Initial update
        self.update_line_numbers()
       
        
    # Mouse chord implementation
    def on_left_press(self, event):
        """Handle left button press"""
        self.left_pressed = True
        self.left_press_pos = self.text.index(f"@{event.x},{event.y}")
        self.selection_start = self.left_press_pos
        # Set cursor position
        self.text.mark_set(tk.INSERT, self.left_press_pos)
        # Don't return "break" to allow selection
        
    def on_left_drag(self, event):
        """Handle left button drag for selection"""
        if self.left_pressed:
            current_pos = self.text.index(f"@{event.x},{event.y}")
            self.text.tag_remove('sel', '1.0', tk.END)
            self.text.tag_add('sel', self.selection_start, current_pos)
            
    def on_left_release(self, event):
        """Handle left button release"""
        self.left_pressed = False
        
    def on_middle_click(self, event):
        """Handle middle button - cut if left is pressed, otherwise paste from clipboard"""
        if self.left_pressed:
            # Cut operation (chord: left + middle)
            try:
                selected_text = self.text.get('sel.first', 'sel.last')
                self.root.clipboard_clear()
                self.root.clipboard_append(selected_text)
                self.text.delete('sel.first', 'sel.last')
                self.output_log("Cut text to clipboard")
            except tk.TclError:
                # No selection, just report
                self.output_log("No selection to cut")
        else:
            # Regular paste from system clipboard
            try:
                text = self.root.clipboard_get()
                insert_pos = self.text.index(f"@{event.x},{event.y}")
                self.text.mark_set(tk.INSERT, insert_pos)
                self.text.insert(tk.INSERT, text)
                self.output_log("Pasted from clipboard")
            except tk.TclError:
                self.output_log("Nothing to paste")
        self.left_pressed = False  # Reset after chord
        return "break"
    
    def on_right_click(self, event):
        """Handle right button - paste if left pressed, otherwise find next occurrence"""
        if self.left_pressed:
            # Paste operation (chord: left + right)
            try:
                text = self.root.clipboard_get()
                # Delete selection if exists
                try:
                    self.text.delete('sel.first', 'sel.last')
                except tk.TclError:
                    pass
                # Insert at current position
                insert_pos = self.text.index(f"@{event.x},{event.y}")
                self.text.mark_set(tk.INSERT, insert_pos)
                self.text.insert(tk.INSERT, text)
                self.output_log("Pasted text (L+R chord)")
            except tk.TclError:
                self.output_log("Nothing to paste")
            self.left_pressed = False  # Reset after chord
        else:
            # Find next occurrence of word under cursor
            pos = self.text.index(f"@{event.x},{event.y}")
            word = self.get_word_at_position(pos)
            if word:
                self.find_next(word, pos)
        return "break"
    
    def get_word_at_position(self, pos):
        """Get word at given position"""
        # Find word boundaries
        start = pos
        while True:
            prev_char = self.text.get(f"{start} -1c", start)
            if not prev_char or not (prev_char.isalnum() or prev_char == '_'):
                break
            start = f"{start} -1c"
            
        end = pos
        while True:
            next_char = self.text.get(end, f"{end} +1c")
            if not next_char or not (next_char.isalnum() or next_char == '_'):
                break
            end = f"{end} +1c"
            
        word = self.text.get(start, end)
        return word if word else None
    
    def find_next(self, word, start_pos):
        """Find next occurrence of word"""
        # Clear previous highlights
        self.text.tag_remove('search_highlight', '1.0', tk.END)
        
        # Search from current position
        pos = self.text.search(word, f"{start_pos} +1c", tk.END)
        if not pos:
            # Wrap around to beginning
            pos = self.text.search(word, '1.0', start_pos)
        
        if pos:
            end_pos = f"{pos} +{len(word)}c"
            self.text.tag_add('search_highlight', pos, end_pos)
            self.text.see(pos)
            self.text.mark_set(tk.INSERT, pos)
            self.output_log(f"Found '{word}' at {pos}")
        else:
            self.output_log(f"'{word}' not found")
    
    def output_log(self, message):
        """Add message to output pane"""
        self.output_text.config(state='normal')
        self.output_text.insert(tk.END, f"{message}\n")
        self.output_text.see(tk.END)
        self.output_text.config(state='disabled')
        self.status.config(text=message)
    
    def output_clear(self):
        """Clear output pane"""
        self.output_text.config(state='normal')
        self.output_text.delete('1.0', tk.END)
        self.output_text.config(state='disabled')
    
    def on_text_change(self, event=None):
        """Handle text changes"""
        self.update_line_numbers()
        
    def update_line_numbers(self):
        """Update line numbers display"""
        self.line_numbers.config(state='normal')
        self.line_numbers.delete('1.0', tk.END)
        
        # Count lines
        line_count = self.text.get('1.0', tk.END).count('\n')
        line_numbers_string = "\n".join(str(i) for i in range(1, line_count))
        self.line_numbers.insert('1.0', line_numbers_string)
        self.line_numbers.config(state='disabled')
    
    def get_word_at_cursor(self):
        """Get the word at cursor position, respecting proper boundaries."""
        # Find the start of the word by moving backward
        start_pos = self.text.search(r"\s", tk.INSERT, backwards=True, regexp=True)
        if not start_pos:
            start_pos = "1.0"
        else:
            start_pos = f"{start_pos}+1c" # Move one char forward from the space

        # Find the end of the word by moving forward
        end_pos = self.text.search(r"\s", tk.INSERT, forwards=True, regexp=True)
        if not end_pos:
            end_pos = tk.END
        
        word = self.text.get(start_pos, end_pos).strip()
        # Return precise start/end for replacement
        return word, start_pos, end_pos
    
    def show_completions(self, event):
        """Show suffix completion popup"""
        self.hide_completions()
        
        word, start_pos, end_pos = self.get_word_at_cursor()
        
        if not word or not word.endswith('_'):
            self.output_log("Type identifier_ first (with underscore)")
            return "break"
        
        base_word = word[:-1]  # Remove trailing underscore
        self.completion_base = word
        self.completion_start = start_pos
        self.completion_end = end_pos
        
        # Create completion window
        self.suffix_window = tk.Toplevel(self.root)
        self.suffix_window.wm_overrideredirect(True)
        self.suffix_window.configure(bg=self.colors['bg'])
        
        # Position near cursor
        bbox = self.text.bbox(tk.INSERT)
        if bbox:
            x = self.text.winfo_rootx() + bbox[0]
            y = self.text.winfo_rooty() + bbox[1] + bbox[3]
        else:
            x = self.root.winfo_x() + 100
            y = self.root.winfo_y() + 100
        
        # Check if it's a user-defined type (starts with capital)
        if base_word and base_word[0].isupper():
            # User-defined type suffixes
            self.create_simple_completion_list([
                (f'{base_word}', f'{base_word} instance'),
                (f'{base_word}p', f'{base_word}* (owned pointer)'),
                (f'{base_word}b', f'const {base_word}* (borrowed)'),
                (f'{base_word}r', f'const {base_word}* (reference)'),
                (f'{base_word}a', f'{base_word} array'),
            ])
        else:
            # Standard suffixes with categories
            self.create_categorized_completion_list()
        
        self.suffix_window.geometry(f"+{x}+{y}")
        return "break"
    
    def create_simple_completion_list(self, completions):
        """Create a simple completion list for user-defined types"""
        listbox = tk.Listbox(
            self.suffix_window,
            height=min(len(completions), 10),
            width=40,
            font=('Iosevka', 9),
            bg=self.colors['listbox_bg'],
            fg=self.colors['listbox_fg'],
            selectbackground=self.colors['listbox_select']
        )
        listbox.pack(padx=2, pady=2)
        
        self.completion_items = []
        for suffix, desc in completions:
            display = f"{suffix:<15} - {desc}"
            listbox.insert(tk.END, display)
            self.completion_items.append(suffix)
        
        listbox.selection_set(0)
        listbox.bind('<Double-Button-1>', self.insert_completion)
        listbox.bind('<Return>', self.insert_completion)
        listbox.bind('<Up>', lambda e: self.navigate_list(listbox, -1))
        listbox.bind('<Down>', lambda e: self.navigate_list(listbox, 1))
        listbox.focus_set()
        
        self.current_listbox = listbox
    
    def create_categorized_completion_list(self):
        """Create categorized completion list"""
        notebook = ttk.Notebook(self.suffix_window)
        notebook.pack(padx=2, pady=2)
        
        self.category_listboxes = {}
        self.category_items = {}
        
        for category, suffixes in self.suffix_categories.items():
            frame = ttk.Frame(notebook)
            notebook.add(frame, text=category)
            
            listbox = tk.Listbox(
                frame,
                height=min(len(suffixes), 10),
                width=40,
                font=('Iosevka', 10),
                bg=self.colors['listbox_bg'],
                fg=self.colors['listbox_fg'],
                selectbackground=self.colors['listbox_select']
            )
            listbox.pack(fill=tk.BOTH, expand=True, padx=2, pady=2)
            
            items = []
            for suffix, desc in suffixes:
                display = f"{suffix:<8} - {desc}"
                listbox.insert(tk.END, display)
                items.append(suffix)
            
            self.category_listboxes[category] = listbox
            self.category_items[category] = items
            
            listbox.selection_set(0)
            listbox.bind('<Double-Button-1>', lambda e, cat=category: self.insert_categorized_completion(cat))
            listbox.bind('<Return>', lambda e, cat=category: self.insert_categorized_completion(cat))
            
        # Focus first tab
        if self.category_listboxes:
            first_cat = list(self.category_listboxes.keys())[0]
            self.category_listboxes[first_cat].focus_set()
    
    def navigate_list(self, listbox, direction):
        """Navigate in listbox"""
        current = listbox.curselection()
        if current:
            idx = current[0]
            new_idx = max(0, min(listbox.size() - 1, idx + direction))
            listbox.selection_clear(0, tk.END)
            listbox.selection_set(new_idx)
            listbox.see(new_idx)
        return "break"
    
    def insert_completion(self, event=None):
        """Insert simple completion"""
        if not hasattr(self, 'current_listbox'):
            return
            
        selection = self.current_listbox.curselection()
        if selection:
            idx = selection[0]
            suffix = self.completion_items[idx]
            
            # Replace the word with base_suffix
            self.text.delete(self.completion_start, self.completion_end)
            self.text.insert(self.completion_start, self.completion_base + suffix)
            
            self.output_log(f"Inserted: {self.completion_base}{suffix}")
        
        self.hide_completions()
        return "break"
    
    def insert_categorized_completion(self, category):
        """Insert categorized completion"""
        listbox = self.category_listboxes[category]
        selection = listbox.curselection()
        
        if selection:
            idx = selection[0]
            suffix = self.category_items[category][idx]
            
            # Replace the word
            self.text.delete(self.completion_start, self.completion_end)
            self.text.insert(self.completion_start, self.completion_base + suffix)
            
            self.output_log(f"Inserted: {self.completion_base}{suffix}")
        
        self.hide_completions()
        return "break"
    
    def hide_completions(self, event=None):
        """Hide completion window and return focus to the text editor."""
        if self.suffix_window:
            self.suffix_window.destroy()
            self.suffix_window = None
        
        self.text.focus_set() 
        
        return "break"
    
    def new_file(self):
        """Create new file"""
        self.text.delete('1.0', tk.END)
        self.current_file = None
        self.root.title("Dust Editor - New File")
        self.output_log("New file created")
    
    def open_file(self):
        """Open a Dust file"""
        filename = filedialog.askopenfilename(
            defaultextension=".dust",
            filetypes=[("Dust files", "*.dust"), ("C files", "*.c"), ("All files", "*.*")]
        )
        if filename:
            try:
                with open(filename, 'r') as f:
                    content = f.read()
                self.text.delete('1.0', tk.END)
                self.text.insert('1.0', content)
                self.current_file = filename
                self.root.title(f"Dust Editor - {os.path.basename(filename)}")
                self.output_log(f"Opened: {filename}")
                self.on_text_change()
            except Exception as e:
                messagebox.showerror("Error", f"Failed to open file: {e}")
    
    def save_file(self):
        """Save current file"""
        if self.current_file:
            self.save_to_file(self.current_file)
        else:
            self.save_as_file()
    
    def save_as_file(self):
        """Save with new name"""
        filename = filedialog.asksaveasfilename(
            defaultextension=".dust",
            filetypes=[("Dust files", "*.dust"), ("C files", "*.c"), ("All files", "*.*")]
        )
        if filename:
            self.save_to_file(filename)
            self.current_file = filename
            self.root.title(f"Dust Editor - {os.path.basename(filename)}")
    
    def save_to_file(self, filename):
        """Save content to file"""
        try:
            content = self.text.get('1.0', tk.END)
            with open(filename, 'w') as f:
                f.write(content)
            self.output_log(f"Saved: {filename}")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to save file: {e}")
    
    def compile_dust(self):
        """Compile Dust to C using dustc"""
        if not self.current_file:
            self.output_log("Please save the file first")
            return
        
        if not self.current_file.endswith('.dust'):
            self.output_log("Not a .dust file")
            return
        
        self.save_file()
        self.output_clear()
        self.output_log("Compiling with dustc...")
        
        try:
            result = subprocess.run(
                ['./dustc', self.current_file],
                capture_output=True,
                text=True,
                cwd=os.path.dirname(self.current_file) or '.'
            )
            
            if result.stdout:
                self.output_log(result.stdout)
            if result.stderr:
                self.output_log(result.stderr)
                
            if result.returncode == 0:
                c_file = self.current_file.replace('.dust', '.c')
                self.output_log(f"✓ Compiled to {os.path.basename(c_file)}")
            else:
                self.output_log("✗ Compilation failed")
                
        except FileNotFoundError:
            self.output_log("dustc not found. Make sure it's in the current directory or PATH")
        except Exception as e:
            self.output_log(f"Error: {e}")
    
    def compile_to_exe(self):
        """Compile C to executable using gcc"""
        if not self.current_file:
            self.output_log("No file loaded")
            return
        
        # Determine C file
        if self.current_file.endswith('.dust'):
            c_file = self.current_file.replace('.dust', '.c')
        elif self.current_file.endswith('.c'):
            c_file = self.current_file
        else:
            self.output_log("Not a .dust or .c file")
            return
        
        if not os.path.exists(c_file):
            self.output_log(f"C file not found: {c_file}")
            self.output_log("Run dustc first (F5)")
            return
        
        exe_file = c_file.replace('.c', '')
        self.output_log(f"Compiling {os.path.basename(c_file)} with gcc...")
        
        try:
            result = subprocess.run(
                ['gcc', '-o', exe_file, c_file, '-Wall', '-Wextra'],
                capture_output=True,
                text=True,
                cwd=os.path.dirname(c_file) or '.'
            )
            
            if result.stdout:
                self.output_log(result.stdout)
            if result.stderr:
                self.output_log(result.stderr)
                
            if result.returncode == 0:
                self.output_log(f"✓ Created executable: {os.path.basename(exe_file)}")
            else:
                self.output_log("✗ GCC compilation failed")
                
        except FileNotFoundError:
            self.output_log("gcc not found. Please install gcc")
        except Exception as e:
            self.output_log(f"Error: {e}")
    
    def run_program(self):
        """Run the compiled executable"""
        if not self.current_file:
            self.output_log("No file loaded")
            return
        
        # Determine executable file
        if self.current_file.endswith('.dust'):
            exe_file = self.current_file.replace('.dust', '')
        elif self.current_file.endswith('.c'):
            exe_file = self.current_file.replace('.c', '')
        else:
            exe_file = self.current_file
        
        if not os.path.exists(exe_file):
            self.output_log(f"Executable not found: {exe_file}")
            self.output_log("Compile with gcc first (F6)")
            return
        
        self.output_log(f"Running {os.path.basename(exe_file)}...")
        self.output_log("-" * 40)
        
        try:
            result = subprocess.run(
                [f'./{os.path.basename(exe_file)}'],
                capture_output=True,
                text=True,
                timeout=5,
                cwd=os.path.dirname(exe_file) or '.'
            )
            
            if result.stdout:
                self.output_log(result.stdout)
            if result.stderr:
                self.output_log(f"[stderr] {result.stderr}")
                
            self.output_log("-" * 40)
            self.output_log(f"Program exited with code: {result.returncode}")
            
        except subprocess.TimeoutExpired:
            self.output_log("Program timed out after 5 seconds")
        except Exception as e:
            self.output_log(f"Error running program: {e}")
    
    def full_build_run(self):
        """Full build and run pipeline"""
        self.output_clear()
        self.output_log("=== Full Build & Run ===")
        
        # Step 1: dustc
        self.compile_dust()
        
        # Step 2: gcc (if dustc succeeded)
        if self.current_file and self.current_file.endswith('.dust'):
            c_file = self.current_file.replace('.dust', '.c')
            if os.path.exists(c_file):
                self.compile_to_exe()
                
                # Step 3: run (if gcc succeeded)
                exe_file = c_file.replace('.c', '')
                if os.path.exists(exe_file):
                    self.output_log("")
                    self.run_program()


if __name__ == "__main__":
    root = tk.Tk()
    editor = DustEditor(root)
    
    # Set initial size
    root.geometry("1000x700")
    
    # Focus on text editor
    editor.text.focus()
    
    root.mainloop()
