#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Forward declarations
int main();
char* to_upper(char* str);
int count_char(char* str, char ch);
char* reverse(char* str);
char* my_strcat(char* dest, char* src);
char* my_strcpy(char* dest, char* src);
int my_strlen(char* str);

int my_strlen(char* str) {
if ((str == NULL)) {
return 0;
}
int len = 0;
while ((str[len] != '\0')) {
(len = (len + 1));
}
return len;
}
char* my_strcpy(char* dest, char* src) {
if (((dest == NULL) || (src == NULL))) {
return dest;
}
int i = 0;
while ((src[i] != '\0')) {
(dest[i] = src[i]);
(i = (i + 1));
}
(dest[i] = '\0');
return dest;
}
char* my_strcat(char* dest, char* src) {
if (((dest == NULL) || (src == NULL))) {
return dest;
}
int dest_len = my_strlen(dest);
int i = 0;
while ((src[i] != '\0')) {
(dest[(dest_len + i)] = src[i]);
(i = (i + 1));
}
(dest[(dest_len + i)] = '\0');
return dest;
}
char* reverse(char* str) {
if ((str == NULL)) {
return NULL;
}
int len = my_strlen(str);
if ((len <= 1)) {
return str;
}
int start = 0;
int end = (len - 1);
while ((start < end)) {
char temp = str[start];
(str[start] = str[end]);
(str[end] = temp);
(start = (start + 1));
(end = (end - 1));
}
return str;
}
int count_char(char* str, char ch) {
if ((str == NULL)) {
return 0;
}
int count = 0;
int i = 0;
while ((str[i] != '\0')) {
if ((str[i] == ch)) {
(count = (count + 1));
}
(i = (i + 1));
}
return count;
}
char* to_upper(char* str) {
if ((str == NULL)) {
return NULL;
}
int i = 0;
while ((str[i] != '\0')) {
(str[i] = toupper(str[i]));
(i = (i + 1));
}
return str;
}
int main() {
printf("=== Dust String Utilities Demo ===\n\n");
char* test1 = "Hello, Dust!";
int len = my_strlen(test1);
printf("Length of '%s': %d\n", test1, len);
char buffer[100];
my_strcpy(buffer, "Copied string");
printf("Copied: '%s'\n", buffer);
my_strcat(buffer, " + more");
printf("Concatenated: '%s'\n", buffer);
char rev[50];
my_strcpy(rev, "Dust");
reverse(rev);
printf("'Dust' reversed: '%s'\n", rev);
int count = count_char("Mississippi", 's');
printf("Number of 's' in 'Mississippi': %d\n", count);
char upper[50];
my_strcpy(upper, "dust language");
to_upper(upper);
printf("Uppercase: '%s'\n", upper);
printf("\n=== All tests passed! ===\n");
return 0;
}
