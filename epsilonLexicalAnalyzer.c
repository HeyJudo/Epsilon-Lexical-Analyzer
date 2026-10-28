/*---------- LIBRARIES ----------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*---------- TOKENS ----------*/
typedef enum {
    TOKEN_ERROR,
    TOKEN_END_OF_FILE,
    TOKEN_NEWLINE,
    TOKEN_LINE_COMMENT,
    TOKEN_BLOCK_COMMENT,
    TOKEN_IDENTIFIER,
    TOKEN_INTEGER_LITERAL,
    TOKEN_DECIMAL_LITERAL,
    TOKEN_CHARACTER_LITERAL,
    TOKEN_STRING_LITERAL,
    TOKEN_KEYWORD_INTEGER,
    TOKEN_KEYWORD_DECIMAL,
    TOKEN_KEYWORD_CHARACTER,
    TOKEN_KEYWORD_STRING,
    TOKEN_KEYWORD_BOOLEAN,
    TOKEN_KEYWORD_VECTOR,
    TOKEN_KEYWORD_INPUT,
    TOKEN_KEYWORD_PRINT,
    TOKEN_KEYWORD_IF,
    TOKEN_KEYWORD_ELIF,
    TOKEN_KEYWORD_ELSE,
    TOKEN_KEYWORD_WHILE,
    TOKEN_KEYWORD_AND,
    TOKEN_KEYWORD_OR,
    TOKEN_KEYWORD_NOT,
    TOKEN_KEYWORD_TRUE,
    TOKEN_KEYWORD_FALSE,
    TOKEN_FUNCTION_MEAN,
    TOKEN_FUNCTION_MEDIAN,
    TOKEN_FUNCTION_MODE,
    TOKEN_FUNCTION_VARIANCE,
    TOKEN_NOISE_WORD_LET,
    TOKEN_NOISE_WORD_THE,
    TOKEN_NOISE_WORD_OF,
    TOKEN_NOISE_WORD_FROM,
    TOKEN_NOISE_WORD_USING,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_FLOOR_DIVIDE,
    TOKEN_MODULO,
    TOKEN_EXPONENT,
    TOKEN_ASSIGN,
    TOKEN_PLUS_ASSIGN,
    TOKEN_MINUS_ASSIGN,
    TOKEN_MULTIPLY_ASSIGN,
    TOKEN_DIVIDE_ASSIGN,
    TOKEN_MODULO_ASSIGN,
    TOKEN_EXPONENT_ASSIGN,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_LESS_THAN,
    TOKEN_GREATER_THAN,
    TOKEN_LESS_THAN_OR_EQUAL,
    TOKEN_GREATER_THAN_OR_EQUAL,
    TOKEN_LEFT_PARENTHESIS,
    TOKEN_RIGHT_PARENTHESIS,
    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_TYPE_COUNT
} tokenTypeList;

typedef struct {
    tokenTypeList tokenType;
    char *pLexemeText;
    int sourceLineNumber;
    int sourceColumnNumber;
    const char *pErrorMessage;
} tokenValues;

