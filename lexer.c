#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"

static FILE* input;
static int ch;

void initLexer(FILE* in) {
    input = in;
    ch = fgetc(input);
}

static void readChar() {
    ch = fgetc(input);
}

static void skipWhitespace() {
    while (isspace(ch)) readChar();
}

Token getNextToken() {
    Token token;
    skipWhitespace();

    if (ch == EOF) {
        token.type = TOKEN_EOF;
        token.text[0] = '\0';
        return token;
    }

    if (isalpha(ch)) {
        int i = 0;
        while (isalnum(ch) || ch == '_') {
            if (i < 63) token.text[i++] = ch;
            readChar();
        }
        token.text[i] = '\0';
        token.type = TOKEN_IDENTIFIER;
        return token;
    }

    if (isdigit(ch)) {
        int i = 0;
        while (isdigit(ch)) {
            if (i < 63) token.text[i++] = ch;
            readChar();
        }
        token.text[i] = '\0';
        token.type = TOKEN_NUMBER;
        return token;
    }

    // symbols (one char for now)
    token.type = TOKEN_SYMBOL;
    token.text[0] = ch;
    token.text[1] = '\0';
    readChar();
    return token;
}

Token currentToken;
