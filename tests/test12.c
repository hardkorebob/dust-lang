#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Greeter Greeter;
struct Greeter {
    char* message;
};
// Forward declarations
int main();
void greeter_destroy(Greeter* greeter);
void greeter_say_hello(Greeter* greeter);
Greeter* greeter_create(char* message);

Greeter* greeter_create(char* message) {
Greeter* new_greeter = (Greeter*)malloc(sizeof(Greeter));
void message_len_st = strlen(message);
new_greeter->message = (char*)malloc((message_len_st + 1));
strcpy(new_greeter->message, message);
return new_greeter;
}
void greeter_say_hello(Greeter* greeter) {
printf("%s\n", greeter->message);
}
void greeter_destroy(Greeter* greeter) {
free(greeter->message);
free(greeter);
}
int main() {
Greeter* world_greeter = greeter_create("Hello from Dust!");
greeter_say_hello(world_greeter);
greeter_destroy(world_greeter);
printf("Dust program executed successfully.\n");
return 0;
}
