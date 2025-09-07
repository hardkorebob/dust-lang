# The Dust Manifesto

We begin as dust, and to dust we return. It is the alpha and the omega, the substrate of all creation. It does not need to be invented; it needs only to be rediscovered.

In the digital realm, we have forgotten this. We build towers of complexity, languages of fleeting abstraction that fight against their own nature. We seek to conquer rust, decay, and error with ever-more-intricate machinery.  

**Dust offers a return to the source.**

Dust is everywhere. Dust doesn't rust.  
Dust is not something that settles on things; it is the fundamental material of things. It is ancient, ubiquitous, and enduring. It does not decay. It is the foundation.  

**Dust is a systems programming language built on this principle.**  
It is an "ancient C" for the modern world, designed not to fight complexity, but to transcend it through simplicity and clarity.  

---

## Principles of Dust

### 1. Grammar over Punctuation
Modern languages are a fortress of punctuation: `()`, `{}`, `[]`, `;`, `::`.  
This syntax, while precise, creates cognitive overhead and visual noise. Dust replaces this complexity with the intuitive power of grammar, inspired by the timeless structure of Sanskrit.

- **Identifiers are Nouns**: Variables are the "things" in our programs (`playera`, `messagee`).
- **Keywords are Verbs**: Control flow words are the "actions" (`if`, `for`, `return`).
- **Suffixes are Case Endings**: A single, unambiguous letter at the end of a noun defines its complete set of properties—its **Type** and its **Semantic Role**.  
  The compiler knows `counta` is an `int` acting as an agent without needing extra keywords.

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