static const char *pTokenTypeName[TOKEN_TYPE_COUNT] = {
    [TOKEN_ERROR] = "ERROR",
    [TOKEN_END_OF_FILE] = "EOF",
    [TOKEN_NEWLINE] = "NEWLINE",
    [TOKEN_LINE_COMMENT] = "LINE_COMMENT",
    [TOKEN_BLOCK_COMMENT] = "BLOCK_COMMENT",
    [TOKEN_IDENTIFIER] = "IDENT",
    [TOKEN_INTEGER_LITERAL] = "INT_LIT",
    [TOKEN_DECIMAL_LITERAL] = "DEC_LIT",
    [TOKEN_CHARACTER_LITERAL] = "CHAR_LIT",
    [TOKEN_STRING_LITERAL] = "STR_LIT",
    [TOKEN_KEYWORD_INTEGER] = "KW_INTEGER",
    [TOKEN_KEYWORD_DECIMAL] = "KW_DECIMAL",
    [TOKEN_KEYWORD_CHARACTER] = "KW_CHARACTER",
    [TOKEN_KEYWORD_STRING] = "KW_STRING",
    [TOKEN_KEYWORD_BOOLEAN] = "KW_BOOLEAN",
    [TOKEN_KEYWORD_VECTOR] = "KW_VECTOR",
    [TOKEN_KEYWORD_INPUT] = "KW_INPUT",
    [TOKEN_KEYWORD_PRINT] = "KW_PRINT",
    [TOKEN_KEYWORD_IF] = "KW_IF",
    [TOKEN_KEYWORD_ELIF] = "KW_ELIF",
    [TOKEN_KEYWORD_ELSE] = "KW_ELSE",
    [TOKEN_KEYWORD_WHILE] = "KW_WHILE",
    [TOKEN_KEYWORD_AND] = "KW_AND",
    [TOKEN_KEYWORD_OR] = "KW_OR",
    [TOKEN_KEYWORD_NOT] = "KW_NOT",
    [TOKEN_KEYWORD_TRUE] = "KW_TRUE",
    [TOKEN_KEYWORD_FALSE] = "KW_FALSE",
    [TOKEN_FUNCTION_MEAN] = "FN_MEAN",
    [TOKEN_FUNCTION_MEDIAN] = "FN_MEDIAN",
    [TOKEN_FUNCTION_MODE] = "FN_MODE",
    [TOKEN_FUNCTION_VARIANCE] = "FN_VARIANCE",
    [TOKEN_NOISE_WORD_LET] = "NW_LET",
    [TOKEN_NOISE_WORD_THE] = "NW_THE",
    [TOKEN_NOISE_WORD_OF] = "NW_OF",
    [TOKEN_NOISE_WORD_FROM] = "NW_FROM",
    [TOKEN_NOISE_WORD_USING] = "NW_USING",
    [TOKEN_PLUS] = "PLUS",
    [TOKEN_MINUS] = "MINUS",
    [TOKEN_MULTIPLY] = "STAR",
    [TOKEN_DIVIDE] = "SLASH",
    [TOKEN_FLOOR_DIVIDE] = "FLOOR_DIV",
    [TOKEN_MODULO] = "PERCENT",
    [TOKEN_EXPONENT] = "CARET",
    [TOKEN_ASSIGN] = "ASSIGN",
    [TOKEN_PLUS_ASSIGN] = "PLUS_EQ",
    [TOKEN_MINUS_ASSIGN] = "MINUS_EQ",
    [TOKEN_MULTIPLY_ASSIGN] = "STAR_EQ",
    [TOKEN_DIVIDE_ASSIGN] = "SLASH_EQ",
    [TOKEN_MODULO_ASSIGN] = "PERCENT_EQ",
    [TOKEN_EXPONENT_ASSIGN] = "CARET_EQ",
    [TOKEN_EQUAL] = "EQ",
    [TOKEN_NOT_EQUAL] = "NOT_EQ",
    [TOKEN_LESS_THAN] = "LT",
    [TOKEN_GREATER_THAN] = "GT",
    [TOKEN_LESS_THAN_OR_EQUAL] = "LE",
    [TOKEN_GREATER_THAN_OR_EQUAL] = "GE",
    [TOKEN_LEFT_PARENTHESIS] = "LPAREN",
    [TOKEN_RIGHT_PARENTHESIS] = "RPAREN",
    [TOKEN_LEFT_BRACKET] = "LBRACKET",
    [TOKEN_RIGHT_BRACKET] = "RBRACKET",
    [TOKEN_LEFT_BRACE] = "LBRACE",
    [TOKEN_RIGHT_BRACE] = "RBRACE",
    [TOKEN_COMMA] = "COMMA",
    [TOKEN_COLON] = "COLON"
};

