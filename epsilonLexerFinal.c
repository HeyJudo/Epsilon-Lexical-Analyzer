#include <stdio.h>
#include <stdlib.h>

enum {
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
};

static const char *tokenName[TOKEN_TYPE_COUNT] = {
    [TOKEN_ERROR] = "ERROR",
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

#define LETTER 0
#define DIGIT 1
#define UNKNOWN 2
#define END_OF_FILE 3

static int charClass;
static char *lexeme;
static int lexLen;
static int lexCapacity;
static int nextChar;
static int nextToken;
static FILE *in_fp;

#define START 0
#define STOP -1
#define STATE_COUNT 164

typedef struct {
    int token;
    int classNext[3];
    int charNext[256];
} dfaState;

static const dfaState dfa[STATE_COUNT] = {
    [0] = { TOKEN_ERROR, { 1, 113, 0 }, {
        ['\n'] = 158, ['"'] = 142, ['#'] = 150, ['%'] = 125, ['\''] = 139, ['('] = 147, [')'] = 148, ['*'] = 120,
        ['+'] = 116, [','] = 138, ['-'] = 118, ['/'] = 122, [':'] = 137, ['<'] = 133, ['='] = 129, ['>'] = 135,
        ['['] = 131, [']'] = 149, ['^'] = 127, ['a'] = 2, ['b'] = 5, ['c'] = 12, ['d'] = 21, ['e'] = 28,
        ['f'] = 34, ['i'] = 42, ['l'] = 53, ['m'] = 56, ['n'] = 67, ['o'] = 70, ['p'] = 73, ['s'] = 78,
        ['t'] = 84, ['u'] = 90, ['v'] = 95, ['w'] = 108, ['{'] = 145, ['}'] = 146
    } },
    [1] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { 0 } },
    [2] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['n'] = 3 } },
    [3] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['d'] = 4 } },
    [4] = { TOKEN_KEYWORD_AND, { 1, 1, 0 }, { 0 } },
    [5] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['o'] = 6 } },
    [6] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['o'] = 7 } },
    [7] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['l'] = 8 } },
    [8] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['e'] = 9 } },
    [9] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['a'] = 10 } },
    [10] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['n'] = 11 } },
    [11] = { TOKEN_KEYWORD_BOOLEAN, { 1, 1, 0 }, { 0 } },
    [12] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['h'] = 13 } },
    [13] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['a'] = 14 } },
    [14] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['r'] = 15 } },
    [15] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['a'] = 16 } },
    [16] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['c'] = 17 } },
    [17] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['t'] = 18 } },
    [18] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['e'] = 19 } },
    [19] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['r'] = 20 } },
    [20] = { TOKEN_KEYWORD_CHARACTER, { 1, 1, 0 }, { 0 } },
    [21] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['e'] = 22 } },
    [22] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['c'] = 23 } },
    [23] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['i'] = 24 } },
    [24] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['m'] = 25 } },
    [25] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['a'] = 26 } },
    [26] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['l'] = 27 } },
    [27] = { TOKEN_KEYWORD_DECIMAL, { 1, 1, 0 }, { 0 } },
    [28] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['l'] = 29 } },
    [29] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['i'] = 30, ['s'] = 32 } },
    [30] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['f'] = 31 } },
    [31] = { TOKEN_KEYWORD_ELIF, { 1, 1, 0 }, { 0 } },
    [32] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['e'] = 33 } },
    [33] = { TOKEN_KEYWORD_ELSE, { 1, 1, 0 }, { 0 } },
    [34] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['a'] = 35, ['r'] = 39 } },
    [35] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['l'] = 36 } },
    [36] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['s'] = 37 } },
    [37] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['e'] = 38 } },
    [38] = { TOKEN_KEYWORD_FALSE, { 1, 1, 0 }, { 0 } },
    [39] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['o'] = 40 } },
    [40] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['m'] = 41 } },
    [41] = { TOKEN_NOISE_WORD_FROM, { 1, 1, 0 }, { 0 } },
    [42] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['f'] = 43, ['n'] = 44 } },
    [43] = { TOKEN_KEYWORD_IF, { 1, 1, 0 }, { 0 } },
    [44] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['p'] = 45, ['t'] = 48 } },
    [45] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['u'] = 46 } },
    [46] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['t'] = 47 } },
    [47] = { TOKEN_KEYWORD_INPUT, { 1, 1, 0 }, { 0 } },
    [48] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['e'] = 49 } },
    [49] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['g'] = 50 } },
    [50] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['e'] = 51 } },
    [51] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['r'] = 52 } },
    [52] = { TOKEN_KEYWORD_INTEGER, { 1, 1, 0 }, { 0 } },
    [53] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['e'] = 54 } },
    [54] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['t'] = 55 } },
    [55] = { TOKEN_NOISE_WORD_LET, { 1, 1, 0 }, { 0 } },
    [56] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['e'] = 57, ['o'] = 64 } },
    [57] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['a'] = 58, ['d'] = 60 } },
    [58] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['n'] = 59 } },
    [59] = { TOKEN_FUNCTION_MEAN, { 1, 1, 0 }, { 0 } },
    [60] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['i'] = 61 } },
    [61] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['a'] = 62 } },
    [62] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['n'] = 63 } },
    [63] = { TOKEN_FUNCTION_MEDIAN, { 1, 1, 0 }, { 0 } },
    [64] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['d'] = 65 } },
    [65] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['e'] = 66 } },
    [66] = { TOKEN_FUNCTION_MODE, { 1, 1, 0 }, { 0 } },
    [67] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['o'] = 68 } },
    [68] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['t'] = 69 } },
    [69] = { TOKEN_KEYWORD_NOT, { 1, 1, 0 }, { ['='] = 163 } },
    [70] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['f'] = 71, ['r'] = 72 } },
    [71] = { TOKEN_NOISE_WORD_OF, { 1, 1, 0 }, { 0 } },
    [72] = { TOKEN_KEYWORD_OR, { 1, 1, 0 }, { 0 } },
    [73] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['r'] = 74 } },
    [74] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['i'] = 75 } },
    [75] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['n'] = 76 } },
    [76] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['t'] = 77 } },
    [77] = { TOKEN_KEYWORD_PRINT, { 1, 1, 0 }, { 0 } },
    [78] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['t'] = 79 } },
    [79] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['r'] = 80 } },
    [80] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['i'] = 81 } },
    [81] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['n'] = 82 } },
    [82] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['g'] = 83 } },
    [83] = { TOKEN_KEYWORD_STRING, { 1, 1, 0 }, { 0 } },
    [84] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['h'] = 85, ['r'] = 87 } },
    [85] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['e'] = 86 } },
    [86] = { TOKEN_NOISE_WORD_THE, { 1, 1, 0 }, { 0 } },
    [87] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['u'] = 88 } },
    [88] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['e'] = 89 } },
    [89] = { TOKEN_KEYWORD_TRUE, { 1, 1, 0 }, { 0 } },
    [90] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['s'] = 91 } },
    [91] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['i'] = 92 } },
    [92] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['n'] = 93 } },
    [93] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['g'] = 94 } },
    [94] = { TOKEN_NOISE_WORD_USING, { 1, 1, 0 }, { 0 } },
    [95] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['a'] = 96, ['e'] = 103 } },
    [96] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['r'] = 97 } },
    [97] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['i'] = 98 } },
    [98] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['a'] = 99 } },
    [99] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['n'] = 100 } },
    [100] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['c'] = 101 } },
    [101] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['e'] = 102 } },
    [102] = { TOKEN_FUNCTION_VARIANCE, { 1, 1, 0 }, { 0 } },
    [103] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['c'] = 104 } },
    [104] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['t'] = 105 } },
    [105] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['o'] = 106 } },
    [106] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['r'] = 107 } },
    [107] = { TOKEN_KEYWORD_VECTOR, { 1, 1, 0 }, { 0 } },
    [108] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['h'] = 109 } },
    [109] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['i'] = 110 } },
    [110] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['l'] = 111 } },
    [111] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { ['e'] = 112 } },
    [112] = { TOKEN_KEYWORD_WHILE, { 1, 1, 0 }, { 0 } },
    [113] = { TOKEN_INTEGER_LITERAL, { 162, 113, 0 }, { ['.'] = 114 } },
    [114] = { TOKEN_ERROR, { 0, 115, 0 }, { 0 } },
    [115] = { TOKEN_DECIMAL_LITERAL, { 162, 115, 0 }, { 0 } },
    [116] = { TOKEN_PLUS, { 0, 0, 0 }, { ['='] = 117 } },
    [117] = { TOKEN_PLUS_ASSIGN, { 0, 0, 0 }, { 0 } },
    [118] = { TOKEN_MINUS, { 0, 0, 0 }, { ['='] = 119 } },
    [119] = { TOKEN_MINUS_ASSIGN, { 0, 0, 0 }, { 0 } },
    [120] = { TOKEN_MULTIPLY, { 0, 0, 0 }, { ['='] = 121 } },
    [121] = { TOKEN_MULTIPLY_ASSIGN, { 0, 0, 0 }, { 0 } },
    [122] = { TOKEN_DIVIDE, { 0, 0, 0 }, { ['*'] = 154, ['/'] = 123, ['='] = 124 } },
    [123] = { TOKEN_FLOOR_DIVIDE, { 0, 0, 0 }, { 0 } },
    [124] = { TOKEN_DIVIDE_ASSIGN, { 0, 0, 0 }, { 0 } },
    [125] = { TOKEN_MODULO, { 0, 0, 0 }, { ['='] = 126 } },
    [126] = { TOKEN_MODULO_ASSIGN, { 0, 0, 0 }, { 0 } },
    [127] = { TOKEN_EXPONENT, { 0, 0, 0 }, { ['='] = 128 } },
    [128] = { TOKEN_EXPONENT_ASSIGN, { 0, 0, 0 }, { 0 } },
    [129] = { TOKEN_ASSIGN, { 0, 0, 0 }, { ['='] = 130 } },
    [130] = { TOKEN_EQUAL, { 0, 0, 0 }, { 0 } },
    [131] = { TOKEN_LEFT_BRACKET, { 0, 0, 0 }, { 0 } },
    [133] = { TOKEN_LESS_THAN, { 0, 0, 0 }, { ['='] = 134 } },
    [134] = { TOKEN_LESS_THAN_OR_EQUAL, { 0, 0, 0 }, { 0 } },
    [135] = { TOKEN_GREATER_THAN, { 0, 0, 0 }, { ['='] = 136 } },
    [136] = { TOKEN_GREATER_THAN_OR_EQUAL, { 0, 0, 0 }, { 0 } },
    [137] = { TOKEN_COLON, { 0, 0, 0 }, { 0 } },
    [138] = { TOKEN_COMMA, { 0, 0, 0 }, { 0 } },
    [139] = { TOKEN_ERROR, { 140, 140, 140 }, { ['\n'] = STOP, ['\r'] = STOP, ['\''] = 159 } },
    [140] = { TOKEN_ERROR, { 160, 160, 160 }, { ['\n'] = STOP, ['\r'] = STOP, ['\''] = 141 } },
    [141] = { TOKEN_CHARACTER_LITERAL, { 0, 0, 0 }, { 0 } },
    [142] = { TOKEN_ERROR, { 143, 143, 143 }, { ['\n'] = STOP, ['\r'] = STOP, ['"'] = 144 } },
    [143] = { TOKEN_ERROR, { 143, 143, 143 }, { ['\n'] = STOP, ['\r'] = STOP, ['"'] = 144 } },
    [144] = { TOKEN_STRING_LITERAL, { 0, 0, 0 }, { 0 } },
    [145] = { TOKEN_LEFT_BRACE, { 0, 0, 0 }, { 0 } },
    [146] = { TOKEN_RIGHT_BRACE, { 0, 0, 0 }, { 0 } },
    [147] = { TOKEN_LEFT_PARENTHESIS, { 0, 0, 0 }, { 0 } },
    [148] = { TOKEN_RIGHT_PARENTHESIS, { 0, 0, 0 }, { 0 } },
    [149] = { TOKEN_RIGHT_BRACKET, { 0, 0, 0 }, { 0 } },
    [150] = { TOKEN_LINE_COMMENT, { 151, 151, 151 }, { ['\n'] = STOP, ['\r'] = STOP } },
    [151] = { TOKEN_LINE_COMMENT, { 151, 151, 151 }, { ['\n'] = STOP, ['\r'] = STOP } },
    [154] = { TOKEN_ERROR, { 155, 155, 155 }, { ['*'] = 156 } },
    [155] = { TOKEN_ERROR, { 155, 155, 155 }, { ['*'] = 156 } },
    [156] = { TOKEN_ERROR, { 155, 155, 155 }, { ['*'] = 156, ['/'] = 157 } },
    [157] = { TOKEN_BLOCK_COMMENT, { 0, 0, 0 }, { 0 } },
    [158] = { TOKEN_NEWLINE, { 0, 0, 0 }, { ['\t'] = 158, ['\n'] = 158, ['\v'] = 158, ['\f'] = 158, ['\r'] = 158, [' '] = 158 } },
    [159] = { TOKEN_ERROR, { 0, 0, 0 }, { 0 } },
    [160] = { TOKEN_ERROR, { 160, 160, 160 }, { ['\n'] = STOP, ['\r'] = STOP, ['\''] = 161 } },
    [161] = { TOKEN_ERROR, { 0, 0, 0 }, { 0 } },
    [162] = { TOKEN_ERROR, { 162, 162, 0 }, { 0 } },
    [163] = { TOKEN_NOT_EQUAL, { 0, 0, 0 }, { 0 } },
};

