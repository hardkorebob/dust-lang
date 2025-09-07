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

- **Identifiers are Nouns**: Variables are the "things" in our programs (`playera`, `messagee`).
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
//test7.dust
#include <stdio.h>
#include <stdbool.h>

struct Point {
    x_i;
    y_i;
};


func print_point_v(p_Pointb) {
    printf("Point(x: %d, y: %d)\n", p_Point->x, p_Point->y);
}


func test_fallthrough_v(start_day_i) {
    printf("\n--- Testing Switch Fallthrough (starting from day %d) ---\n", start_day_i);
    switch (start_day_i) {
        case 1:
            printf("It's Monday. ");
        case 2:
            printf("It's a weekday. ");
        case 3:
            printf("Still a weekday. ");
            break;
        case 4:
            printf("Thursday. ");
        case 5:
            printf("End of the work week! ");
        case 6:
        case 7:
            printf("It's the weekend!\n");
            break;
        default:
            printf("Invalid day provided.\n");
    }
}


func test_control_flow_v() {
    printf("\n--- Testing Control Flow ---\n");
    
    let temperature_i = 25;
    if (temperature_i > 30) {
        printf("It's hot.\n");
    } else if (temperature_i > 20) {
        printf("It's warm.\n");
    } else {
        printf("It's cool.\n");
    }

    printf("For loop: ");
    for (let i_i = 0; i_i < 5; i_i = i_i + 1) {
        if (i_i == 3) {
            continue;
        }
        printf("%d ", i_i);
    }
    printf("\n");

    printf("While loop: ");
    let countdown_i = 3;
    while (countdown_i > 0) {
        printf("%d... ", countdown_i);
        countdown_i = countdown_i - 1;
    }
    printf("Lift off!\n");
    
    printf("Do-While loop: ");
    let num_i = 5;
    do {
        printf("%d ", num_i);
        num_i = num_i - 1;
    } while (num_i > 5);
    printf("\n");
}

func test_operators_v() {
    printf("\n--- Testing Operators ---\n");
    let a_i = 10;
    let b_i = 4;
    printf("a = %d, b = %d\n", a_i, b_i);
    printf("a + b = %d\n", a_i + b_i);
    printf("a - b = %d\n", a_i - b_i);
    printf("a * b = %d\n", a_i * b_i);
    printf("a / b = %d\n", a_i / b_i);
    printf("a %% b = %d\n", a_i % b_i);
    
    let is_equal_bl = (a_i == 10);
    let is_not_equal_bl = (b_i != 4);
    if (is_equal_bl && !is_not_equal_bl) {
        printf("Logical operators work!\n");
    }
}


func test_memory_features_v() {
    printf("\n--- Testing Memory Features ---\n");

    let numbers_ia[5] = {10, 20, 30, 40, 50};
    printf("First number: %d\n", numbers_ia[0]);
    printf("Third number: %d\n", numbers_ia[2]);
    numbers_ia[2] = 35;
    printf("Modified third number: %d\n", numbers_ia[2]);

    printf("Size of int: %zu bytes\n", sizeof(let_i));
    printf("Size of Point struct: %zu bytes\n", sizeof(Point));
    printf("Size of numbers array: %zu bytes\n", sizeof(numbers_ia));
    
    let my_point_Point = {100, 200};

    let point_ptr_Point = &my_point_Point;

    printf("Access via pointer: %d\n", point_ptr_Point->x);
    
    print_point_v(point_ptr_Point);
}


func main_i() {
    printf("--- Comprehensive Language Test Suite ---\n");

    test_fallthrough_v(1);
    test_fallthrough_v(5);
    test_control_flow_v();
    test_operators_v();
    test_memory_features_v();

    printf("\n--- Test Suite Complete ---\n");
    return 0;
}
```
```
dustc test7.dust ; gcc test7.c
Successfully compiled 'test7.dust' to 'test7.c'
test7.c: In function ‘test_memory_features_v’:
test7.c:86:21: warning: initialization of ‘int *’ from ‘int’ makes pointer from integer without a cast [-Wint-conversion]
   86 | int* numbers[5] = { 10, 20, 30, 40, 50 };
      |                     ^~
test7.c:86:21: note: (near initialization for ‘numbers[0]’)
test7.c:86:25: warning: initialization of ‘int *’ from ‘int’ makes pointer from integer without a cast [-Wint-conversion]
   86 | int* numbers[5] = { 10, 20, 30, 40, 50 };
      |                         ^~
test7.c:86:25: note: (near initialization for ‘numbers[1]’)
test7.c:86:29: warning: initialization of ‘int *’ from ‘int’ makes pointer from integer without a cast [-Wint-conversion]
   86 | int* numbers[5] = { 10, 20, 30, 40, 50 };
      |                             ^~
test7.c:86:29: note: (near initialization for ‘numbers[2]’)
test7.c:86:33: warning: initialization of ‘int *’ from ‘int’ makes pointer from integer without a cast [-Wint-conversion]
   86 | int* numbers[5] = { 10, 20, 30, 40, 50 };
      |                                 ^~
test7.c:86:33: note: (near initialization for ‘numbers[3]’)
test7.c:86:37: warning: initialization of ‘int *’ from ‘int’ makes pointer from integer without a cast [-Wint-conversion]
   86 | int* numbers[5] = { 10, 20, 30, 40, 50 };
      |                                     ^~
test7.c:86:37: note: (near initialization for ‘numbers[4]’)
test7.c:87:24: warning: format ‘%d’ expects argument of type ‘int’, but argument 2 has type ‘int *’ [-Wformat=]
   87 | printf("First number: %d\n", numbers[0]);
      |                       ~^     ~~~~~~~~~~
      |                        |            |
      |                        int          int *
      |                       %ls
test7.c:88:24: warning: format ‘%d’ expects argument of type ‘int’, but argument 2 has type ‘int *’ [-Wformat=]
   88 | printf("Third number: %d\n", numbers[2]);
      |                       ~^     ~~~~~~~~~~
      |                        |            |
      |                        int          int *
      |                       %ls
test7.c:89:13: warning: assignment to ‘int *’ from ‘int’ makes pointer from integer without a cast [-Wint-conversion]
   89 | (numbers[2] = 35);
      |             ^
test7.c:90:33: warning: format ‘%d’ expects argument of type ‘int’, but argument 2 has type ‘int *’ [-Wformat=]
   90 | printf("Modified third number: %d\n", numbers[2]);
      |                                ~^     ~~~~~~~~~~
      |                                 |            |
      |                                 int          int *
      |                                %ls
test7.c:95:19: error: invalid initializer
   95 | Point point_ptr = &(my_point);
      |                   ^
test7.c:96:45: error: invalid type argument of ‘->’ (have ‘Point’)
   96 | printf("Access via pointer: %d\n", point_ptr->x);
      |                                             ^~
test7.c:97:15: error: incompatible type for argument 1 of ‘print_point_v’
   97 | print_point_v(point_ptr);
      |               ^~~~~~~~~
      |               |
      |               Point
test7.c:9:33: note: expected ‘const Point *’ but argument is of type ‘Point’
    9 | void print_point_v(const Point* p) {

```

I need to clean up this dust to make it better! Help. me?

---