static const char *pTokenClassificationName[TOKEN_TYPE_COUNT] = {
    [TOKEN_ERROR] = "Invalid",
    [TOKEN_END_OF_FILE] = "End of File",
    [TOKEN_NEWLINE] = "Delimiter",
    [TOKEN_LINE_COMMENT] = "Comment",
    [TOKEN_BLOCK_COMMENT] = "Comment",
    [TOKEN_IDENTIFIER] = "Identifier",
    [TOKEN_INTEGER_LITERAL] = "Constant",
    [TOKEN_DECIMAL_LITERAL] = "Constant",
    [TOKEN_CHARACTER_LITERAL] = "Constant",
    [TOKEN_STRING_LITERAL] = "Constant",
    [TOKEN_KEYWORD_INTEGER] = "Keyword",
    [TOKEN_KEYWORD_DECIMAL] = "Keyword",
    [TOKEN_KEYWORD_CHARACTER] = "Keyword",
    [TOKEN_KEYWORD_STRING] = "Keyword",
    [TOKEN_KEYWORD_BOOLEAN] = "Keyword",
    [TOKEN_KEYWORD_VECTOR] = "Keyword",
    [TOKEN_KEYWORD_INPUT] = "Keyword",
    [TOKEN_KEYWORD_PRINT] = "Keyword",
    [TOKEN_KEYWORD_IF] = "Keyword",
    [TOKEN_KEYWORD_ELIF] = "Keyword",
    [TOKEN_KEYWORD_ELSE] = "Keyword",
    [TOKEN_KEYWORD_WHILE] = "Keyword",
    [TOKEN_KEYWORD_AND] = "Boolean Operator",
    [TOKEN_KEYWORD_OR] = "Boolean Operator",
    [TOKEN_KEYWORD_NOT] = "Boolean Operator",
    [TOKEN_KEYWORD_TRUE] = "Constant",
    [TOKEN_KEYWORD_FALSE] = "Constant",
    [TOKEN_FUNCTION_MEAN] = "Reserved Word",
    [TOKEN_FUNCTION_MEDIAN] = "Reserved Word",
    [TOKEN_FUNCTION_MODE] = "Reserved Word",
    [TOKEN_FUNCTION_VARIANCE] = "Reserved Word",
    [TOKEN_NOISE_WORD_LET] = "Noise Word",
    [TOKEN_NOISE_WORD_THE] = "Noise Word",
    [TOKEN_NOISE_WORD_OF] = "Noise Word",
    [TOKEN_NOISE_WORD_FROM] = "Noise Word",
    [TOKEN_NOISE_WORD_USING] = "Noise Word",
    [TOKEN_PLUS] = "Arithmetic Operator",
    [TOKEN_MINUS] = "Arithmetic Operator",
    [TOKEN_MULTIPLY] = "Arithmetic Operator",
    [TOKEN_DIVIDE] = "Arithmetic Operator",
    [TOKEN_FLOOR_DIVIDE] = "Arithmetic Operator",
    [TOKEN_MODULO] = "Arithmetic Operator",
    [TOKEN_EXPONENT] = "Arithmetic Operator",
    [TOKEN_ASSIGN] = "Assignment Operator",
    [TOKEN_PLUS_ASSIGN] = "Assignment Operator",
    [TOKEN_MINUS_ASSIGN] = "Assignment Operator",
    [TOKEN_MULTIPLY_ASSIGN] = "Assignment Operator",
    [TOKEN_DIVIDE_ASSIGN] = "Assignment Operator",
    [TOKEN_MODULO_ASSIGN] = "Assignment Operator",
    [TOKEN_EXPONENT_ASSIGN] = "Assignment Operator",
    [TOKEN_EQUAL] = "Boolean Operator",
    [TOKEN_NOT_EQUAL] = "Boolean Operator",
    [TOKEN_LESS_THAN] = "Boolean Operator",
    [TOKEN_GREATER_THAN] = "Boolean Operator",
    [TOKEN_LESS_THAN_OR_EQUAL] = "Boolean Operator",
    [TOKEN_GREATER_THAN_OR_EQUAL] = "Boolean Operator",
    [TOKEN_LEFT_PARENTHESIS] = "Bracket",
    [TOKEN_RIGHT_PARENTHESIS] = "Bracket",
    [TOKEN_LEFT_BRACKET] = "Bracket",
    [TOKEN_RIGHT_BRACKET] = "Bracket",
    [TOKEN_LEFT_BRACE] = "Bracket",
    [TOKEN_RIGHT_BRACE] = "Bracket",
    [TOKEN_COMMA] = "Delimiter",
    [TOKEN_COLON] = "Delimiter"
};

const char *pTokenName(tokenTypeList tokenType) {
    return pTokenTypeName[tokenType];
}

const char *pTokenClassification(tokenTypeList tokenType) {
    return pTokenClassificationName[tokenType];
}

/*---------- AUTOMATON ----------*/
#define MAX_STATES 256
#define CHARACTER_COUNT 256
#define NO_TRANSITION -1
#define NOT_ACCEPTING -1
#define NO_STATE -1
#define LETTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define DIGITS "0123456789"

enum {
    STATE_START = 0,
    STATE_IDENTIFIER = 1,
    STATE_INTEGER = 113,
    STATE_DECIMAL_POINT = 114,
    STATE_DECIMAL = 115,
    STATE_SLASH = 122,
    STATE_FLOOR_DIVIDE = 123,
    STATE_DIVIDE_ASSIGN = 124,
    STATE_CHARACTER_OPEN = 139,
    STATE_CHARACTER_BODY = 140,
    STATE_CHARACTER_CLOSE = 141,
    STATE_STRING_BODY = 142,
    STATE_STRING_TEXT = 143,
    STATE_STRING_CLOSE = 144,
    STATE_LINE_COMMENT = 150,
    STATE_LINE_COMMENT_TEXT = 151,
    STATE_BLOCK_COMMENT = 154,
    STATE_BLOCK_COMMENT_TEXT = 155,
    STATE_BLOCK_COMMENT_STAR = 156,
    STATE_BLOCK_COMMENT_END = 157,
    STATE_NEWLINE = 158,
    STATE_CHARACTER_EMPTY = 159,
    STATE_CHARACTER_EXTRA = 160,
    STATE_CHARACTER_EXTRA_CLOSE = 161,
    STATE_NUMBER_SUFFIX = 162,
    STATE_NOT_EQUAL = 163
};

static int transitionTable[MAX_STATES][CHARACTER_COUNT];
static int acceptedTokenType[MAX_STATES];
static const char *pStateErrorMessage[MAX_STATES];
static int stateCount;

static int getCharacterCode(char character) {
    int characterCode = character;
    if (characterCode < 0) {
        characterCode = characterCode + 256;
    }
    return characterCode;
}

