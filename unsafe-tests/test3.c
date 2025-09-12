#include <stdio.h>

// Forward declarations
int main();
int find_max(int* arr, int size);
void print_array(int* arr, int size);

int ARRAYSIZE = 10;
void print_array(int* arr, int size) {
printf("Array Contents:\n");
int i = 0;
if ((size > 0)) {
do {
printf("  Element %d: %d\n", i, arr[i]);
i = (i + 1);
} while ((i < size));
} else {
printf("  (Array is empty)\n");
}
printf("--------------------\n");
}
int find_max(int* arr, int size) {
if ((size <= 0)) {
return -1;
}
int max_val = arr[0];
int i = 1;
while ((i < size)) {
if ((arr[i] > max_val)) {
max_val = arr[i];
}
i = (i + 1);
}
return max_val;
}
int main() {
printf("--- Array Test Program ---\n");
int numbers[ARRAYSIZE];
printf("Initializing array...\n");
int i = 0;
while ((i < ARRAYSIZE)) {
numbers[i] = ((i + 1) * 7);
i = (i + 1);
}
print_array(numbers, ARRAYSIZE);
int max_number = find_max(numbers, ARRAYSIZE);
printf("Maximum value in the array is: %d\n", max_number);
printf("--- Test Complete ---\n");
return 0;
}
