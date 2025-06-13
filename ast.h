typedef enum {
    AST_ASSIGNMENT,
    AST_BINARY_EXPR,
    AST_NUMBER,
    AST_IDENTIFIER
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;

    union {
        struct { // Assignment: identifier = expr
            char identifier[64];
            struct ASTNode* expr;
        } assignment;

        struct { // Binary expression: left op right
            char operator;
            struct ASTNode* left;
            struct ASTNode* right;
        } binaryExpr;

        int number; // For numbers

        char identifier[64]; // For identifiers
    };
} ASTNode;
