%{
#include <stdio.h>
#include <stdlib.h>

void write_output(const char *token, const char *type);
int yywrap(void);  // Declare the yywrap function

%}

%%

[0-9]+       { write_output(yytext, "integer"); }
[+\-*/]     { write_output(yytext, "operator"); }
";"         { write_output(";", "semicolon"); } // Add semicolon token
[ \t\n]+    ; // Ignore whitespace
.           { /* Ignore any other character */ }

%%

// Function to handle writing output
void write_output(const char *token, const char *type) {
    FILE *fp = fopen("output.txt", "a");
    if (fp == NULL) {
        fprintf(stderr, "Could not open output.txt for writing\n");
        exit(1);
    }
    fprintf(fp, "%s,%s\n", token, type);
    fclose(fp);
}

// Definition of the yywrap function
int yywrap(void) {
    return 1;  // Indicate that there is no more input
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    
    FILE *input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        fprintf(stderr, "Could not open input file: %s\n", argv[1]);
        return 1;
    }

    yyin = input_file; // Set the input file for Flex
    yylex();           // Start the lexer
    fclose(input_file);
    
    return 0;
}
