#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct Config Config;
struct Config {
    int port;
    char* host;
    float timeout;
};
typedef struct User User;
struct User {
    int id;
    char* name;
    int age;
    float score;
    char grade;
};
typedef struct Database Database;
struct Database {
    char* connection;
    int max_connections;
    int is_active;
};
void allocate_memory_p(int size) {
void memory_p = malloc(size);
return memory_p;
}
void create_user_p(int id, char* name, int age) {
void user_p = malloc(sizeof(User));
return user_p;
}
int calculate_sum(int a, int b) {
return (a + b);
}
float calculate_average(int sum, int count) {
return (sum / count);
}
void print_user_info(const void* user) {
printf("User info accessed via borrowed pointer\n");
}
void validate_config(void config_r) {
printf("Config validated via reference\n");
}
int main() {
printf("=== Dust Compiler Test Suite ===\n");
printf("Testing suffix-based type system\n\n");
printf("Test 1: Basic Types\n");
int count = 42;
int status = 1;
int error_code = 0;
float pi = 3.14159;
float temperature = 98.6;
float version = 2.5;
char grade = 'A';
char initial = 'D';
char symbol = '*';
char* message = "Dust: Built for the future, built to outlast";
char* author = "Ancient wisdom in modern code";
char* motto = "Simple ECS in disguise";
printf("Integer: %d\n", count);
printf("Float: %f\n", pi);
printf("Char: %c\n", grade);
printf("String: %s\n", message);
printf("\nTest 2: Pointer Types\n");
int* number = &(count);
const int* number_view = number;
const int* number_ref = number;
void temp_fp = &(temperature);
void temp_view_fb = temp_fp;
printf("Value via owned pointer: %d\n", *(number));
printf("Value via borrowed pointer: %d\n", *(number_view));
printf("\nTest 3: User-Defined Types\n");
void config_p = malloc(sizeof(Config));
const void* config_view = config_p;
void config_ref_r = config_p;
void admin_p = create_user_p(1, "Admin", 30);
void user_p = create_user_p(2, "Alice", 25);
const void* admin_view = admin_p;
const void* user_view = user_p;
print_user_info(user_view);
validate_config(config_ref_r);
printf("\nTest 4: Expressions\n");
int x = 10;
int y = 20;
int sum = (x + y);
int diff = (y - x);
int prod = (x * 3);
int quot = (y / 2);
printf("Sum: %d + %d = %d\n", x, y, sum);
printf("Product: %d * 3 = %d\n", x, prod);
int result = calculate_sum(x, y);
float avg = calculate_average(sum, 2);
printf("Function result: %d\n", result);
printf("\nTest 5: Control Flow\n");
int is_running = 1;
int debug_mode = 0;
if ((is_running == 1)) {
printf("System is running\n");
if ((debug_mode == 0)) {
printf("Debug mode is OFF\n");
} else {
printf("Debug mode is ON\n");
}
}
if ((x < y)) {
printf("%d is less than %d\n", x, y);
}
if ((sum == 30)) {
printf("Sum equals 30\n");
}
if ((grade != 'F')) {
printf("Grade is passing\n");
}
printf("\nTest 6: Complex Expressions\n");
int complex = (((x + y) * 2) - 10);
int condition = ((x > 5) && (y < 100));
if (((is_running == 1) && (status == 1))) {
printf("Both conditions met\n");
}
int a = 5;
int b = 10;
int c = 15;
int nested_sum = calculate_sum(calculate_sum(a, b), c);
printf("Nested sum: %d\n", nested_sum);
printf("\nTest 7: Memory Management\n");
void buffer_p = allocate_memory_p(256);
void data_p = malloc((sizeof(User) * 10));
printf("Memory allocated successfully\n");
int config_size = sizeof(Config);
int user_size = sizeof(User);
int int_size = sizeof(int);
printf("Config size: %d bytes\n", config_size);
printf("User size: %d bytes\n", user_size);
printf("\nTest 8: String Operations\n");
char* greeting = "Hello, Dust!";
char* language = "Dust";
char* description = "A language built for human-mind programming";
printf("Greeting: %s\n", greeting);
printf("Language: %s\n", language);
printf("Description: %s\n", description);
printf("\nTest 9: Edge Cases\n");
int user_count = 100;
int max_retry = 3;
int is_valid = 1;
void ptr_to_ptr_ix = &(number);
if (admin_p) {
printf("Admin user exists\n");
}
printf("\nTest 10: Cleanup\n");
free(config_p);
free(admin_p);
free(user_p);
free(buffer_p);
free(data_p);
printf("\n=== All Tests Complete ===\n");
printf("Dust compiler test successful!\n");
printf("Built with Dust - Simple ECS in disguise\n");
return 0;
}
int callback_handler(int value) {
return (value * 2);
}
int factorial(int n) {
if ((n <= 1)) {
return 1;
}
return (n * factorial((n - 1)));
}
void initialize_database_p(char* connection, int max) {
void db_p = malloc(sizeof(Database));
return db_p;
}
float safe_divide(float numerator, float denominator) {
if ((denominator == 0)) {
printf("Error: Division by zero\n");
return 0.0;
}
return (numerator / denominator);
}
