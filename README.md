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

**Dust is a systems programming language built on this principle.**  

It is an "ancient C" for the modern world, designed not to fight complexity, but to transcend it through simplicity and clarity.  

---

## Principles of Dust

### 1. Grammar over Punctuation
Modern languages are a fortress of punctuation: `()`, `{}`, `[]`, `;`, `::`.  
This syntax, while precise, creates cognitive overhead and visual noise. Dust replaces this complexity with the intuitive power of grammar, inspired by the timeless structure of Sanskrit.

- **Identifiers are Nouns**: Variables are the "things" in our programs.
- **Keywords are Verbs**: Control flow words are the "actions" (`if`, `for`, `return`).
- **Suffixes are Case Endings**: A single, unambiguous letter at the end of a noun defines its complete set of properties—its **Type** and its **Semantic Role**.  


---

### 2. An Entity-Component System for Code
Dust is designed like a modern **Entity Component System (ECS)**. This is not just an analogy; it is the **core architecture** of the compiler.

- **Entity**: The base name of an identifier (`player`).
- **Components**: The properties attached to that entity, defined by its suffix (`Type: int`, `Role: agent`).
- **System**: The compiler itself, which operates on these entities based on their components.

This makes the language transparent. The code you write is a direct representation of the data and its roles, with no hidden abstractions.

---

### 3. Simplicity is the Ultimate Safeguard
Rust ensures safety through a sophisticated compiler that enforces complex ownership and lifetime rules. This is a valid and powerful approach.  

Dust ensures safety through **radical simplicity**.

By eliminating syntactic ambiguity and making the role and type of every piece of data explicit in its name, the possibility for many classes of errors is removed at the design level.  

The code is not just safe; it is **transparently correct**.

---

## The Vision
Dust is for systems programmers who believe that the best solution is often the simplest one. 
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
```

Will you please help me clean up all this Dust?

<3 
---


