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

## What Dust Actually Is

Dust is a C-like language where every identifier must carry its type as a suffix. The compiler enforces this convention, validates type compatibility, and transpiles to standard C. It's essentially a strict, compiler-enforced Hungarian notation system with its own syntax.

### What We Built
- A full lexer, parser, and AST generator
- A type checker that validates suffix consistency 
- A code generator that emits clean C
- ~4000 lines of C that can compile itself

### What We Claimed vs Reality
- **Claim**: "Transcends complexity through simplicity"  
  **Reality**: Moves complexity from type syntax to naming rules
  
- **Claim**: "No type checker needed"  
  **Reality**: We built a 1000+ line type checker because it was needed
  
- **Claim**: "The foundation is already here"  
  **Reality**: We built an entire compiler infrastructure on top of C

## The Actual Principles

### 1. Types in Names
Every variable must encode its type in its suffix:
- `count_i` - integer
- `name_s` - string (char*)  
- `buffer_u8a` - uint8_t array
- `player_Playerp` - Player pointer

The compiler enforces this strictly. No suffix = compilation error.

### 2. The Suffix System
We built a sophisticated suffix parser that handles:
- Basic types: `_i`, `_f`, `_s`, `_bl` (int, float, string, bool)
- Pointers: `_ip`, `_cp`, `_vp` (int*, char*, void*)
- Arrays: `_ia`, `_fa`, `_u8a` (int[], float[], uint8_t[])
- User types: `_Player`, `_Playerp`, `_Playera`
- Modifiers: `z` prefix for static, `k` for const

### 3. What This Achieves
- **Extreme readability**: You always know a variable's type
- **Catch naming errors**: Mismatched suffixes = compiler error
- **Self-documenting code**: Types are literally in the names
- **C compatibility**: Transpiles to standard, portable C

## The Escape Hatch
```c
@c( /* raw C code here */ )
```
When our suffix system can't express something, drop down to raw C.

## Example Code
```c
struct Player {
    health_i
    position_f
    name_s
}

func update_player_v(player_Playerp, delta_f) {
    player_Playerp->health_i = player_Playerp->health_i - 1
    player_Playerp->position_f = player_Playerp->position_f + delta_f
}

func main_i() {
    let player_Player
    player_Player.health_i = 100
    update_player_v(&player_Player, 0.016)
    return 0
}
```

## Performance Reality

The Dust compiler is actually quite efficient:
- Single-pass lexer with arena allocation
- Type checking in O(n) with hash-table symbol lookup  
- Generates C code directly without intermediate representations
- Compiles 1000s of lines per second on modest hardware

## The Requirements Comparison (This Part is True!)

**RUST REQUIREMENTS:**
- 30GB+ of free disk space
- 8GB+ RAM  
- 2+ cores (10-20 recommended)

**DUST REQUIREMENTS:**
- 10MB of free disk space
- Any computer from 2005+
- 20MB RAM
- Compiles itself in <1 second

## What We Learned

Building Dust taught us that:
1. Enforced naming conventions can provide real type safety
2. You still need a type checker even when types are in names
3. Simple ideas (types-in-names) require complex implementation
4. The C type system is more nuanced than it first appears
5. Arena allocators make compiler writing much easier

## Should You Use Dust?

**Use Dust if you:**
- Love Hungarian notation and want it enforced
- Value extreme code readability over brevity
- Want to learn how compilers work
- Think variable names should tell the whole story

**Don't use Dust if you:**
- Prefer type inference
- Value concise identifiers
- Need a mature ecosystem
- Want actual memory safety (it's still just C underneath)

## The Bottom Line

Dust is a real, working transpiler that enforces a radical naming convention. It won't revolutionize systems programming, but it does prove an interesting point: **making type information visible in names, and checking it rigorously, can create a unique form of type safety**.

We built something that shouldn't exist but does. It compiles. It type-checks. It can compile itself. 

And yes, it really does run on a potato while Rust needs a data center.

**Build with what you can see. Build with mandatory Hungarian notation. Build with Dust.**

---

*"From dust we came, to dust we transpile"* ✨
