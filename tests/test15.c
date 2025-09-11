#include <stdio.h>
#include <stdint.h>

typedef uint32_t Score_t;

typedef union Packet Packet;
union Packet {
uint32_t id;
int value;
char code[4];
};
// Forward declarations
int main();
int process_data(int* data, int count);

int process_data(int* data, int count) {
int total = 0;
for (int i = 0; (i < count); i++) {
total += *data;
data++;
}
return total;
}
int main() {
printf("--- Dust Language Test 2 ---\n");
int numbers[5] = { 10, 20, 30, 40, 50 };
int sum = process_data(&numbers[0], 5);
printf("Sum of array: %d\n", sum);
Packet data_packet;
data_packet.id = 12345;
printf("Packet ID: %u\n", data_packet.id);
data_packet.value = -100;
printf("Packet Value: %d\n", data_packet.value);
int status_code = 1;
switch (status_code) {
case 0:
printf("Status: OK\n");
break;
case 1:
printf("Status: Warning\n");
break;
case 2:
printf("Status: Error\n");
break;
default:
printf("Status: Unknown\n");
}

printf("Test complete!\n");
return 0;
}