static void getChar(void) {
    nextChar = getc(in_fp);
    if (nextChar == EOF) {
        charClass = END_OF_FILE;
    } else if ((nextChar >= 'a' && nextChar <= 'z') || (nextChar >= 'A' && nextChar <= 'Z') || nextChar == '_') {
        charClass = LETTER;
    } else if (nextChar >= '0' && nextChar <= '9') {
        charClass = DIGIT;
    } else {
        charClass = UNKNOWN;
    }
}

static void addChar(void) {
    if (lexLen == lexCapacity) {
        lexCapacity += 100;
        lexeme = realloc(lexeme, lexCapacity);
        if (lexeme == NULL) {
            fprintf(stderr, "Out of memory!\n");
            exit(1);
        }
    }
    lexeme[lexLen++] = (char)nextChar;
}

static void getNonBlank(void) {
    while (nextChar == ' ' || nextChar == '\t' || nextChar == '\r' || nextChar == '\v' || nextChar == '\f') {
        getChar();
    }
}

static int getNextState(int state) {
    if (charClass == END_OF_FILE) {
        return STOP;
    }
    int next = dfa[state].charNext[nextChar];
    if (next == 0) {
        next = dfa[state].classNext[charClass];
    }
    return next;
}

static int lookup(int state) {
    return dfa[state].token;
}

