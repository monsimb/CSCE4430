#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ASTNode {
    char *value;           // Value of the node (operator or integer)
    struct ASTNode *left;  // Left child
    struct ASTNode *right; // Right child
} ASTNode;

typedef struct Token {
    char *type; // e.g., "integer", "operator"
    char *value; // The actual token string
} Token;

Token *tokens[100]; // Array to store tokens
int token_count = 0; // Count of tokens
int current_token = 0; // Pointer to current token

// Function declarations
ASTNode* parse_expression();
ASTNode* parse_term();
ASTNode* parse_factor();
void error(const char *msg);
void free_ast(ASTNode *node);
void print_ast(ASTNode *node, int depth);
void parse_tokens_from_file(const char *filename);


// Token parsing from file
void parse_tokens_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Could not open token file: %s\n", filename);
        exit(1);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        Token *token = malloc(sizeof(Token));
        char *type = strtok(line, ",");
        char *value = strtok(NULL, "\n");
        token->type = strdup(type);
        token->value = strdup(value);
        tokens[token_count++] = token;
    }
    fclose(file);
}

// Recursive Descent Parsing
ASTNode* parse_expression() {
    ASTNode *node = parse_term();
    while (current_token < token_count && 
           (strcmp(tokens[current_token]->type, "operator") == 0) &&
           (strcmp(tokens[current_token]->value, "+") == 0 || strcmp(tokens[current_token]->value, "-") == 0)) {
        
        ASTNode *new_node = malloc(sizeof(ASTNode));
        new_node->value = strdup(tokens[current_token]->value);
        new_node->left = node;
        new_node->right = NULL;

        current_token++; // Move to the next token

        new_node->right = parse_term();
        node = new_node; // Update current node
    }
    return node;
}

ASTNode* parse_term() {
    ASTNode *node = parse_factor();
    while (current_token < token_count && 
           (strcmp(tokens[current_token]->type, "operator") == 0) &&
           (strcmp(tokens[current_token]->value, "*") == 0 || strcmp(tokens[current_token]->value, "/") == 0)) {
        
        ASTNode *new_node = malloc(sizeof(ASTNode));
        new_node->value = strdup(tokens[current_token]->value);
        new_node->left = node;
        new_node->right = NULL;

        current_token++; // Move to the next token

        new_node->right = parse_factor();
        node = new_node; // Update current node
    }
    return node;
}

ASTNode* parse_factor() {
    if (current_token >= token_count) {
        error("Unexpected end of input.");
    }

    ASTNode *node = malloc(sizeof(ASTNode));

    if (strcmp(tokens[current_token]->type, "integer") == 0) {
        node->value = strdup(tokens[current_token]->value);
        node->left = node->right = NULL; // No children for integer nodes
        current_token++;
    } else if (strcmp(tokens[current_token]->value, "(") == 0) {
        current_token++; // Consume '('
        node = parse_expression(); // Recursively parse expression
        if (current_token >= token_count || strcmp(tokens[current_token]->value, ")") != 0) {
            error("Expected ')'");
        }
        current_token++; // Consume ')'
    } else {
        error("Expected integer or '('.");
    }

    return node;
}

// Free the AST
void free_ast(ASTNode *node) {
    if (node) {
        free_ast(node->left);
        free_ast(node->right);
        free(node->value);
        free(node);
    }
}

// Print the AST
void print_ast(ASTNode *node, int depth) {
    if (node) {
        for (int i = 0; i < depth; i++) {
            printf("    ");
        }
        printf("%s\n", node->value);
        print_ast(node->left, depth + 1);
        print_ast(node->right, depth + 1);
    }
}

void error(const char *msg) {
    fprintf(stderr, "Syntax Error: %s\n", msg);
    exit(1);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <token_file>\n", argv[0]);
        return 1;
    }

    parse_tokens_from_file(argv[1]);
    
    ASTNode *ast = parse_expression();
    
    printf("Abstract Syntax Tree:\n");
    print_ast(ast, 0);
    
    free_ast(ast);
    return 0;
}
