#include <stdio.h>
#include <stdbool.h>

typedef struct Point Point;
struct Point {
int x;
int y;
};
// Forward declarations
int main();
void test_memory_features();
void test_operators();
void test_control_flow();
void test_fallthrough(int start_day);
void print_point(const Point* p);

void print_point(const Point* p) {
printf("Point(x: %d, y: %d)\n", p->x, p->y);
}
void test_fallthrough(int start_day) {
printf("\n--- Testing Switch Fallthrough (starting from day %d) ---\n", start_day);
switch (start_day) {
case 1:
printf("It's Monday.\n");
case 2:
printf("It's a weekday.\n");
case 3:
printf("Still a weekday.\n");
break;
case 4:
printf("Thursday.\n");
case 5:
printf("End of the work week!\n");
case 6:
case 7:
printf("It's the weekend!\n");
break;
default:
printf("Invalid day provided.\n");
}

}
void test_control_flow() {
printf("\n--- Testing Control Flow ---\n");
int temperature = 25;
if ((temperature > 30)) {
printf("It's hot.\n");
} else if ((temperature > 20)) {
printf("It's warm.\n");
} else {
printf("It's cool.\n");
}
printf("For loop: ");
for (int i = 0; (i < 5); i = (i + 1)) {
if ((i == 3)) {
continue;
}
printf("%d ", i);
}
printf("\n");
printf("While loop: ");
int countdown = 3;
while ((countdown > 0)) {
printf("%d... ", countdown);
countdown = (countdown - 1);
}
printf("Lift off!\n");
printf("Do-While loop: ");
int num = 5;
do {
printf("%d ", num);
num = (num - 1);
} while ((num > 5));
printf("\n");
}
void test_operators() {
printf("\n--- Testing Operators ---\n");
int a = 10;
int b = 4;
printf("a = %d, b = %d\n", a, b);
printf("a + b = %d\n", (a + b));
printf("a - b = %d\n", (a - b));
printf("a * b = %d\n", (a * b));
printf("a / b = %d\n", (a / b));
printf("a %% b = %d\n", (a % b));
bool is_equal = (a == 10);
bool is_not_equal = (b != 4);
if ((is_equal && !is_not_equal)) {
printf("Logical operators work!\n");
}
}
void test_memory_features() {
printf("\n--- Testing Memory Features ---\n");
int numbers[5] = { 10, 20, 30, 40, 50 };
printf("First number: %d\n", numbers[0]);
printf("Third number: %d\n", numbers[2]);
numbers[2] = 35;
printf("Modified third number: %d\n", numbers[2]);
printf("Size of int: %zu bytes\n", sizeof(int));
printf("Size of Point struct: %zu bytes\n", sizeof(Point));
printf("Size of numbers array: %zu bytes\n", sizeof(numbers));
Point my_point = { 100, 200 };
Point* point_ptr = &my_point;
printf("Access via pointer: %d\n", point_ptr->x);
print_point(point_ptr);
}
int main() {
printf("--- Comprehensive Language Test Suite ---\n");
test_fallthrough(1);
test_fallthrough(5);
test_control_flow();
test_operators();
test_memory_features();
printf("\n--- Test Suite Complete ---\n");
return 0;
}
