#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct Player Player;
struct Player {
    int x;
    int y;
    uint32_t health;
    char* name;
    int inventory[10];
    void active_bl;
};
typedef enum GameState {
    MENU = 0,
    PLAYING = 1,
    PAUSED = 2,
    GAME_OVER = 3
} GameState;
typedef uint32_t Score;

// Forward declarations
int main();
void test_arrays();
void test_control_flow();
void test_operators();
void update_player(Player* p);
int sum_array(int* arr, size_t size);
int add(int a, int b);

int add(int a, int b) {
return (a + b);
}
int sum_array(int* arr, size_t size) {
int total = 0;
for (size_t i = 0; (i < size); i++) {
total += arr[i];
}
return total;
}
void update_player(Player* p) {
p->x++;
p->y++;
p->health -= 10;
}
void test_operators() {
int a = 10;
int b = 3;
int sum = (a + b);
int diff = (a - b);
int prod = (a * b);
int quot = (a / b);
int rem = (a % b);
int andd = (a & b);
int orr = (a | b);
int xor = (a ^ b);
int lshift = (a << 2);
int rshift = (a >> 1);
a += 5;
a -= 2;
a *= 2;
a /= 3;
(a &= 0xFF);
(a |= 0x10);
(a ^= 0x0F);
(a <<= 1);
(a >>= 1);
int pre = ++a;
int post = b++;
--a;
b--;
void eq_bl = (a == b);
void neq_bl = (a != b);
void lt_bl = (a < b);
void gt_bl = (a > b);
void lte_bl = (a <= b);
void gte_bl = (a >= b);
void and_bl = ((a > 5) && (b < 10));
void or_bl = ((a > 5) || (b < 10));
void not_bl = !(a == b);
int max = ((a > b) ? a : b);
printf("Operators test complete\n");
}
void test_control_flow() {
GameState state = PLAYING;
switch (state) {
case MENU:
printf("In menu\n");
break;
case PLAYING:
printf("Playing game\n");
break;
case PAUSED:
printf("Game paused\n");
break;
case GAME_OVER:
printf("Game over\n");
break;
default:
printf("Unknown state\n");
}

int count = 0;
while ((count < 5)) {
count++;
}
do {
count--;
} while ((count > 0));
for (int i = 0; (i < 10); i++) {
if ((i == 5)) {
continue;
}
if ((i == 8)) {
break;
}
}
}
void test_arrays() {
int numbers[5] = { 1, 2, 3, 4, 5 };
uint8_t buffer[256];
char message[] = "Hello, Dust!";
numbers[0] = 10;
buffer[0] = 0xFF;
int* ptr = &numbers[0];
int val = *ptr;
ptr++;
char* name = "Dust Language";
char first = name[0];
}
int main() {
printf("=== Dust Compiler Test Suite ===\n\n");
int result = add(5, 3);
printf("add(5, 3) = %d\n", result);
Player player;
player.x = 100;
player.y = 200;
player.health = 100;
player.name = "Hero";
player.active = 1;
printf("Player at (%d, %d) with %d health\n", player.x, player.y, player.health);
update_player(&player);
printf("After update: (%d, %d) with %d health\n", player.x, player.y, player.health);
int nums[3] = { 10, 20, 30 };
int sum = sum_array(nums, 3);
printf("Array sum: %d\n", sum);
test_operators();
test_control_flow();
}
