#include <stdio.h>
#include <string.h>

int main() {
    char user_input[8];
    char password[8] = PASSWORD;

    printf("Введите пароль: ");
    scanf("%s", user_input);

    if (strcmp(user_input, password) == 0) {
        printf("SECRET: ну_тип_что_то_ценное!\n");
    } else {
        printf("Permission denied!\n");
    }
    return 0;
}