static void addTransition(int fromState, char character, int toState) {
    int characterCode = getCharacterCode(character);
    transitionTable[fromState][characterCode] = toState;
}

static void addTransitionForCharacters(int fromState, const char *pCharacters, int toState) {
    for (int i = 0; pCharacters[i] != '\0'; i++) {
        addTransition(fromState, pCharacters[i], toState);
    }
}

static void addTransitionForAnyCharacter(int fromState, int toState) {
    for (int characterCode = 1; characterCode < CHARACTER_COUNT; characterCode++) {
        transitionTable[fromState][characterCode] = toState;
    }
}

static int getNextState(int currentState, char character) {
    int characterCode = getCharacterCode(character);
    return transitionTable[currentState][characterCode];
}

static int createState(int acceptedToken) {
    if (stateCount >= MAX_STATES) {
        fprintf(stderr, "Too many states, no more room available.\n");
        exit(1);
    }
    int newState = stateCount;
    stateCount++;
    acceptedTokenType[newState] = acceptedToken;
    return newState;
}

static void addSymbol(char symbol, int symbolState, int symbolToken) {
    addTransition(STATE_START, symbol, symbolState);
    acceptedTokenType[symbolState] = symbolToken;
}

static void addSymbolWithEquals(char symbol, int symbolState, int symbolToken, int equalsState, int tokenWithEquals) {
    addSymbol(symbol, symbolState, symbolToken);
    addTransition(symbolState, '=', equalsState);
    acceptedTokenType[equalsState] = tokenWithEquals;
}

static void addKeyword(const char *pKeywordText, int keywordState, int keywordToken) {
    int currentState = STATE_START;
    int i = 0;
    while (pKeywordText[i] != '\0') {
        int nextState = getNextState(currentState, pKeywordText[i]);
        if (nextState == NO_TRANSITION || nextState == STATE_IDENTIFIER) {
            nextState = createState(TOKEN_IDENTIFIER);
            addTransitionForCharacters(nextState, LETTERS, STATE_IDENTIFIER);
            addTransitionForCharacters(nextState, DIGITS, STATE_IDENTIFIER);
            addTransition(nextState, '_', STATE_IDENTIFIER);
            addTransition(currentState, pKeywordText[i], nextState);
        }
        currentState = nextState;
        i++;
    }
    if (currentState != keywordState) {
        fprintf(stderr, "Keyword \"%s\" ends in q%d, but the DFA has q%d.\n", pKeywordText, currentState, keywordState);
        exit(1);
    }
    acceptedTokenType[currentState] = keywordToken;
}

static void clearTransitionTable(void) {
    for (int state = 0; state < MAX_STATES; state++) {
        for (int characterCode = 0; characterCode < CHARACTER_COUNT; characterCode++) {
            transitionTable[state][characterCode] = NO_TRANSITION;
        }
        acceptedTokenType[state] = NOT_ACCEPTING;
        pStateErrorMessage[state] = NULL;
    }
    stateCount = STATE_IDENTIFIER + 1;
}

static void addBlankAndNewlineStates(void) {
    addTransitionForCharacters(STATE_START, " \t\r\v\f", STATE_START);
    addTransition(STATE_START, '\n', STATE_NEWLINE);
    addTransitionForCharacters(STATE_NEWLINE, "\n \t\r\v\f", STATE_NEWLINE);
    acceptedTokenType[STATE_NEWLINE] = TOKEN_NEWLINE;
}

static void addLineCommentStates(void) {
    addTransition(STATE_START, '#', STATE_LINE_COMMENT);
    addTransitionForAnyCharacter(STATE_LINE_COMMENT, STATE_LINE_COMMENT_TEXT);
    addTransitionForCharacters(STATE_LINE_COMMENT, "\r\n", NO_TRANSITION);
    addTransitionForAnyCharacter(STATE_LINE_COMMENT_TEXT, STATE_LINE_COMMENT_TEXT);
    addTransitionForCharacters(STATE_LINE_COMMENT_TEXT, "\r\n", NO_TRANSITION);
    acceptedTokenType[STATE_LINE_COMMENT] = TOKEN_LINE_COMMENT;
    acceptedTokenType[STATE_LINE_COMMENT_TEXT] = TOKEN_LINE_COMMENT;
}

static void addIdentifierStates(void) {
    addTransitionForCharacters(STATE_START, LETTERS, STATE_IDENTIFIER);
    addTransition(STATE_START, '_', STATE_IDENTIFIER);
    addTransitionForCharacters(STATE_IDENTIFIER, LETTERS, STATE_IDENTIFIER);
    addTransitionForCharacters(STATE_IDENTIFIER, DIGITS, STATE_IDENTIFIER);
    addTransition(STATE_IDENTIFIER, '_', STATE_IDENTIFIER);
    acceptedTokenType[STATE_IDENTIFIER] = TOKEN_IDENTIFIER;
}

