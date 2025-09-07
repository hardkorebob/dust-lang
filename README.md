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
// ============================================================================
// Dust Language - Advanced Test Suite
// ============================================================================
// This file tests the latest features: arrays and function pointers in structs.
// It is designed to pass with the current compiler but also highlight missing
// functionality through comments.
//
// To compile:
// 1. ./dustc test_suite_advanced.dust
// 2. gcc -o advanced_test test_suite_advanced.c
// 3. ./advanced_test
// ============================================================================

#include <stdio.h>
#include <string.h> // Needed for strcpy

// --- 1. Struct Definition: The Core Test ---
// This struct uses all the newly implemented features.
struct Widget {
    // A. Simple member
    id_i;

    // B. Array as a member (SUCCESS)
    // PROOF: The compiler should correctly parse this array member.
    name_c[32];

    // C. Function pointer members (SUCCESS)
    // PROOF: The compiler should parse these function pointer signatures.
    
    // A simple pointer: takes nothing, returns nothing (void).
    on_destroy_fp(ret_v);

    // A pointer with parameters and a return value.
    on_update_fp(ret_i, delta_time_f);

    // A pointer that takes a pointer to its own struct type. A great stress test!
    on_event_fp(ret_v, self_Widgetp);
};


// --- 2. C Helper Functions ---
// Since Dust cannot yet define functions to be assigned to pointers,
// we define them here as plain C functions to prove the pointers work.
// These functions will be linked with the Dust-generated code.

func widget_destroy_v() {
    printf("  -> Event: Widget destroyed.\n");
}

func widget_update_i(dt_f) {
    printf("  -> Event: Widget updated with dt = %f.\n", dt_f);
    return 1;
}

func widget_event_v(w_Widgetp) {
    // We can access the widget's members through the pointer.
    printf("  -> Event: on widget ID %d, Name '%s'.\n", w_Widgetp->id_i, w_Widgetp->name_c);
}


// --- 3. Main Program Entry Point ---

func main_i() {
    printf("--- Dust Advanced Test Suite Running ---\n\n");

    // --- 4. Struct Initialization and Member Access ---
    printf("1. Initializing widget...\n");
    let ui_button_Widget;

    // Test simple member assignment
    ui_button_Widget.id_i = 101;

    // Test array member assignment using a C library function
    // This proves that the C code for the struct is correctly generated.
    strcpy(ui_button_Widget.name_c, "Login Button");

    printf("Widget created with ID %d and Name '%s'.\n\n", ui_button_Widget.id_i, ui_button_Widget.name_c);

    // --- 5. Assigning to Function Pointers ---
    printf("2. Assigning function pointers...\n");

    // This is the most critical test. We are assigning our C helper functions
    // to the function pointer members of the Dust struct.
    ui_button_Widget.on_destroy_fp = &widget_destroy_v;
    ui_button_Widget.on_update_fp = &widget_update_i;
    ui_button_Widget.on_event_fp = &widget_event_v;
    
    printf("Assignments successful.\n\n");

    // --- 6. Calling Function Pointers ---
    printf("3. Calling functions via pointers...\n");
    
    // If these lines work, our implementation is a success.
    ui_button_Widget.on_event_fp(&ui_button_Widget);
    ui_button_Widget.on_update_fp(0.016);
    ui_button_Widget.on_destroy_fp();

    printf("\nFunction pointer calls successful.\n\n");

    // --- 7. Where We Are Lacking ---
    printf("4. Highlighting Missing Features...\n");
    
    // LACKING FEATURE 1: Local function pointer variables.
    // The following line will cause a PARSE ERROR because the `let` statement parser
    // does not yet understand the function pointer `(...)` syntax.
    // let my_callback_fp(ret_v, code_i);
    printf("  - LACKING: Cannot declare local function pointers with 'let'.\n");

    // LACKING FEATURE 2: Typedefs.
    // In C, you would use typedef to make the `on_event` signature reusable.
    // Dust has no equivalent yet.
    // C example: typedef void (*event_handler_t)(struct Widget*);
    printf("  - LACKING: No 'typedef' for simplifying complex types.\n");
    
    // LACKING FEATURE 3: Enums.
    // This is a natural next step for state management.
    // enum ButtonState { IDLE, HOVER, PRESSED };
    printf("  - LACKING: No 'enum' for defining states or constants.\n");

    printf("\n--- Test Suite Complete ---\n");
    return 0;
}
```
```
--- Dust Advanced Test Suite Running ---

1. Initializing widget...
Widget created with ID 101 and Name 'Login Button'.

2. Assigning function pointers...
Assignments successful.

3. Calling functions via pointers...
  -> Event: on widget ID 101, Name 'Login Button'.
  -> Event: Widget updated with dt = 0.016000.
  -> Event: Widget destroyed.

Function pointer calls successful.

4. Highlighting Missing Features...
  - LACKING: Cannot declare local function pointers with 'let'.
  - LACKING: No 'typedef' for simplifying complex types.
  - LACKING: No 'enum' for defining states or constants.

--- Test Suite Complete ---


```

Will you please help me clean up all this Dust?

<3 
---


