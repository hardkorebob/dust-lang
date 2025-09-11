#include <stdio.h>
#include <string.h> // Needed for strcpy

typedef struct Widget Widget;
struct Widget {
int id;
char name[32];
    void (*on_destroy)();
    int (*on_update)(float);
    void (*on_event)(Widget*);
};
// Forward declarations
int main();
void widget_event(Widget* w);
int widget_update(float dt);
void widget_destroy();

void widget_destroy() {
printf("  -> Event: Widget destroyed.\n");
}
int widget_update(float dt) {
printf("  -> Event: Widget updated with dt = %f.\n", dt);
return 1;
}
void widget_event(Widget* w) {
printf("  -> Event: on widget ID %d, Name '%s'.\n", w->id, w->name);
}
int main() {
printf("--- Dust Advanced Test Suite Running ---\n\n");
printf("1. Initializing widget...\n");
Widget ui_button;
ui_button.id = 101;
strcpy(ui_button.name, "Login Button");
printf("Widget created with ID %d and Name '%s'.\n\n", ui_button.id, ui_button.name);
printf("2. Assigning function pointers...\n");
ui_button.on_destroy = &widget_destroy;
ui_button.on_update = &widget_update;
ui_button.on_event = &widget_event;
printf("Assignments successful.\n\n");
printf("3. Calling functions via pointers...\n");
ui_button.on_event(&ui_button);
ui_button.on_update(0.016);
ui_button.on_destroy();
printf("\nFunction pointer calls successful.\n\n");
printf("4. Highlighting Missing Features...\n");
printf("  - LACKING: Cannot declare local function pointers with 'let'.\n");
printf("  - LACKING: No 'typedef' for simplifying complex types.\n");
printf("  - LACKING: No 'enum' for defining states or constants.\n");
printf("\n--- Test Suite Complete ---\n");
return 0;
}