static void addNumberStates(void) {
    addTransitionForCharacters(STATE_START, DIGITS, STATE_INTEGER);
    addTransitionForCharacters(STATE_INTEGER, DIGITS, STATE_INTEGER);
    acceptedTokenType[STATE_INTEGER] = TOKEN_INTEGER_LITERAL;

    addTransition(STATE_INTEGER, '.', STATE_DECIMAL_POINT);
    addTransitionForCharacters(STATE_DECIMAL_POINT, DIGITS, STATE_DECIMAL);
    addTransitionForCharacters(STATE_DECIMAL, DIGITS, STATE_DECIMAL);
    acceptedTokenType[STATE_DECIMAL] = TOKEN_DECIMAL_LITERAL;
    pStateErrorMessage[STATE_DECIMAL_POINT] = "Decimal point must be followed by a digit!";

    addTransitionForCharacters(STATE_INTEGER, LETTERS, STATE_NUMBER_SUFFIX);
    addTransition(STATE_INTEGER, '_', STATE_NUMBER_SUFFIX);
    addTransitionForCharacters(STATE_DECIMAL, LETTERS, STATE_NUMBER_SUFFIX);
    addTransition(STATE_DECIMAL, '_', STATE_NUMBER_SUFFIX);
    addTransitionForCharacters(STATE_NUMBER_SUFFIX, LETTERS, STATE_NUMBER_SUFFIX);
    addTransitionForCharacters(STATE_NUMBER_SUFFIX, DIGITS, STATE_NUMBER_SUFFIX);
    addTransition(STATE_NUMBER_SUFFIX, '_', STATE_NUMBER_SUFFIX);
    pStateErrorMessage[STATE_NUMBER_SUFFIX] = "A number can't be followed by a letter or '_'!";
}

static void addCharacterLiteralStates(void) {
    addTransition(STATE_START, '\'', STATE_CHARACTER_OPEN);
    addTransitionForAnyCharacter(STATE_CHARACTER_OPEN, STATE_CHARACTER_BODY);
    addTransition(STATE_CHARACTER_OPEN, '\'', STATE_CHARACTER_EMPTY);
    addTransitionForCharacters(STATE_CHARACTER_OPEN, "\r\n", NO_TRANSITION);
    acceptedTokenType[STATE_CHARACTER_CLOSE] = TOKEN_CHARACTER_LITERAL;

    addTransitionForAnyCharacter(STATE_CHARACTER_BODY, STATE_CHARACTER_EXTRA);
    addTransition(STATE_CHARACTER_BODY, '\'', STATE_CHARACTER_CLOSE);
    addTransitionForCharacters(STATE_CHARACTER_BODY, "\r\n", NO_TRANSITION);
    addTransitionForAnyCharacter(STATE_CHARACTER_EXTRA, STATE_CHARACTER_EXTRA);
    addTransition(STATE_CHARACTER_EXTRA, '\'', STATE_CHARACTER_EXTRA_CLOSE);
    addTransitionForCharacters(STATE_CHARACTER_EXTRA, "\r\n", NO_TRANSITION);

    pStateErrorMessage[STATE_CHARACTER_OPEN] = "Char literal not closed!";
    pStateErrorMessage[STATE_CHARACTER_BODY] = "Char literal not closed!";
    pStateErrorMessage[STATE_CHARACTER_EXTRA] = "Char literal not closed!";
    pStateErrorMessage[STATE_CHARACTER_EMPTY] = "Empty char literal!";
    pStateErrorMessage[STATE_CHARACTER_EXTRA_CLOSE] = "A char literal can only hold exactly one character!";
}

static void addStringLiteralStates(void) {
    addTransition(STATE_START, '"', STATE_STRING_BODY);
    addTransitionForAnyCharacter(STATE_STRING_BODY, STATE_STRING_TEXT);
    addTransition(STATE_STRING_BODY, '"', STATE_STRING_CLOSE);
    addTransitionForCharacters(STATE_STRING_BODY, "\r\n", NO_TRANSITION);
    addTransitionForAnyCharacter(STATE_STRING_TEXT, STATE_STRING_TEXT);
    addTransition(STATE_STRING_TEXT, '"', STATE_STRING_CLOSE);
    addTransitionForCharacters(STATE_STRING_TEXT, "\r\n", NO_TRANSITION);
    acceptedTokenType[STATE_STRING_CLOSE] = TOKEN_STRING_LITERAL;
    pStateErrorMessage[STATE_STRING_BODY] = "String not terminated!";
    pStateErrorMessage[STATE_STRING_TEXT] = "String not terminated!";
}

