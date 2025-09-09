#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

puts()
typedef void* isr_t_isr;

typedef enum DeviceStatus {
    STATUS_OFFLINE = 0,
    STATUS_ONLINE = 1,
    STATUS_ERROR = 5
} DeviceStatus;
typedef struct PCIDevice PCIDevice;
struct PCIDevice {
    uint32_t status;
    uint16_t vendor_id;
    uint8_t mmio_base;
    void interrupt_line_irq;
    void handler_isr;
};
int i;
// Forward declarations
int main();
void my_interrupt_handler();
int calculate_offset(int base, int offset, int multiplier, int divisor);

int calculate_offset(int base, int offset, int multiplier, int divisor) {
int result = (((base + offset) * multiplier) / divisor);
return result;
}
void my_interrupt_handler() {
puts("Interrupt handler called!");
}
int main() {
printf("--- Dust Advanced Systems Test ---\n");
PCIDevice* my_device = (PCIDevice*)malloc(sizeof(PCIDevice));
printf("1. Testing Struct Initialization...\n");
(my_device->status = STATUS_ONLINE);
(my_device->vendor_id = 0x8086);
(my_device->mmio_base = 0xF0000000);
(my_device->interrupt_line = 9);
(my_device->handler = &my_interrupt_handler);
printf(" Device MMIO base: 0x%lx\n", my_device->mmio_base);
printf("   Device Status: %d\n", my_device->status);
printf("\n2. Testing Operator Precedence...\n");
int calculated_offset = calculate_offset(10, 20, 4, 2);
printf("   (10 + 20) * 4 / 2 = %d (should be 60)\n", calculated_offset);
printf("\n3. Testing Function Pointers...\n");
if ((my_device->handler != NULL)) {
printf("   Calling device ISR via function pointer...\n   ");
my_device->handler();
}
free(my_device);
printf("\n--- Test Complete ---\n");
return 0;
}