static int lex(void) {
    int state = START;
    int next;

    lexLen = 0;
    getNonBlank();
    if (charClass == END_OF_FILE) {
        nextToken = TOKEN_END_OF_FILE;
        return nextToken;
    }
    while ((next = getNextState(state)) > 0) {
        state = next;
        addChar();
        getChar();
    }
    if (state == START) {
        addChar();
        getChar();
    }
    nextToken = lookup(state);
    return nextToken;
}

static void printRow(FILE *out) {
    int width = 0;
    for (int i = 0; i < lexLen; i++) {
        if (lexeme[i] == '\n') {
            width += fprintf(out, "\\n");
        } else if (lexeme[i] == '\t') {
            width += fprintf(out, "\\t");
        } else if (lexeme[i] != '\r') {
            fputc(lexeme[i], out);
            width++;
        }
    }
    fprintf(out, "%*s %s\n", width < 30 ? 30 - width : 0, "", tokenName[nextToken]);
}

static int hasEpsilonExtension(const char *path) {
    int state = 0;
    for (int i = 0; path[i] != '\0'; i++) {
        if (path[i] == '.' && i > 0) {
            state = 1;
        } else if (state == 1 && path[i] == 'e') {
            state = 2;
        } else if (state == 2 && path[i] == 'p') {
            state = 3;
        } else if (state == 3 && path[i] == 'l') {
            state = 4;
        } else {
            state = 0;
        }
    }
    return state == 4;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file.epl>\n", argv[0]);
        return 1;
    }
    if (!hasEpsilonExtension(argv[1])) {
        fprintf(stderr, "Invalid file type: only .epl files can be read!\n");
        return 1;
    }
    in_fp = fopen(argv[1], "rb");
    if (in_fp == NULL) {
        fprintf(stderr, "Cannot open file: %s\n", argv[1]);
        return 1;
    }
    FILE *symbolTable = fopen("Symbol Table.txt", "w");
    if (symbolTable == NULL) {
        fprintf(stderr, "Cannot create Symbol Table.txt\n");
        return 1;
    }

    printf("%-30s %s\n", "LEXEME", "TOKEN");
    fprintf(symbolTable, "%-30s %s\n", "LEXEME", "TOKEN");

    getChar();
    while (lex() != TOKEN_END_OF_FILE) {
        printRow(stdout);
        printRow(symbolTable);
    }

    fclose(symbolTable);
    fclose(in_fp);
    free(lexeme);
    return 0;
}