static void addSlashStates(void) {
    addTransition(STATE_START, '/', STATE_SLASH);
    acceptedTokenType[STATE_SLASH] = TOKEN_DIVIDE;

    addTransition(STATE_SLASH, '=', STATE_DIVIDE_ASSIGN);
    acceptedTokenType[STATE_DIVIDE_ASSIGN] = TOKEN_DIVIDE_ASSIGN;

    addTransition(STATE_SLASH, '/', STATE_FLOOR_DIVIDE);
    acceptedTokenType[STATE_FLOOR_DIVIDE] = TOKEN_FLOOR_DIVIDE;

    addTransition(STATE_SLASH, '*', STATE_BLOCK_COMMENT);
    addTransitionForAnyCharacter(STATE_BLOCK_COMMENT, STATE_BLOCK_COMMENT_TEXT);
    addTransition(STATE_BLOCK_COMMENT, '*', STATE_BLOCK_COMMENT_STAR);
    addTransitionForAnyCharacter(STATE_BLOCK_COMMENT_TEXT, STATE_BLOCK_COMMENT_TEXT);
    addTransition(STATE_BLOCK_COMMENT_TEXT, '*', STATE_BLOCK_COMMENT_STAR);
    addTransitionForAnyCharacter(STATE_BLOCK_COMMENT_STAR, STATE_BLOCK_COMMENT_TEXT);
    addTransition(STATE_BLOCK_COMMENT_STAR, '*', STATE_BLOCK_COMMENT_STAR);
    addTransition(STATE_BLOCK_COMMENT_STAR, '/', STATE_BLOCK_COMMENT_END);
    acceptedTokenType[STATE_BLOCK_COMMENT_END] = TOKEN_BLOCK_COMMENT;
    pStateErrorMessage[STATE_BLOCK_COMMENT] = "Comment block not terminated!";
    pStateErrorMessage[STATE_BLOCK_COMMENT_TEXT] = "Comment block not terminated!";
    pStateErrorMessage[STATE_BLOCK_COMMENT_STAR] = "Comment block not terminated!";
}

static void addOperatorStates(void) {
    addSymbolWithEquals('+', 116, TOKEN_PLUS, 117, TOKEN_PLUS_ASSIGN);
    addSymbolWithEquals('-', 118, TOKEN_MINUS, 119, TOKEN_MINUS_ASSIGN);
    addSymbolWithEquals('*', 120, TOKEN_MULTIPLY, 121, TOKEN_MULTIPLY_ASSIGN);
    addSymbolWithEquals('%', 125, TOKEN_MODULO, 126, TOKEN_MODULO_ASSIGN);
    addSymbolWithEquals('^', 127, TOKEN_EXPONENT, 128, TOKEN_EXPONENT_ASSIGN);
    addSymbolWithEquals('=', 129, TOKEN_ASSIGN, 130, TOKEN_EQUAL);
    addSymbolWithEquals('<', 133, TOKEN_LESS_THAN, 134, TOKEN_LESS_THAN_OR_EQUAL);
    addSymbolWithEquals('>', 135, TOKEN_GREATER_THAN, 136, TOKEN_GREATER_THAN_OR_EQUAL);
}

static void addDelimiterStates(void) {
    addSymbol(':', 137, TOKEN_COLON);
    addSymbol(',', 138, TOKEN_COMMA);
    addSymbol('{', 145, TOKEN_LEFT_BRACE);
    addSymbol('}', 146, TOKEN_RIGHT_BRACE);
    addSymbol('(', 147, TOKEN_LEFT_PARENTHESIS);
    addSymbol(')', 148, TOKEN_RIGHT_PARENTHESIS);
    addSymbol('[', 131, TOKEN_LEFT_BRACKET);
    addSymbol(']', 149, TOKEN_RIGHT_BRACKET);
}

static void addKeywordStates(void) {
    addKeyword("and", 4, TOKEN_KEYWORD_AND);
    addKeyword("boolean", 11, TOKEN_KEYWORD_BOOLEAN);
    addKeyword("character", 20, TOKEN_KEYWORD_CHARACTER);
    addKeyword("decimal", 27, TOKEN_KEYWORD_DECIMAL);
    addKeyword("elif", 31, TOKEN_KEYWORD_ELIF);
    addKeyword("else", 33, TOKEN_KEYWORD_ELSE);
    addKeyword("false", 38, TOKEN_KEYWORD_FALSE);
    addKeyword("from", 41, TOKEN_NOISE_WORD_FROM);
    addKeyword("if", 43, TOKEN_KEYWORD_IF);
    addKeyword("input", 47, TOKEN_KEYWORD_INPUT);
    addKeyword("integer", 52, TOKEN_KEYWORD_INTEGER);
    addKeyword("let", 55, TOKEN_NOISE_WORD_LET);
    addKeyword("mean", 59, TOKEN_FUNCTION_MEAN);
    addKeyword("median", 63, TOKEN_FUNCTION_MEDIAN);
    addKeyword("mode", 66, TOKEN_FUNCTION_MODE);
    addKeyword("not", 69, TOKEN_KEYWORD_NOT);
    addKeyword("of", 71, TOKEN_NOISE_WORD_OF);
    addKeyword("or", 72, TOKEN_KEYWORD_OR);
    addKeyword("print", 77, TOKEN_KEYWORD_PRINT);
    addKeyword("string", 83, TOKEN_KEYWORD_STRING);
    addKeyword("the", 86, TOKEN_NOISE_WORD_THE);
    addKeyword("true", 89, TOKEN_KEYWORD_TRUE);
    addKeyword("using", 94, TOKEN_NOISE_WORD_USING);
    addKeyword("variance", 102, TOKEN_FUNCTION_VARIANCE);
    addKeyword("vector", 107, TOKEN_KEYWORD_VECTOR);
    addKeyword("while", 112, TOKEN_KEYWORD_WHILE);

    addTransition(69, '=', STATE_NOT_EQUAL);
    acceptedTokenType[STATE_NOT_EQUAL] = TOKEN_NOT_EQUAL;
}

