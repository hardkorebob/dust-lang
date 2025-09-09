#include <stdio.h>

typedef union Variant Variant;
union Variant {
    int i_value;
    float f_value;
    char* s_value;
    void* ptr;
};
// Forward declarations
int main();

int main() {
Variant v;
(v.i_value = 42);
printf("Int: %d\n", v.i_value);
(v.f_value = 3.14);
printf("Float: %f\n", v.f_value);
return 0;
}
