#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_ENTITIES 5
#define GOBLIN_HEALTH  20

typedef enum EntityType {
PLAYER = 0,
GOBLIN = 1,
TREASURE = 2
} EntityType;
typedef struct Entity Entity;
struct Entity {
int id;
int health;
EntityType type;
};
typedef Entity* EntityPtr;

// Forward declarations
int main();
void test_loops_and_arrays();
void print_entity(Entity entity);
Entity* create_entity(int id, int health, EntityType type);

Entity* create_entity(int id, int health, EntityType type) {
printf("--> Allocating new entity on the heap...\n");
Entity new_entity = malloc(sizeof(Entity));
new_entity->id = id;
new_entity->health = health;
new_entity->type = type;
return new_entity;
}
void print_entity(Entity entity) {
if ((entity == NULL)) {
printf("Entity (null)\n");
return;
}
printf("Entity ID: %d, Health: %d, Type: ", entity->id, entity->health);
if ((entity->type == PLAYER)) {
printf("PLAYER\n");
} else if ((entity->type == GOBLIN)) {
printf("GOBLIN\n");
} else {
printf("TREASURE\n");
}
}
void test_loops_and_arrays() {
printf("\n--- Testing Loops and Arrays ---\n");
int numbers[MAX_ENTITIES];
printf("Initializing with for loop...\n");
int i = 0;
for (i = 0; (i < MAX_ENTITIES); i = (i + 1)) {
numbers[i] = ((i + 1) * 11);
}
printf("Printing with while loop...\n");
i = 0;
while ((i < MAX_ENTITIES)) {
printf("  numbers[%d] = %d\n", i, numbers[i]);
i = (i + 1);
}
printf("Countdown with do-while loop...\n");
i = (MAX_ENTITIES - 1);
do {
printf("  ... %d\n", numbers[i]);
i = (i - 1);
} while ((i >= 0));
}
int main() {
printf("--- Dust Comprehensive Test Suite ---\n\n");
printf("--- Testing Stack-based Struct ---\n");
Entity player;
player.id = 1;
player.health = 100;
player.type = PLAYER;
print_entity(&player);
printf("\n--- Testing Heap-based Struct ---\n");
Entity goblin = create_entity(2, GOBLIN_HEALTH, GOBLIN);
print_entity(goblin);
test_loops_and_arrays();
printf("\n--- Testing FFI (rand) ---\n");
srand(time(NULL));
int random_damage = ((rand() % 10) + 1);
printf("Dealt %d damage to the goblin!\n", random_damage);
goblin->health = (goblin->health - random_damage);
print_entity(goblin);
printf("\n--> Freeing heap-allocated entity.\n");
free(goblin);
printf("\n--- Test Suite Complete ---\n");
return 0;
}
