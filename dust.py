# dust_editor_refactored.py
import tkinter as tk
from tkinter import ttk, scrolledtext
import re

class DustEditor:
    def __init__(self, root):
        self.root = root
        self.root.title("Dust Editor - Shift+Space for suffix completion")
        
        # Acme editor color scheme
        self.colors = {
            'bg': '#ffffea',      # Pale yellow background
            'fg': '#000000',      # Black text
            'select_bg': '#d0d0d0', # Light gray selection
            'status_bg': '#e8e8e8', # Status bar background
            'listbox_bg': '#ffffea', # Listbox background
            'listbox_fg': '#000000', # Listbox text
            'listbox_select': '#d0d0d0', # Listbox selection
        }
        
        # Configure style
        self.style = ttk.Style()
        self.style.configure('TFrame', background=self.colors['bg'])
        self.style.configure('TLabel', background=self.colors['status_bg'], foreground=self.colors['fg'])
        
        # Common Dust suffixes organized by category
        self.suffix_categories = {
            'Primitives': {
                'i ': 'int',
                'bl ': 'bool', 
                'f ': 'float',
                'c ': 'char',
                's ': 'string (char*)',
                'v ': 'void',
            },
            'Pointers': {
                'ip ': 'int pointer',
                'cp ': 'char pointer',
                'vp ': 'void pointer',
                'fp ': 'function pointer',
                'p ': 'owned pointer (suffix)',
                'b ': 'borrowed pointer (suffix)',
                'r ': 'reference pointer (suffix)',
            },
            'Arrays': {
                'ia ': 'int array',
                'ca ': 'char array',
                'u8a ': 'uint8_t array',
            },
            'Fixed-width': {
                'u8 ': 'uint8_t',
                'u16 ': 'uint16_t',
                'u32 ': 'uint32_t',
                'u64 ': 'uint64_t',
                'st ': 'size_t',
                'ux ': 'uintptr_t',
            }
        }
        
        # Flatten suffixes for easy access
        self.suffixes = {k: v for category in self.suffix_categories.values() for k, v in category.items()}
        
        self.suffix_window = None
        self.setup_ui()
        
    def setup_ui(self):
        # Configure root background
        self.root.configure(bg=self.colors['bg'])
        
        # Menu frame
        menu_frame = ttk.Frame(self.root)
        menu_frame.pack(fill=tk.X, padx=5, pady=5)
        
        ttk.Label(menu_frame, text="Type identifier_ then Shift+Space for suffix completion").pack(side=tk.LEFT)
        
        # Text editor
        self.text = scrolledtext.ScrolledText(
            self.root, 
            wrap=tk.WORD,
            width=80,
            height=30,
            font=('DejaVu Sans Mono', 10),  # Monospace font similar to Acme
            undo=True,
            bg=self.colors['bg'],
            fg=self.colors['fg'],
            insertbackground=self.colors['fg'],  # Cursor color
            selectbackground=self.colors['select_bg']
        )
        self.text.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Bind keys
        self.text.bind('<Shift-space>', self.show_completions)
        self.root.bind('<Escape>', self.hide_completions)
        self.root.bind('<Return>', self.select_completion)
        self.text.bind('<KeyRelease>', self._on_key_release)
        
        # Status bar
        self.status = ttk.Label(self.root, text="Ready", relief=tk.SUNKEN)
        self.status.pack(fill=tk.X, side=tk.BOTTOM)
        
        # Configure tags for syntax highlighting
        self._configure_text_tags()
        
    def _configure_text_tags(self):
        """Configure text tags for syntax highlighting"""
        # Comment highlighting
        self.text.tag_configure("comment", foreground="#505050")
        # String highlighting
        self.text.tag_configure("string", foreground="#007f00")
        
    def _on_key_release(self, event):
        """Handle syntax highlighting on key release"""
        self._highlight_syntax()
        
    def _highlight_syntax(self):
        """Basic syntax highlighting for comments and strings"""
        # Remove previous tags
        self.text.tag_remove("comment", "1.0", tk.END)
        self.text.tag_remove("string", "1.0", tk.END)
        
        # Get all text
        text_content = self.text.get("1.0", tk.END)
        
        # Highlight comments (// until end of line)
        for match in re.finditer(r"//.*", text_content):
            start = f"1.0+{match.start()}c"
            end = f"1.0+{match.end()}c"
            self.text.tag_add("comment", start, end)
            
        # Highlight strings (between double quotes)
        for match in re.finditer(r"\".*?\"", text_content):
            start = f"1.0+{match.start()}c"
            end = f"1.0+{match.end()}c"
            self.text.tag_add("string", start, end)
    
    def get_current_word(self):
        """Get the word at cursor position"""
        try:
            insert_pos = self.text.index(tk.INSERT)
            line_num, col_num = map(int, insert_pos.split('.'))
            
            # Get the current line text
            line_start = f"{line_num}.0"
            line_end = f"{line_num}.end"
            line_text = self.text.get(line_start, line_end)
            
            # Find word at cursor position
            if col_num > len(line_text):
                col_num = len(line_text)
                
            # Find start of word
            start_col = col_num
            while start_col > 0 and (line_text[start_col-1].isalnum() or line_text[start_col-1] == '_'):
                start_col -= 1
                
            # Get the word
            word = line_text[start_col:col_num]
            word_start = f"{line_num}.{start_col}"
            word_end = f"{line_num}.{col_num}"
            
            return word, word_start, word_end
            
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
            category_name = "User-defined Types"
            categorized_suffixes = {category_name: dict(suffixes)}
        else:
            # Regular suffixes
            categorized_suffixes = self.suffix_categories
            
        self.show_suffix_menu(categorized_suffixes, start_pos, end_pos)
        
    def hide_completions(self, event=None):
        """Hide the completion window and unbind navigation keys"""
        try:
            # Unbind navigation keys
            self.text.unbind('<Up>')
            self.text.unbind('<Down>')
            self.text.unbind('<Return>')
            
            if self.suffix_window:
                self.suffix_window.destroy()
        except:
            pass
        finally:
            self.suffix_window = None
        return "break"

    def show_suffix_menu(self, categorized_suffixes, start_pos, end_pos):
        """Display categorized suffix selection menu"""
        try:
            self.suffix_window = tk.Toplevel(self.root)
            self.suffix_window.wm_overrideredirect(True)
            self.suffix_window.wm_attributes("-topmost", True)
            self.suffix_window.configure(bg=self.colors['bg'])
            
            # Store data for later use
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
                
            # Create notebook with categories
            notebook = ttk.Notebook(self.suffix_window)
            notebook.pack(padx=2, pady=2)
            
            # Create a frame for each category
            self.category_frames = {}
            self.category_listboxes = {}
            self.all_suffixes = []  # Flat list of all suffixes for navigation
            
            for category_name, suffixes in categorized_suffixes.items():
                frame = ttk.Frame(notebook)
                self.category_frames[category_name] = frame
                notebook.add(frame, text=category_name)
                
                # Create listbox for this category
                listbox = tk.Listbox(
                    frame,
                    height=min(len(suffixes), 10),
                    width=50,
                    font=('DejaVu Sans Mono', 9),
                    selectmode=tk.SINGLE,
                    activestyle='dotbox',
                    bg=self.colors['listbox_bg'],
                    fg=self.colors['listbox_fg'],
                    selectbackground=self.colors['listbox_select']
                )
                
                # Add scrollbar if needed
                if len(suffixes) > 10:
                    scrollbar = ttk.Scrollbar(frame, orient=tk.VERTICAL, command=listbox.yview)
                    listbox.configure(yscrollcommand=scrollbar.set)
                    scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
                
                listbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
                self.category_listboxes[category_name] = listbox
                
                # Add items to listbox
                for suffix, description in suffixes.items():
                    display = f"{suffix:<12} - {description}"
                    listbox.insert(tk.END, display)
                    self.all_suffixes.append((suffix, description, category_name))
                
                # Select first item
                if suffixes:
                    listbox.selection_set(0)
                    listbox.activate(0)
                
                # Bind events
                listbox.bind('<Double-Button-1>', self.select_completion)
                listbox.bind('<Button-1>', self.on_listbox_click)
            
            # Position window
            self.suffix_window.geometry(f"+{x}+{y}")
            
            # Bind navigation keys to the main text widget
            self.text.bind('<Up>', self.navigate_up)
            self.text.bind('<Down>', self.navigate_down)
            self.text.bind('<Return>', self.select_completion)
            self.text.bind('<Tab>', self.switch_category)
            
        except Exception as e:
            print(f"Error showing menu: {e}")
            self.hide_completions()

    def switch_category(self, event):
        """Switch between categories using Tab key"""
        if not self.suffix_window:
            return
            
        notebook = self.suffix_window.winfo_children()[0]
        current_tab = notebook.index(notebook.select())
        next_tab = (current_tab + 1) % len(notebook.tabs())
        notebook.select(next_tab)
        
        # Update selection in the new tab
        category_name = notebook.tab(next_tab, "text")
        listbox = self.category_listboxes[category_name]
        listbox.selection_clear(0, tk.END)
        listbox.selection_set(0)
        listbox.activate(0)
        
        return "break"

    def on_listbox_click(self, event):
        """Handle single click on listbox"""
        widget = event.widget
        widget.selection_clear(0, tk.END)
        widget.selection_set(widget.nearest(event.y))

    def navigate_up(self, event):
        """Navigate up in completion list"""
        if not self.suffix_window:
            return
            
        notebook = self.suffix_window.winfo_children()[0]
        current_tab = notebook.index(notebook.select())
        category_name = notebook.tab(current_tab, "text")
        listbox = self.category_listboxes[category_name]
        
        current = listbox.curselection()
        if not current:
            listbox.selection_set(0)
        else:
            idx = current[0]
            if idx > 0:
                listbox.selection_clear(idx)
                listbox.selection_set(idx - 1)
                listbox.activate(idx - 1)
                listbox.see(idx - 1)
        return "break"

    def navigate_down(self, event):
        """Navigate down in completion list"""
        if not self.suffix_window:
            return
            
        notebook = self.suffix_window.winfo_children()[0]
        current_tab = notebook.index(notebook.select())
        category_name = notebook.tab(current_tab, "text")
        listbox = self.category_listboxes[category_name]
        
        current = listbox.curselection()
        if not current:
            listbox.selection_set(0)
        else:
            idx = current[0]
            if idx < listbox.size() - 1:
                listbox.selection_clear(idx)
                listbox.selection_set(idx + 1)
                listbox.activate(idx + 1)
                listbox.see(idx + 1)
        return "break"

    def select_completion(self, event=None):
        """Insert selected completion"""
        if not self.suffix_window:
            return
            
        try:
            notebook = self.suffix_window.winfo_children()[0]
            current_tab = notebook.index(notebook.select())
            category_name = notebook.tab(current_tab, "text")
            listbox = self.category_listboxes[category_name]
            
            selection = listbox.curselection()
            if selection:
                idx = selection[0]
                suffix = list(list(self.suffix_categories[category_name].keys())[idx])
                
                # Replace word with completed version
                self.text.delete(self.current_start, self.current_end)
                self.text.insert(self.current_start, suffix)
                
                # Move cursor to end of inserted text
                new_pos = f"{self.current_start} +{len(suffix)}c"
                self.text.mark_set(tk.INSERT, new_pos)
                
                self.status.config(text=f"Inserted: {suffix}")
                
        except Exception as e:
            print(f"Error selecting: {e}")
        finally:
            self.hide_completions()
        return "break"
        
            
if __name__ == "__main__":
    root = tk.Tk()
    editor = DustEditor(root)
    
    # Sample Dust code
    sample_code = """// Dust Editor - Type 'name_' then Shift+Space
func process_data(buf_: &Buffer, count_: i32) -> bool {
    // Process the data buffer
    let result_ = false;
    let message_ = "Processing complete";
    
    if count_ > 0 {
        for i in 0..count_ {
            // Process each element
            let element_ = buf_[i];
            result_ = process_element(element_);
        }
    }
    
    return result_;
}

struct Player_ {
    name_: String,
    score_: i32,
    active_: bool,
}

let player1_: Player_;
let players_: [Player_; 10];
"""
    
    editor.text.insert("1.0", sample_code)
    editor._highlight_syntax()  # Apply syntax highlighting to sample code
    
    root.mainloop()
