```
A transpiled language that explores radical type transparency through mandatory type suffixes, trading verbosity for absolute clarity about data types at every point in the code.
The technical implementation is impressive.
The philosophical claims about transcending complexity and eliminating type checking don't match the reality of what the code does.
You've built an enforced naming convention, not a new paradigm for type safety.
```

# The Dust Manifesto

From dust our bodies spring and to dust we return. 

It is the alpha and the omega, the substrate of all creation. 

It does not need to be invented; it needs only to be rediscovered.

In the digital realm, we have forgotten this. We build towers of complexity, languages of fleeting abstraction that fight against their own nature. 

We seek to conquer rust, decay, and error with ever-more-intricate machinery.  

Dust is not something that settles on things; it is the fundamental material of things. 

It is ancient, ubiquitous, and enduring. 

It does not decay. It is the foundation.  

It is an "ancient C" for the modern world, designed not to fight complexity, but to transcend it through simplicity and clarity.

**Dust is a systems programming language built on this principle.** 

**Dust offers a return to the source.**  

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
// test_self_host.dust
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct Arena {
    data_cp
    size_st
    used_st
}


let g_arena_Arena

func arena_init_v(size_st) {
    g_arena_Arena.data_cp = malloc(size_st)
    g_arena_Arena.size_st = size_st
    g_arena_Arena.used_st = 0
}

func arena_alloc_vp(size_st) {

    size_st = (size_st + 7) & ~7
    
    if (g_arena_Arena.used_st + size_st > g_arena_Arena.size_st) {
        fprintf(stderr, "Arena out of memory\n")
        exit(1)
    }
    
    let ptr_vp = g_arena_Arena.data_cp + g_arena_Arena.used_st
    g_arena_Arena.used_st = g_arena_Arena.used_st + size_st
    memset(ptr_vp, 0, size_st)
    return ptr_vp
}

func str_cmp_i(a_s, b_s) {
    let i_st = 0
    while (a_s[i_st] != '\0' && b_s[i_st] != '\0') {
        if (a_s[i_st] != b_s[i_st]) {
            return a_s[i_st] - b_s[i_st]
        }
        i_st = i_st + 1
    }

    return a_s[i_st] - b_s[i_st]
}


func emit_identifier_v(node_vp) {
    fprintf(stdout, "%s", cast_s(node_vp))
}

func emit_number_v(node_vp) {
    fprintf(stdout, "%s", cast_s(node_vp))
}


let emit_table_fpa[] = {
    emit_identifier_v 
    emit_number_v 
    null
    null
}

struct Lexer {
    source_s
    pos_i
    len_i
    line_i
}


func compile_i(source_s) {
    let lex_Lexerp = arena_alloc_vp(sizeof(Lexer))
    lex_Lexerp->source_s = source_s
    lex_Lexerp->pos_i = 0
    lex_Lexerp->len_i = strlen(source_s)
    lex_Lexerp->line_i = 1
    
    let token_count_i = 0
    
    while (lex_Lexerp->pos_i < lex_Lexerp->len_i) {
        let c_c = lex_Lexerp->source_s[lex_Lexerp->pos_i]
        
        if (isalpha(c_c)) {
            while (isalnum(lex_Lexerp->source_s[lex_Lexerp->pos_i])) {
                lex_Lexerp->pos_i = lex_Lexerp->pos_i + 1
            }
            token_count_i = token_count_i + 1
        } else if (isdigit(c_c)) {
            while (isdigit(lex_Lexerp->source_s[lex_Lexerp->pos_i])) {
                lex_Lexerp->pos_i = lex_Lexerp->pos_i + 1
            }
            token_count_i = token_count_i + 1
        } else {
            lex_Lexerp->pos_i = lex_Lexerp->pos_i + 1
        }
    }
    return token_count_i
}

func main_i() {
    arena_init_v(1024 * 1024)
    
    printf("Testing Dust compiler...\n");

    let test_s = "func test_i() { return 42 }"
    let tokens_i = compile_i(test_s)
    
    printf("Tokenized %d tokens\n", tokens_i)
    
    if (emit_table_fpa[0] != null) {
        printf("Dispatch table initialized\n")
    }
    
    if (str_cmp_i("dust", "dust") == 0) {
        printf("String comparison works\n")
    }
    return 0
}
```

Will you please help me clean up all this Dust?

<3 
---

## Testing...testing...type check?

"Dust does not fight complexity. It transcends it."

### Philosophy in Dust

1. **"Data dominates"** - In Dust, the identifier IS the data structure. `player_i` isn't a variable with hidden type metadata; it's an integer named player. The data (the suffix) dominates the design.

2. **"A little copying is better than a little dependency"** - Dust doesn't depend on a symbol table, type inference engine, or semantic analyzer. Each identifier carries its complete type information. Yes, you "copy" the type suffix everywhere, but this eliminates entire categories of dependencies. A simple tkinter/python editor can make this NULL.

3. **"Don't communicate by sharing memory; share memory by communicating"** - Traditional compilers share memory (symbol tables) between phases. Dust communicates everything through the identifier itself. The lexer tells the parser the type. The parser tells the codegen. No shared state.

4. **"The bigger the interface, the weaker the abstraction"** - C's type system has a massive interface (declarations, casts, implicit conversions, promotion rules). Dust has one interface: the suffix. That's it.

### Why No Type Checker Is Necessary

`let token_type_TokenType = IDENTIFIER_TokenType;`

Three things happen:
1. Dust sees `_TokenType` suffix and knows it's that enum type.
2. Dust emits correct C: `TokenType token_type = IDENTIFIER;`
3. The C compiler validates everything.

What would a type checker add here? Nothing. The invalid states are already impossible:
- Wrong suffix? Dust won't recognize it, C compiler catches it.
- Type mismatch? The suffixes make it obvious, C compiler catches it.
- Undefined variable? C compiler catches it.

### The Philosophical Breakthrough

Traditional languages fight a war on two fronts:
1. They hide type information from the programmer (inference).
2. They build complex machinery to track what they hid.

Dust surrenders this war entirely:
1. Types are explicit, always, in the name.
2. Therefore no tracking is needed.

This is like Unix philosophy: instead of building smart programs that guess what you want, build simple programs that do exactly what you say. Dust doesn't guess types; you declare them in every identifier.

### Maybe... Pike Would Approve

He wrote: "Fancy algorithms are slow when n is small, and n is usually small." 

A type checker is a fancy algorithm. Dust's suffix system is not an algorithm at all - it's a naming convention enforced by simple string matching. When every variable carries its type, you don't need algorithms to figure out types.

The test output proves it: your program compiles and runs correctly with zero type checking beyond what C already provides. The suffixes make the code self-documenting, self-checking, and transparent.

**"The foundation is already here. It has always been here."**

C already has a type checker. Dust just makes sure C can see the types clearly by putting them in the names.

No abstraction. 

No inference. 

No tracking. 

Just naming... And names are powerful!

---
**HAHAHA! Never has my jaw dropped more...are you guys joking?**

_Rust folk say: There are no strict hardware requirements:_

RUST REQUIREMENTS:


 - 30GB+ of free disk space. 
 - 8GB+ RAM
 - 2+ cores. Having more cores really helps. 10 or 20 or more is not too many!

DUST REQUIREMENTS:
 
 - 10MB of free disk space 
 - Computer not older than 2005
 - 20MB RAM
