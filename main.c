#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int errorcode = 0;
char errormsg[64];

struct argdb {
    bool silent;
};

struct argdb args;

float add(float x, float y) { //Addition function
    return x+y;
}
float sub(float x, float y) { //Subtraction function
    return x-y;
}
float mul(float x, float y) { //Multiplication function
    return x*y;
}
float dvd(float x, float y) { //Division function
    if (y == 0) {
        errorcode = 2;
        strcpy(errormsg, "DIVIDE BY ZERO |[2]| DIVIDE BY ZERO");
        return 0;
    } else {
        return x/y;
    }
}
float pex(float x, float y) { //Exponent function
    float res = x;
    for (size_t i = 1; i < y; i++) {
        res = mul(res, x);
    }
    return res;
}

float eval(float x, float y, char op) { //evaluates (x op y) where op is a mathematical operator
    switch (op) {
        case '+':
            return add(x, y);
            break;
        case '-':
            return sub(x, y);
            break;
        case '*':
            return mul(x, y);
            break;
        case '/':
            return dvd(x, y);
            break;
        case '^':
            return pex(x, y);
            break;
        default:
            printf("Unexpected operator: '%c'\n", op);
            return 0;
    }
}

int otoi(char* op) {
    char opp = op[0];
    switch (opp) {
        case '+':
            return 0;
            break;
        case '-':
            return 1;
            break;
        case '*':
            return 2;
            break;
        case '/':
            return 3;
            break;
        case '^':
            return 4;
            break;
        default:
            //printf("Unexpected operator: '%c'\n", op);
            return -1;
        }
}