static void buildTransitionTable(void) {
    clearTransitionTable();
    addBlankAndNewlineStates();
    addLineCommentStates();
    addIdentifierStates();
    addNumberStates();
    addCharacterLiteralStates();
    addStringLiteralStates();
    addSlashStates();
    addOperatorStates();
    addDelimiterStates();
    addKeywordStates();
}

/*---------- SCANNING ----------*/
typedef struct {
    const char *pCurrentCharacter;
    const char *pCurrentLineStart;
    int currentLineNumber;
} lexerState;

void initializeLexer(lexerState *pLexer, const char *pSourceCode) {
    buildTransitionTable();
    pLexer->pCurrentCharacter = pSourceCode;
    pLexer->pCurrentLineStart = pSourceCode;
    pLexer->currentLineNumber = 1;
}

static void finishToken(lexerState *pLexer, tokenValues *pToken, const char *pStart, const char *pEnd) {
    size_t lexemeLength = (size_t)(pEnd - pStart);
    pToken->pLexemeText = malloc(lexemeLength + 1);
    if (pToken->pLexemeText == NULL) {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }
    memcpy(pToken->pLexemeText, pStart, lexemeLength);
    pToken->pLexemeText[lexemeLength] = '\0';

    for (const char *pCharacter = pStart; pCharacter < pEnd; pCharacter++) {
        if (*pCharacter == '\n') {
            pLexer->currentLineNumber++;
            pLexer->pCurrentLineStart = pCharacter + 1;
        }
    }
    pLexer->pCurrentCharacter = pEnd;
}

tokenValues getNextToken(lexerState *pLexer) {
    tokenValues currentToken;
    while (*pLexer->pCurrentCharacter != '\0' && getNextState(STATE_START, *pLexer->pCurrentCharacter) == STATE_START) {
        pLexer->pCurrentCharacter++;
    }
    const char *pTokenStart = pLexer->pCurrentCharacter;
    currentToken.sourceLineNumber = pLexer->currentLineNumber;
    currentToken.sourceColumnNumber = (int)(pTokenStart - pLexer->pCurrentLineStart) + 1;
    currentToken.pErrorMessage = NULL;

    if (*pTokenStart == '\0') {
        currentToken.tokenType = TOKEN_END_OF_FILE;
        finishToken(pLexer, &currentToken, pTokenStart, pTokenStart);
        return currentToken;
    }

    int currentState = STATE_START;
    int lastAcceptingState = NO_STATE;
    const char *pLastAcceptingEnd = pTokenStart;
    const char *pLookahead = pTokenStart;
    int keepReading = 1;

    while (keepReading == 1) {
        int nextState = NO_TRANSITION;
        if (*pLookahead != '\0') {
            nextState = getNextState(currentState, *pLookahead);
        }

        if (nextState == NO_TRANSITION) {
            keepReading = 0;
        } else {
            currentState = nextState;
            pLookahead++;
            if (acceptedTokenType[currentState] != NOT_ACCEPTING) {
                lastAcceptingState = currentState;
                pLastAcceptingEnd = pLookahead;
            }
        }
    }

    if (pStateErrorMessage[currentState] != NULL) {
        currentToken.tokenType = TOKEN_ERROR;
        currentToken.pErrorMessage = pStateErrorMessage[currentState];
        finishToken(pLexer, &currentToken, pTokenStart, pLookahead);
    } else if (lastAcceptingState != NO_STATE) {
        currentToken.tokenType = acceptedTokenType[lastAcceptingState];
        finishToken(pLexer, &currentToken, pTokenStart, pLastAcceptingEnd);
    } else {
        currentToken.tokenType = TOKEN_ERROR;
        currentToken.pErrorMessage = "Unexpected character!";
        finishToken(pLexer, &currentToken, pTokenStart, pTokenStart + 1);
    }
    return currentToken;
}

/*---------- FILE AND OUTPUT ----------*/
#define SOURCE_EXTENSION ".epl"
#define SYMBOL_TABLE_NAME "Symbol Table.txt"

