#include <stdio.h>

void print_day_type(int day_number) {
printf("Day %d is a ", day_number);
switch (day_number) {
case 1:
case 2:
case 3:
case 4:
case 5:
printf("Weekday.\n");
break;
case 6:
case 7:
printf("Weekend day! Enjoy!\n");
break;
default:
printf("day of unknown type (invalid number).\n");
break;
}

}
int main() {
printf("--- Switch Statement Test ---\n\n");
for (int day = 4; (day <= 8); (day = (day + 1))) {
print_day_type(day);
}
printf("\n--- Test Complete ---\n");
return 0;
}