int parse(char* string) {
    /* What follows is a terrible implementation of the Shunting-yard algorithm */
    int precedence[] = {2, 2, 3, 3, 4}; //Operator precedence in order of function declaration above
    char operators[] = {'+', '-', '*', '/', '^'};
    char associativity[] = {'L', 'L', 'L', 'L', 'R'};
    char exclude[] = {'(', ')'};

    //printf("Creating IB\n");
    char inputb[strlen(string)]; //Create an input buffer

    //printf("Copying...\n");
    strcpy(inputb, string);

    //printf("Searching for operators...\n");
    int ops = 0;
    for (size_t i = 0; i < (sizeof(inputb) / sizeof(char)); i++) {
        for (size_t v = 0; v < (sizeof(operators) / sizeof(char)); v++) {
            if (inputb[i] == operators[v]) {
                ops++;
            }
        }
    }

    //printf("Creating INFIX\n");
    char infix[ops+1][strlen(string)+ops];
    //char buffer[strlen(string)+ops];
    int inp = 0;
    int inpp = 0;


    //printf("Populating INFIX...\n");
    for (size_t i = 0; i < (sizeof(inputb) / sizeof(char)); i++) {
        int isOperator = 0;
        char op = '$';
        for (size_t v = 0; v < (sizeof(operators) / sizeof(char)); v++) {
            if (inputb[i] == operators[v]) {
                isOperator = 1;
                op = operators[v];
            }
        }
        for (size_t v = 0; v < (sizeof(exclude) / sizeof(char)); v++) {
            if (inputb[i] == exclude[v]) {
                isOperator = 1;
                op = exclude[v];
            }
        }
        if (isOperator == 1) {
            //printf("Dumping buffer into INFIX\n");

            infix[inp][inpp] = '\0';
            inp++;
            infix[inp][0] = op;
            infix[inp][1] = '\0';
            inp++;
            inpp = 0;
        } else {
            //printf("Adding '%c' to the buffer\n", inputb[i]);
            infix[inp][inpp] = inputb[i];
            inpp++;

        }
    }
    //printf("Dumping buffer into INFIX\n");
    infix[inp][inpp] = '\0';
    inp++;
    inpp = 0;

    //printf("String: %s\nLength: %i\nSize of IB: %i\nElements: %i\nOperators: %i\nSize of IF: %i\nElements: %i\n", string, strlen(string), sizeof(inputb), (sizeof(inputb) / sizeof(char)), ops, sizeof(infix), inp);

    //for (size_t i = 0; i < inp; i++) {
    //    printf("[%s]", infix[i]);
    //}
    //printf("\n");

    /* Now for the actual implementation of the Shunting-yard algorithm */

    char stack[inp + 8][512];
    int top = 0;

    //printf("Creating POSTFIX\n");
    char tpostfix[inp+1][strlen(string)+ops];
    int pp = 0;

    for (size_t i = 0; i < inp; i++) {
        strcpy(tpostfix[i], "        ");
        tpostfix[i][0] = '\0';

        char* token = infix[i];
        //postfix[i][0] = '\0';

        int isOperator = 0;
        char op = '$';
        for (size_t v = 0; v < (sizeof(operators) / sizeof(char)); v++) {
            if (infix[i][0] == operators[v]) {
                isOperator = 1;
                op = operators[v];
            }
        }

        if (((isOperator == 0) && (token[0] != '(') && (token[0] != ')') && (token[0] != '\0'))) {
            strcpy(tpostfix[pp], token);
            //printf("NUM[%i]: %s\n", i, tpostfix[pp]);
            pp++;
        } else if (isOperator == 1) {
            char opc[] = {op, '\0'};
            while (((otoi(stack[top]) != -1) && (associativity[otoi(opc)] == 'L' && (precedence[otoi(opc)] <= precedence[otoi(stack[top])]))) || (associativity[otoi(opc)] == 'R' && (precedence[otoi(opc)] < precedence[otoi(stack[top])]))) {
                strcpy(tpostfix[pp], stack[top]);
                //printf("OPR[%i]: %s\n", i, tpostfix[pp]);
                pp++;
                top--;
            }
            top++;
            strcpy(stack[top], infix[i]);
            //printf("STK[%i]: %s\n", top, stack[top]);

        } else if (strcmp(token, "(") == 0) {
            top++;
            strcpy(stack[top], token);
        } else if (strcmp(token, ")") == 0) {
            while (strcmp(token, "(") != 0) {
                strcpy(tpostfix[pp], stack[top]);
                pp++;
                top--;
            }
            top--;
        } else {
            //printf("NULL REACHED\n");
            tpostfix[pp][0] = '\0';
            pp++;
        }

    }
    while (top > 0) {
        strcpy(tpostfix[pp], stack[top]);
        //printf("END[%i]: %s\n", pp, stack[top]);

        pp++;
        top--;
    }

    //printf("RPN FINAL: ");
    //for (size_t i = 0; i < pp; i++) {
    //    printf("[%s]", tpostfix[i]);
    //}
    //printf("\n");

    char postfix[inp][strlen(string)+ops];
    int rp = 0;

    for (size_t i = 0; i < pp; i++) {
        if (tpostfix[i][0] != '\0') {
            //printf("TPX[%i]: %s", i, tpostfix[i]);
            //printf("\n");
            strcpy(postfix[rp], tpostfix[i]);
            rp++;
        }
    }

    //printf("POSTFIX: ");
    //for (size_t i = 0; i < rp; i++) {
    //    printf("[%s]", postfix[i]);
    //}
    //printf("\n");


    /* Now to actually evaluate */

    top = -1;
    float bufa, bufb, bufc;

    for (size_t i = 0; i < rp; i++) {

        int isOperator = 0;
        char op = '$';
        for (size_t v = 0; v < (sizeof(operators) / sizeof(char)); v++) {
            if (postfix[i][0] == operators[v]) {
                isOperator = 1;
                op = operators[v];
            }
        }

        if (isOperator == 0) {
            top++;
            char tmp[strlen(postfix[i])+1];
            strcpy(tmp, postfix[i]);
            for (size_t v = 0; v < strlen(postfix[i]); v++) {
                switch (tmp[v]) {
                    case '_':
                        tmp[v] = '-';
                        break;
                }
            }
            strcpy(stack[top], tmp);
        } else {
            char bufd[64];
            bufb = atof(stack[top]);
            top--;
            bufa = atof(stack[top]);
            top--;
            bufc = eval(bufa, bufb, op);
            //printf("%f %c %f\n", bufa, op, bufb);
            snprintf(bufd, sizeof(bufd), "%f", bufc);
            top++;
            strcpy(stack[top], bufd);
        }
    }

    if (top < 0) {
        errorcode = 1;
        strcpy(errormsg, "SYNTAX ERROR |[1]| STACK UNDERFLOW");
    }

    return atof(stack[0]);
}

char* readIn(char* prompt) {
    printf("%s ", prompt);
    char* line = NULL;
    size_t size;
    if (getline(&line, &size, stdin) != -1) {
        return line;
    }
    return NULL;
}

int main(int argc, char const *argv[]) {
    for (size_t i = 0; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 's': //Silent mode
                    args.silent = true;
            }
        }
    }


    while (1) {
        //extern char* input;
        /*
        if (args.silent) {
            char* input = readIn("");
        } else {
            char* input = readIn("$> ");
        }*/
        char* input = readIn("$> ");
        if (input == NULL) {
            exit(1);
        }
        if (strcmp(input, "") != 0 && input != NULL) {
            float res = parse(input);
            if (errorcode == 0) {
                if ((res - (int)res) > 0) {
                    printf("%f\n", res);
                } else {
                    printf("%i\n", (int)res);
                }
            } else {
                if (!args.silent) {
                    printf("\n+");
                    for (size_t i = 0; i < (strlen(errormsg) + 7); i++) {
                        printf("-");
                    }
                    printf("+\n");
                    printf("| ERR: %s |\n", errormsg);
                    printf("+");
                    for (size_t i = 0; i < (strlen(errormsg) + 7); i++) {
                        printf("-");
                    }
                    printf("+\n\n");
                    errorcode = 0;
                }
            }
        }
    }
    return 0;
}
