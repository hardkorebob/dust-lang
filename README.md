# The Dust Manifesto

We begin as dust, and to dust we return. 

It is the alpha and the omega, the substrate of all creation. 

It does not need to be invented; it needs only to be rediscovered.

In the digital realm, we have forgotten this. We build towers of complexity, languages of fleeting abstraction that fight against their own nature. 

We seek to conquer rust, decay, and error with ever-more-intricate machinery.  

**Dust offers a return to the source.**

Dust is not something that settles on things; it is the fundamental material of things. 

It is ancient, ubiquitous, and enduring. 

It does not decay. It is the foundation.  

It is an "ancient C" for the modern world, designed not to fight complexity, but to transcend it through simplicity and clarity.

**Dust is a systems programming language built on this principle.**    

---

## Principles of Dust

### 1. Identifiers as Grammar
C syntax is powerful but can obscure intent. The meaning of `char* name` is determined by punctuation (`*`) and word order. Dust clarifies this by embedding the core properties of a variable directly into its name, treating the identifier itself as a piece of grammar.

-   **Identifiers are Nouns**: The base name of a variable is the "thing" (`player`, `lexer`).
-   **Keywords are Verbs**: Control flow words are the "actions" (`if`, `for`, `return`).
-   **Suffixes are Adjectives**: A simple `_` suffix unambiguously defines a variable's **Type** and its **Semantic Role**. `player_i` is an integer; `player_ip` is a pointer to an integer you own.

### 2. An Entity-Component System for Code
Dust is designed like a modern **Entity Component System (ECS)**. This is not just an analogy; it is the **core architecture** of the compiler.

-   **Entity**: The base name of an identifier (`player`).
-   **Components**: The properties attached to that entity, defined by its suffix (`Type: int`, `Role: owned_pointer`).
-   **System**: The compiler itself, which operates on these entities based on their components.

This makes the language transparent. The code you write is a direct representation of the data and its roles, with no hidden abstractions.

### 3. Simplicity is the Ultimate Safeguard
Many languages ensure safety through a complex compiler that enforces sophisticated rules. This is a valid and powerful approach.

Dust ensures safety through **radical simplicity and deliberate explicitness**.

By forcing the type and role of every piece of data to be declared in its name, the possibility for many classes of errors is removed at the design level. This is a trade-off: a little more verbosity in exchange for a massive reduction in ambiguity. It is a system designed to honor our human nature—we forget things. Dust's suffixes serve as a constant, compiler-checked reminder. The code is not just safe; it is **transparently correct**.

### The Escape Hatch
For complex scenarios that don't fit the simple suffix model—like interacting with C libraries, complex function pointers, or platform-specific macros—Dust provides a pragmatic escape hatch: 

```
@c(...)
```

This allows you to embed raw C code directly into your Dust source, ensuring you are never limited by the language's core simplicity.

## The Vision
Dust is for systems programmers who believe that the best solution is often the simplest and most explicit one. 
It is for those who want a direct, unfiltered connection between their ideas and the machine.  
It is for rediscovering the foundational patterns that allow us to build software that is not just robust, but is also clear, concise, and enduring.  

**Build with what lasts. Build with Dust.**

---

## Summary
The Dust compiler leverages a **component-based system** to look up the correct C type for each identifier, effectively translating your **suffix-based clarity** into standard C.  

> Dust does not fight complexity. It transcends it.  
> The foundation is already here. It has always been here.  
> Build with Dust.

---

