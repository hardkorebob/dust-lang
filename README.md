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
// test_complete.dust - Comprehensive Dust language test
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Test struct definition
struct Player {
    x_i
    y_i
    health_u32
    name_cp
    inventory_ia[10]
    active_bl
}

// Test enum
enum GameState {
    MENU = 0,
    PLAYING = 1,
    PAUSED,
    GAME_OVER
}

// Test typedef
typedef dummy_u32 Score

// Test function returning int
func add_i(a_i, b_i) {
    return a_i + b_i
}

// Test function with array parameter
func sum_array_i(arr_ia, size_st) {
    let total_i = 0
    for (let i_st = 0 ; i_st < size_st ; i_st++) {
        total_i += arr_ia[i_st]
    }
    return total_i
}

// Test function with struct pointer
func update_player_v(p_Playerp) {
    p_Playerp->x++
    p_Playerp->y++
    p_Playerp->health -= 10
}

// Test all operators
func test_operators_v() {
    let a_i = 10
    let b_i = 3
    
    // Arithmetic
    let sum_i = a_i + b_i
    let diff_i = a_i - b_i
    let prod_i = a_i * b_i
    let quot_i = a_i / b_i
    let rem_i = a_i % b_i
    
    // Bitwise
    let andd_i = a_i & b_i
    let orr_i = a_i | b_i
    let xor_i = a_i ^ b_i
    let lshift_i = a_i << 2
    let rshift_i = a_i >> 1
    
    // Compound assignments
    a_i += 5
    a_i -= 2
    a_i *= 2
    a_i /= 3
    a_i &= 0xFF
    a_i |= 0x10
    a_i ^= 0x0F
    a_i <<= 1
    a_i >>= 1
    
    // Increment/Decrement
    let pre_i = ++a_i
    let post_i = b_i++
    --a_i
    b_i--
    
    // Comparison
    let eq_bl = (a_i == b_i)
    let neq_bl = (a_i != b_i)
    let lt_bl = (a_i < b_i)
    let gt_bl = (a_i > b_i)
    let lte_bl = (a_i <= b_i)
    let gte_bl = (a_i >= b_i)
    
    // Logical
    let and_bl = (a_i > 5) && (b_i < 10)
    let or_bl = (a_i > 5) || (b_i < 10)
    let not_bl = !(a_i == b_i)
    
    // Ternary
    let max_i = (a_i > b_i) ? a_i : b_i
    
    printf("Operators test complete\n")
}

// Test control flow
func test_control_flow_v() {
    let state_GameState = PLAYING_GameState
    
    // Switch on enum
    switch (state_GameState) {
        case MENU_GameState:
            printf("In menu\n")
            break
        case PLAYING_GameState:
            printf("Playing game\n")
            break
        case PAUSED_GameState:
            printf("Game paused\n")
            break
        case GAME_OVER_GameState:
            printf("Game over\n")
            break
        default:
            printf("Unknown state\n")
    }
    
    // While loop
    let count_i = 0
    while (count_i < 5) {
        count_i++
    }
    
    // Do-while
    do {
        count_i--
    } while (count_i > 0)
    
    // For loop
    for (let i_i = 0 ; i_i < 10 ; i_i++) {
        if (i_i == 5) {
            continue
        }
        if (i_i == 8) {
            break
        }
    }
}

// Test arrays and pointers
func test_arrays_v() {
    let numbers_ia[5] = {1, 2, 3, 4, 5}
    let buffer_u8a[256]
    let message_ca = "Hello, Dust!"
    
    // Array indexing
    numbers_ia[0] = 10
    buffer_u8a[0] = 0xFF
    
    // Pointer arithmetic
    let ptr_ip = &numbers_ia[0]
    let val_i = *ptr_ip
    ptr_ip++
    
    // String as char pointer
    let name_s = "Dust Language"
    let first_c = name_s[0]
}

func main_i() {
    printf("=== Dust Compiler Test Suite ===\n\n")
    
    // Test basic operations
    let result_i = add_i(5, 3)
    printf("add(5, 3) = %d\n", result_i)
    
    // Test struct
    let player_Player
    player_Player.x = 100
    player_Player.y = 200
    player_Player.health = 100
    player_Player.name = "Hero"
    player_Player.active = 1
    
    printf("Player at (%d, %d) with %d health\n",player_Player.x, player_Player.y, player_Player.health)
    
    update_player_v(&player_Player)
    printf("After update: (%d, %d) with %d health\n",player_Player.x, player_Player.y, player_Player.health)
    
    // Test array operations
    let nums_ia[3] = {10, 20, 30}
    let sum_i = sum_array_i(nums_ia, 3)
    printf("Array sum: %d\n", sum_i)
    
    // Run test suites
    test_operators_v()
    test_control_flow()
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

C already has a type checker. Dust just makes sure C can see the types clearly by putting them in the names. No abstraction. No inference. No tracking. Just naming.

---
**HAHAHA! Never has my jaw dropped more...are you guys joking?**

RUST REQUIREMENTS:
There are no strict hardware requirements: 

 - 30GB+ of free disk space. 
 - 8GB+ RAM
 - 2+ cores. Having more cores really helps. 10 or 20 or more is not too many!

DUST REQUIREMENTS:
 
 - 10MB of free disk space 
 - Computer not older than 2005
 - 25MB RAM
