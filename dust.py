# dust_editor_fixed.py
import tkinter as tk
from tkinter import ttk, scrolledtext
import re

class DustEditor:
    def __init__(self, root):
        self.root = root
        self.root.title("Dust Editor - Shift+Space for suffix completion")
        
        # Common Dust suffixes organized by category
        self.suffixes = {
            # Primitives
            'i': 'int',
            'bl': 'bool', 
            'f': 'float',
            'c': 'char',
            's': 'string (char*)',
            'v': 'void',
            
            # Pointers
            'ip': 'int pointer',
            'cp': 'char pointer',
            'vp': 'void pointer',
            'fp': 'function pointer',
            'p': 'owned pointer (suffix)',
            'b': 'borrowed pointer (suffix)',
            'r': 'reference pointer (suffix)',
            
            # Arrays (append 'a' to base type)
            'ia': 'int array',
            'ca': 'char array',
            'u8a': 'uint8_t array',
            
            # Fixed-width
            'u8': 'uint8_t',
            'u16': 'uint16_t',
            'u32': 'uint32_t',
            'u64': 'uint64_t',
            'st': 'size_t',
            'ux': 'uintptr_t',
        }
        
        self.suffix_window = None
        self.setup_ui()
        
    def setup_ui(self):
        # Menu frame
        menu_frame = ttk.Frame(self.root)
        menu_frame.pack(fill=tk.X, padx=5, pady=5)
        
        ttk.Label(menu_frame, text="Type identifier_ then Shift+Space for suffix").pack(side=tk.LEFT)
        
        # Text editor
        self.text = scrolledtext.ScrolledText(
            self.root, 
            wrap=tk.WORD,
            width=80,
            height=30,
            font=('Consolas', 11),
            undo=True
        )
        self.text.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Bind keys
        self.text.bind('<Shift-space>', self.show_completions)
        self.root.bind('<Escape>', self.hide_completions)
        self.root.bind('<Return>', self.select_completion)
        
        # Status bar
        self.status = ttk.Label(self.root, text="Ready", relief=tk.SUNKEN)
        self.status.pack(fill=tk.X, side=tk.BOTTOM)
        
    def get_current_word(self):
        """Get the word at cursor position"""
        try:
            insert_pos = self.text.index(tk.INSERT)
            line_start = f"{insert_pos.split('.')[0]}.0"
            line_text = self.text.get(line_start, insert_pos)
            
            # Find last word boundary
            match = re.findall(r'[\w_]+$', line_text)
            if match:
                word = match[0]
                word_start = f"{insert_pos} -{len(word)}c"
                return word, word_start, insert_pos
                
        except Exception as e:
            print(f"Error getting word: {e}")
            
        return "", insert_pos, insert_pos
        
    def show_completions(self, event):
        """Show suffix completion popup"""
        # Clean up any existing window first
        self.hide_completions()
        
        word, start_pos, end_pos = self.get_current_word()
        
        if not word or not word.endswith('_'):
            self.status.config(text="Type identifier_ first")
            return "break"
            
        base_word = word[:-1]  # Remove trailing underscore
        
        # Build suffix list
        if base_word and base_word[0].isupper():
            # User-defined type suffixes
            suffixes = [
                (f'{word}{base_word}', f'{base_word} instance'),
                (f'{word}{base_word}p', f'{base_word}* (owned)'),
                (f'{word}{base_word}b', f'const {base_word}* (borrowed)'),
                (f'{word}{base_word}r', f'const {base_word}* (reference)'),
                (f'{word}{base_word}a', f'{base_word} array'),
            ]
        else:
            # Regular suffixes
            suffixes = [(f'{word}{k}', f'{v}') for k, v in self.suffixes.items()]
            
        self.show_suffix_menu(suffixes, start_pos, end_pos)
        return "break"
        
    def show_suffix_menu(self, suffixes, start_pos, end_pos):
        """Display suffix selection menu"""
        try:
            self.suffix_window = tk.Toplevel(self.root)
            self.suffix_window.wm_overrideredirect(True)
            self.suffix_window.wm_attributes("-topmost", True)
            
            # Store data for later use
            self.current_suffixes = suffixes
            self.current_start = start_pos
            self.current_end = end_pos
            
            # Position near text cursor
            bbox = self.text.bbox(tk.INSERT)
            if bbox:
                x = self.text.winfo_rootx() + bbox[0]
                y = self.text.winfo_rooty() + bbox[1] + bbox[3]
            else:
                x = self.root.winfo_x() + 100
                y = self.root.winfo_y() + 100
                
            # Create listbox with frame
            frame = ttk.Frame(self.suffix_window, relief=tk.RAISED, borderwidth=1)
            frame.pack()
            
            self.listbox = tk.Listbox(
                frame, 
                height=min(len(suffixes), 10),
                width=40,
                font=('Consolas', 10)
            )
            self.listbox.pack()
            
            for full_name, desc in suffixes:
                display = f"{full_name:<15} # {desc}"
                self.listbox.insert(tk.END, display)
                
            # Select first item
            if suffixes:
                self.listbox.selection_set(0)
                self.listbox.activate(0)
                
            # Position window
            self.suffix_window.geometry(f"+{x}+{y}")
            
            # Bind events to listbox
            self.listbox.bind('<Double-Button-1>', lambda e: self.select_completion())
            self.listbox.bind('<Up>', self.navigate_list)
            self.listbox.bind('<Down>', self.navigate_list)
            
            # Focus on main window to capture keyboard
            self.text.focus_set()
            
        except Exception as e:
            print(f"Error showing menu: {e}")
            self.hide_completions()
            
    def navigate_list(self, event):
        """Navigate the completion list with arrow keys"""
        if not self.suffix_window or not self.listbox:
            return
            
        current = self.listbox.curselection()
        if not current:
            self.listbox.selection_set(0)
            return
            
        idx = current[0]
        self.listbox.selection_clear(idx)
        
        if event.keysym == 'Up':
            new_idx = max(0, idx - 1)
        else:
            new_idx = min(self.listbox.size() - 1, idx + 1)
            
        self.listbox.selection_set(new_idx)
        self.listbox.activate(new_idx)
        self.listbox.see(new_idx)
        
    def select_completion(self, event=None):
        """Insert selected completion"""
        if not self.suffix_window or not hasattr(self, 'listbox'):
            return
            
        try:
            selection = self.listbox.curselection()
            if selection:
                idx = selection[0]
                if idx < len(self.current_suffixes):
                    full_name = self.current_suffixes[idx][0]
                    
                    # Replace word with completed version
                    self.text.delete(self.current_start, self.current_end)
                    self.text.insert(self.current_start, full_name)
                    
                    self.status.config(text=f"Inserted: {full_name}")
                    
        except Exception as e:
            print(f"Error selecting: {e}")
        finally:
            self.hide_completions()
            
    def hide_completions(self, event=None):
        """Hide the completion window"""
        try:
            if self.suffix_window:
                self.suffix_window.destroy()
        except:
            pass
        finally:
            self.suffix_window = None
            
if __name__ == "__main__":
    root = tk.Tk()
    editor = DustEditor(root)
    
    # Sample Dust code
    editor.text.insert("1.0", """// Dust Editor - Type 'name_' then Shift+Space
func process_
    let buffer_
    let player_
    let count_
    let Player_
""")
    
    root.mainloop()
