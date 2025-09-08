#include <stdio.h>
#define DATA_SIZE 10

// Forward declarations
int main();

int main() {
printf("--- Advanced Features Test ---\n");
printf("\n1. Testing Array Initializer...\n");
int data[DATA_SIZE] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
printf("\n2. Testing break and continue...\n");
printf("Looping through the data. Will skip printing 40 and break at 80.\n");
for (int i = 0; (i < DATA_SIZE); (i = (i + 1))) {
int value = data[i];
if ((value == 40)) {
printf("   (Found 40, continuing...)\n");
continue;
}
if ((value == 80)) {
printf("   (Found 80, breaking...)\n");
break;
}
printf("   Processing value: %d\n", value);
}
printf("\n--- Test Complete ---\n");
return 0;
}