```c
// test9.dust - Demonstrating Dust's self-hosting capabilities
// This implements a simple lexer/token system similar to the actual compiler

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Token types (would be enum in C)
#define TOKEN_EOF 0
#define TOKEN_IDENTIFIER 1
#define TOKEN_NUMBER 2
#define TOKEN_KEYWORD 3
#define TOKEN_OPERATOR 4

// Token structure
struct Token {
    type_i;
    text_cp;
    line_i;
    next_Tokenp;  // For linked list
};

// Lexer structure
struct Lexer {
    source_cp;
    pos_i;
    len_i;
    line_i;
};

// Create a new token - clean Dust!
func make_token_Tokenp(type_i, text_cp, line_i) {
    let tok_Tokenp = cast_Tokenp(malloc(sizeof(Token)));
    if (tok_Tokenp == null) {
        return null;
    }
    
    tok_Tokenp->type = type_i;
    tok_Tokenp->line = line_i;
    tok_Tokenp->next = null;
    
    // Clone the text
    if (text_cp != null) {
        let len_i = strlen(text_cp);
        tok_Tokenp->text = cast_cp(malloc(len_i + 1));
        strcpy(tok_Tokenp->text, text_cp);
    } else {
        tok_Tokenp->text = null;
    }
    
    return tok_Tokenp;
}

// Free a token
func token_free_v(tok_Tokenp) {
    if (tok_Tokenp == null) {
        return;
    }
    
    if (tok_Tokenp->text != null) {
        free(tok_Tokenp->text);
    }
    free(tok_Tokenp);
}

// Create a lexer
func lexer_create_Lexerp(source_cp) {
    let lex_Lexerp = cast_Lexerp(malloc(sizeof(Lexer)));
    if (lex_Lexerp == null) {
        return null;
    }
    
    lex_Lexerp->source = source_cp;
    lex_Lexerp->pos = 0;
    lex_Lexerp->len = strlen(source_cp);
    lex_Lexerp->line = 1;
    
    return lex_Lexerp;
}

// Skip whitespace
func skip_whitespace_v(lex_Lexerp) {
    while (lex_Lexerp->pos < lex_Lexerp->len) {
        let c_c = lex_Lexerp->source[lex_Lexerp->pos];
        
        if (isspace(c_c)) {
            if (c_c == '\n') {
                lex_Lexerp->line = lex_Lexerp->line + 1;
            }
            lex_Lexerp->pos = lex_Lexerp->pos + 1;
        } else {
            break;
        }
    }
}

// Get next token - demonstrates complex logic in Dust
func lexer_next_Tokenp(lex_Lexerp) {
    skip_whitespace_v(lex_Lexerp);
    
    // Check for EOF
    if (lex_Lexerp->pos >= lex_Lexerp->len) {
        return make_token_Tokenp(TOKEN_EOF, "", lex_Lexerp->line);
    }
    
    let start_i = lex_Lexerp->pos;
    let c_c = lex_Lexerp->source[lex_Lexerp->pos];
    
    // Identifiers and keywords
    if (isalpha(c_c) || c_c == '_') {
        while (lex_Lexerp->pos < lex_Lexerp->len) {
            let ch_c = lex_Lexerp->source[lex_Lexerp->pos];
            if (isalnum(ch_c) || ch_c == '_') {
                lex_Lexerp->pos = lex_Lexerp->pos + 1;
            } else {
                break;
            }
        }
        
        // Extract the word
        let len_i = lex_Lexerp->pos - start_i;
        let word_cp = cast_cp(malloc(len_i + 1));
        memcpy(word_cp, lex_Lexerp->source + start_i, len_i);
        word_cp[len_i] = '\0';
        
        // Check if it's a keyword
        let type_i = TOKEN_IDENTIFIER;
        if (strcmp(word_cp, "if") == 0 || strcmp(word_cp, "while") == 0) {
            type_i = TOKEN_KEYWORD;
        }
        
        let tok_Tokenp = make_token_Tokenp(type_i, word_cp, lex_Lexerp->line);
        free(word_cp);
        return tok_Tokenp;
    }
    
    // Numbers
    if (isdigit(c_c)) {
        while (lex_Lexerp->pos < lex_Lexerp->len && isdigit(lex_Lexerp->source[lex_Lexerp->pos])) {
            lex_Lexerp->pos = lex_Lexerp->pos + 1;
        }
        
        let len_i = lex_Lexerp->pos - start_i;
        let num_cp = cast_cp(malloc(len_i + 1));
        memcpy(num_cp, lex_Lexerp->source + start_i, len_i);
        num_cp[len_i] = '\0';
        
        let tok_Tokenp = make_token_Tokenp(TOKEN_NUMBER, num_cp, lex_Lexerp->line);
        free(num_cp);
        return tok_Tokenp;
    }
    
    // Single character operator
    lex_Lexerp->pos = lex_Lexerp->pos + 1;
    let op_ca[2];
    op_c[0] = c_c;
    op_c[1] = '\0';
    
    return make_token_Tokenp(TOKEN_OPERATOR, op_ca, lex_Lexerp->line);
}

// Test string parsing capabilities
func test_string_handling_v() {
    printf("Testing string operations:\n");
    
    let test_s = "Hello, Dust!";
    let copy_cp = cast_cp(malloc(strlen(test_s) + 1));
    strcpy(copy_cp, test_s);
    
    printf("  Original: %s\n", test_s);
    printf("  Copy: %s\n", copy_cp);
    
    // Modify the copy
    copy_cp[0] = 'J';
    printf("  Modified: %s\n", copy_cp);
    
    free(copy_cp);
}

// Test array operations
func test_arrays_v() {
    printf("\nTesting array operations:\n");
    
    let nums_ia[10];
    let i_i = 0;
    
    // Initialize array
    while (i_i < 10) {
        nums_ia[i_i] = i_i * i_i;
        i_i = i_i + 1;
    }
    
    // Print some values
    printf("  nums[0] = %d\n", nums_ia[0]);
    printf("  nums[5] = %d\n", nums_ia[5]);
    printf("  nums[9] = %d\n", nums_ia[9]);
}

// Main test function
func main_i() {
    printf("=== Dust Self-Hosting Capability Test ===\n\n");
    
    // Test 1: String handling
    test_string_handling_v();
    
    // Test 2: Arrays
    test_arrays_v();
    
    // Test 3: Lexer functionality
    printf("\nTesting lexer:\n");
    let source_s = "if x 123 + while";
    let lex_Lexerp = lexer_create_Lexerp(source_s);
    
    if (lex_Lexerp == null) {
        printf("  Failed to create lexer\n");
        return 1;
    }
    
    printf("  Tokenizing: '%s'\n", source_s);
    
    // Tokenize and print
    let token_count_i = 0;
    while (1) {
        let tok_Tokenp = lexer_next_Tokenp(lex_Lexerp);
        
        if (tok_Tokenp->type == TOKEN_EOF) {
            token_free_v(tok_Tokenp);
            break;
        }
        
        printf("    Token %d: type=%d, text='%s', line=%d\n", 
               token_count_i, tok_Tokenp->type, tok_Tokenp->text, tok_Tokenp->line);
        
        token_count_i = token_count_i + 1;
        token_free_v(tok_Tokenp);
    }
    
    printf("  Total tokens: %d\n", token_count_i);
    
    // Cleanup
    free(lex_Lexerp);
    
    printf("\n=== All tests passed! ===\n");
    return 0;
}
```
```
=== Dust Self-Hosting Capability Test ===

Testing string operations:
  Original: Hello, Dust!
  Copy: Hello, Dust!
  Modified: Jello, Dust!

Testing array operations:
  nums[0] = 0
  nums[5] = 25
  nums[9] = 81

Testing lexer:
  Tokenizing: 'if x 123 + while'
    Token 0: type=3, text='if', line=1
    Token 1: type=1, text='x', line=1
    Token 2: type=2, text='123', line=1
    Token 3: type=4, text='+', line=1
    Token 4: type=3, text='while', line=1
  Total tokens: 5

=== All tests passed! ===

```

Will you please help me clean up all this Dust?

<3 
---


