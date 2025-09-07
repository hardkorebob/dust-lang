#include <stdio.h>
#define DATA_SIZE 15

typedef struct AnalysisResult AnalysisResult;
struct AnalysisResult {
    int sum;
    float average;
};
int calculate_sum(int* data, int size) {
int total = 0;
for (int i = 0; (i < size); (i = (i + 1))) {
(total = (total + data[i]));
}
return total;
}
int main() {
printf("--- For Loop Test Program ---\n");
int data[DATA_SIZE];
printf("1. Initializing data array...\n");
for (int i = 0; (i < DATA_SIZE); (i = (i + 1))) {
(data[i] = (i * 3));
}
printf("2. Array contents:\n");
for (int i = 0; (i < DATA_SIZE); (i = (i + 1))) {
printf("   data[%d] = %d\n", i, data[i]);
}
printf("3. Analyzing data...\n");
AnalysisResult results;
(results.sum_i = calculate_sum(data, DATA_SIZE));
(results.average_f = (results.sum_i / DATA_SIZE));
printf("   Sum of all elements is: %d\n", results.sum_i);
printf("   Average of all elements is: %f\n", results.average_f);
printf("--- Test Complete ---\n");
return 0;
}