static char *pReadFile(const char *pFilePath) {
    FILE *pFile = fopen(pFilePath, "rb");
    if (pFile == NULL) {
        fprintf(stderr, "Cannot open file: %s\n", pFilePath);
        exit(1);
    }
    fseek(pFile, 0, SEEK_END);
    long fileSize = ftell(pFile);
    rewind(pFile);

    char *pFileContents = malloc(fileSize + 1);
    if (pFileContents == NULL) {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }
    size_t bytesRead = fread(pFileContents, 1, fileSize, pFile);
    pFileContents[bytesRead] = '\0';
    fclose(pFile);

    if (memchr(pFileContents, '\0', bytesRead) != NULL) {
        fprintf(stderr, "Source file contains a NUL byte!\n");
        exit(1);
    }
    return pFileContents;
}

static void printLexeme(FILE *pOutput, const char *pText) {
    for (int i = 0; pText[i] != '\0'; i++) {
        if (pText[i] == '\n') {
            fputs("\\n", pOutput);
        } else if (pText[i] == '\t') {
            fputs("\\t", pOutput);
        } else if (pText[i] != '\r') {
            fputc(pText[i], pOutput);
        }
    }
}

static int hasEpsilonExtension(const char *pFilePath) {
    size_t pathLength = strlen(pFilePath);
    size_t extensionLength = strlen(SOURCE_EXTENSION);
    return pathLength > extensionLength && strcmp(pFilePath + pathLength - extensionLength, SOURCE_EXTENSION) == 0;
}

static char *pMakeSymbolTablePath(const char *pSourcePath) {
    size_t folderLength = 0;
    for (size_t i = 0; pSourcePath[i] != '\0'; i++) {
        if (pSourcePath[i] == '/' || pSourcePath[i] == '\\') {
            folderLength = i + 1;
        }
    }
    char *pTablePath = malloc(folderLength + sizeof(SYMBOL_TABLE_NAME));
    if (pTablePath == NULL) {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }
    memcpy(pTablePath, pSourcePath, folderLength);
    strcpy(pTablePath + folderLength, SYMBOL_TABLE_NAME);
    return pTablePath;
}

/*---------- MAIN ----------*/
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file%s>.\n", argv[0], SOURCE_EXTENSION);
        return 1;
    }
    if (hasEpsilonExtension(argv[1]) == 0) {
        fprintf(stderr, "Invalid file type: only %s files can be read!\n", SOURCE_EXTENSION);
        return 1;
    }
    char *pSourceCode = pReadFile(argv[1]);

    char *pTablePath = pMakeSymbolTablePath(argv[1]);
    FILE *pSymbolTable = fopen(pTablePath, "w");
    if (pSymbolTable == NULL) {
        fprintf(stderr, "Cannot create symbol table: %s.\n", pTablePath);
        exit(1);
    }
    fprintf(pSymbolTable, "%-5s | %-5s | %-15s | %-20s | %s\n", "LINE", "COL", "TOKEN", "CLASSIFICATION", "LEXEME");
    fprintf(pSymbolTable, "---------------------------------------------------------------\n");

    lexerState lexer;
    initializeLexer(&lexer, pSourceCode);

    int tokenCount = 0;
    int errorCount = 0;
    int isFinished = 0;

    printf("%-5s %-15s %s\n", "LINE:", "TOKEN:", "LEXEME:");
    printf("%-5s %-15s %s\n", "----", "-----", "------");

    while (isFinished == 0) {
        tokenValues t = getNextToken(&lexer);

        printf("%-5d %-15s ", t.sourceLineNumber, pTokenName(t.tokenType));
        printLexeme(stdout, t.pLexemeText);
        putchar('\n');
        fprintf(pSymbolTable, "%-5d | %-5d | %-15s | %-20s | ", t.sourceLineNumber, t.sourceColumnNumber, pTokenName(t.tokenType), pTokenClassification(t.tokenType));
        printLexeme(pSymbolTable, t.pLexemeText);
        fputc('\n', pSymbolTable);
        free(t.pLexemeText);

        if (t.tokenType == TOKEN_ERROR) {
            printf("^ lexical error (line %d, col %d): %s.\n", t.sourceLineNumber, t.sourceColumnNumber, t.pErrorMessage);
            errorCount++;
        }

        if (t.tokenType == TOKEN_END_OF_FILE) {
            isFinished = 1;
        } else {
            tokenCount++;
        }
    }

    if (errorCount > 0) {
        printf("\n%d lexical error(s) found.\n", errorCount);
    } else {
        printf("\nNo lexical error found. %d tokens.\n", tokenCount);
    }

    fclose(pSymbolTable);
    printf("Symbol table written to %s.\n", pTablePath);
    free(pTablePath);
    free(pSourceCode);
    return errorCount > 0;
}
