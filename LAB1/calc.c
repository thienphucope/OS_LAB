#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "calc_logic.h"

int main() {
    char expression[50];
    float ANS = 0;
    float a, b;
    
    while (1) {
        printf(">> ");
        fgets(expression, sizeof(expression), stdin);
        
        // Remove newline character from input
        expression[strcspn(expression, "\n")] = '\0';

        if (strcmp(expression, "EXIT") == 0) {
            break;
        }

        char splited[3][20];
        sscanf(expression, "%s %s %s", splited[0], splited[1], splited[2]);

        if ((strcmp(splited[1], "+") != 0 && strcmp(splited[1], "-") != 0 &&
             strcmp(splited[1], "*") != 0 && strcmp(splited[1], "/") != 0)) {
            printf("SYNTAX ERROR\n");
            continue;
        }

        // Handling ANS for previous result
        if (strcmp(splited[0], "ANS") == 0) {
            a = ANS;
        } else {
            a = atof(splited[0]);
        }

        if (strcmp(splited[2], "ANS") == 0) {
            b = ANS;
        } else {
            b = atof(splited[2]);
        }

        // Perform the calculation based on the operator
        if (strcmp(splited[1], "+") == 0) {
            ANS = add(a, b);
        } else if (strcmp(splited[1], "-") == 0) {
            ANS = subtract(a, b);
        } else if (strcmp(splited[1], "*") == 0) {
            ANS = multiply(a, b);
        } else if (strcmp(splited[1], "/") == 0) {
            if (b == 0) {
                printf("MATH ERROR\n");
                continue;
            } else {
                ANS = divide(a, b);
            }
        }

        if (ANS == (int)ANS) {
            printf("%d\n", (int)ANS);
        } else {
            printf("%.2f\n", ANS);
        }
    }

    return 0;
}
