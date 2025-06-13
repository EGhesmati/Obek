#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

typedef enum {
    AST_ASSIGNMENT,
    AST_BINARY_EXPR,
    AST_NUMBER,
    AST_IDENTIFIER
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        struct {
            char identifier[64];
            struct ASTNode* expr;
        } assignment;
        struct {
            char operator;
            struct ASTNode* left;
            struct ASTNode* right;
        } binaryExpr;
        int number;
        char identifier[64];
    };
} ASTNode;

void nextToken() {
    currentToken = getNextToken();
}

void match(TokenType expected) {
    if (currentToken.type == expected) {
        nextToken();
    } else {
        printf("Syntax error: unexpected token '%s'\n", currentToken.text);
        exit(1);
    }
}

ASTNode* parseExpression();

ASTNode* parseTerm() {
    if (currentToken.type == TOKEN_NUMBER) {
        ASTNode* node = malloc(sizeof(ASTNode));
        node->type = AST_NUMBER;
        node->number = atoi(currentToken.text);
        match(TOKEN_NUMBER);
        return node;
    } else if (currentToken.type == TOKEN_IDENTIFIER) {
        ASTNode* node = malloc(sizeof(ASTNode));
        node->type = AST_IDENTIFIER;
        strcpy(node->identifier, currentToken.text);
        match(TOKEN_IDENTIFIER);
        return node;
    } else {
        printf("Syntax error: expected number or identifier but got '%s'\n", currentToken.text);
        exit(1);
    }
}

ASTNode* parseExpression() {
    ASTNode* left = parseTerm();

    while (currentToken.type == TOKEN_SYMBOL &&
           (strcmp(currentToken.text, "+") == 0 || strcmp(currentToken.text, "-") == 0)) {
        char op = currentToken.text[0];
        match(TOKEN_SYMBOL);

        ASTNode* right = parseTerm();

        ASTNode* newNode = malloc(sizeof(ASTNode));
        newNode->type = AST_BINARY_EXPR;
        newNode->binaryExpr.operator = op;
        newNode->binaryExpr.left = left;
        newNode->binaryExpr.right = right;

        left = newNode;
    }

    return left;
}

ASTNode* parseAssignment() {
    if (currentToken.type != TOKEN_IDENTIFIER) {
        printf("Syntax error: expected identifier\n");
        exit(1);
    }

    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_ASSIGNMENT;
    strcpy(node->assignment.identifier, currentToken.text);
    match(TOKEN_IDENTIFIER);

    if (currentToken.type == TOKEN_SYMBOL && strcmp(currentToken.text, "=") == 0) {
        match(TOKEN_SYMBOL);
    } else {
        printf("Syntax error: expected '='\n");
        exit(1);
    }

    node->assignment.expr = parseExpression();

    return node;
}

void printAST(ASTNode* node, int indent) {
    for (int i=0; i<indent; i++) printf("  ");
    if (!node) return;

    switch (node->type) {
        case AST_ASSIGNMENT:
            printf("Assignment:\n");
            for (int i=0; i<indent+1; i++) printf("  ");
            printf("Identifier: %s\n", node->assignment.identifier);
            for (int i=0; i<indent+1; i++) printf("  ");
            printf("Expression:\n");
            printAST(node->assignment.expr, indent + 2);
            break;
        case AST_BINARY_EXPR:
            printf("BinaryExpr: '%c'\n", node->binaryExpr.operator);
            printAST(node->binaryExpr.left, indent + 1);
            printAST(node->binaryExpr.right, indent + 1);
            break;
        case AST_NUMBER:
            printf("Number: %d\n", node->number);
            break;
        case AST_IDENTIFIER:
            printf("Identifier: %s\n", node->identifier);
            break;
    }
}

void freeAST(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case AST_ASSIGNMENT:
            freeAST(node->assignment.expr);
            break;
        case AST_BINARY_EXPR:
            freeAST(node->binaryExpr.left);
            freeAST(node->binaryExpr.right);
            break;
        case AST_NUMBER:
        case AST_IDENTIFIER:
            break;
    }
    free(node);
}

int main() {
    initLexer(stdin);
    nextToken();

    printf("Start parsing:\n");
    ASTNode* root = parseAssignment();

    printAST(root, 0);
    printf("Parsing complete.\n");

    freeAST(root);
    return 0;
}
