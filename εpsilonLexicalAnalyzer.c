/*
 * εpsilon LEXICAL ANALYZER - study copy with explanations.
 *
 * WHAT IT DOES IN ONE SENTENCE:
 *   It reads a .epl file one character at a time and walks through a fixed
 *   DFA (a table of states) to cut the text into tokens.
 *
 * THE IDEA:
 *   1. Start in state q0.
 *   2. Look at the next character.
 *   3. Look up "from this state, on this character, which state is next?"
 *      in the table.
 *   4. If there is a next state: go there, keep the character, repeat.
 *   5. If there is no next state: stop. The state you are in says what the
 *      token is (or that it is an error).
 *
 * Only two libraries are used: stdio.h (files, printing) and stdlib.h
 * (memory). There is no string.h, so no strcmp() exists in this program.
 */
#include <stdio.h>
#include <stdlib.h>

/*
 * Every kind of token the lexer can produce gets a number.
 * TOKEN_TYPE_COUNT is not a token; it is just "how many token types there are".
 */
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

/*
 * The short name printed for each token type, e.g. TOKEN_IDENTIFIER -> "IDENT".
 * This is only used for PRINTING. It is never used to decide what a word is.
 */
static const char *tokenName[TOKEN_TYPE_COUNT] = {
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
    [TOKEN_FLOOR_DIVIDE] = "FLOORDIV",
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

/*
 * Character classes. Every character read is put in one of these groups,
 * so the table can say "any letter goes to q1" in one entry instead of 53.
 *   LETTER  = a-z, A-Z and _      DIGIT = 0-9
 *   UNKNOWN = everything else     END_OF_FILE = no more characters
 */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 2
#define END_OF_FILE 3

/* What the lexer is looking at right now. */
static int charClass;           /* class of nextChar (LETTER, DIGIT, ...)     */
static char *lexeme;            /* the characters of the current token        */
static int lexLen;              /* how many characters are in lexeme          */
static int lexCapacity;         /* how much room lexeme has                   */
static int nextChar;            /* the next character, not yet used (lookahead) */
static int nextToken;           /* the token type found by lex()              */
static const char *errorMessage;/* the error message, if the token is ERROR   */
static FILE *in_fp;             /* the .epl file being read                   */

/* Where we are in the file, for error messages and the Symbol Table. */
static int currentLine = 1;     /* line of nextChar                           */
static int currentColumn = 1;   /* column of nextChar                         */
static int tokenLine;           /* line where the current token started       */
static int tokenColumn;         /* column where the current token started     */

#define START 0                 /* q0, the start state                        */
#define STOP -1                 /* "no transition" for one specific character */
#define STATE_COUNT 164         /* states q0 to q163                          */

/*
 * ONE STATE OF THE DFA.
 *   token      What the token is if we STOP in this state.
 *              TOKEN_ERROR means "stopping here is a lexical error".
 *   charNext   Arrows for single characters: charNext['='] = 117 means
 *              "on '=' go to q117". 0 = no arrow listed for this character.
 *              STOP = there is definitely no arrow for this character.
 *   classNext  Arrows for whole groups: classNext[LETTER] = 1 means
 *              "on any letter go to q1". Used only when charNext has 0.
 *              0 = no arrow.
 *
 * Why 0 can mean "no arrow": no state ever goes BACK to q0, so 0 is never a
 * real destination.
 */
typedef struct {
    int token;
    int classNext[3];
    int charNext[256];
} dfaState;

/*
 * THE DFA. This is the whole automaton from the diagrams, written out
 * state by state. It is "static const": it is fixed when the program is
 * compiled and can never be changed or added to while the program runs.
 * This is the answer to "the transitions must not be created dynamically".
 *
 * It is a PARTIAL DFA (same as the diagrams): there is no trap state.
 * A missing arrow simply means "stop here".
 */
static const dfaState dfa[STATE_COUNT] = {
    /* q0 START. Each listed character goes to the state that begins its token
       (e.g. '+' -> q116, 'a' -> q2 for "and"). Letters not listed go to q1
       (identifier), digits go to q113 (integer). Blanks are skipped before
       we get here, by getNonBlank(). */
    [0] = { TOKEN_ERROR, { 1, 113, 0 }, {
        ['\n'] = 158, ['"'] = 142, ['#'] = 150, ['%'] = 125, ['\''] = 139, ['('] = 147, [')'] = 148, ['*'] = 120,
        ['+'] = 116, [','] = 138, ['-'] = 118, ['/'] = 122, [':'] = 137, ['<'] = 133, ['='] = 129, ['>'] = 135,
        ['['] = 131, [']'] = 149, ['^'] = 127, ['a'] = 2, ['b'] = 5, ['c'] = 12, ['d'] = 21, ['e'] = 28,
        ['f'] = 34, ['i'] = 42, ['l'] = 53, ['m'] = 56, ['n'] = 67, ['o'] = 70, ['p'] = 73, ['s'] = 78,
        ['t'] = 84, ['u'] = 90, ['v'] = 95, ['w'] = 108, ['{'] = 145, ['}'] = 146
    } },
    /* q1 IDENTIFIER: any letter, digit or _ stays in q1. */
    [1] = { TOKEN_IDENTIFIER, { 1, 1, 0 }, { 0 } },

    /* q2-q112 KEYWORDS, NOISE WORDS, RESERVED WORDS, true/false.
       Each word is a chain of states, one per letter. Example "and":
         q0 -a-> q2 -n-> q3 -d-> q4 (KEYWORD and)
       q2 and q3 are IDENTIFIER states: if the input is "an" and then a space,
       we stop in q3 and get an identifier "an". Any other letter/digit/_ goes
       to q1, so "andy" is an identifier and so is "and_1".
       THIS is how keywords are found without comparing strings: the word is
       recognized letter by letter, by the states it passes through. */
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

    /* q113-q115 NUMBERS.
         q113 INTEGER  (digits)            e.g. 45
         q114 not accepting (digits + '.') e.g. "5." stops here = error
         q115 DECIMAL  (digits.digits)     e.g. 115.5
       A letter or _ right after a number goes to q162 (see below). */
    [113] = { TOKEN_INTEGER_LITERAL, { 162, 113, 0 }, { ['.'] = 114 } },
    [114] = { TOKEN_ERROR, { 0, 115, 0 }, { 0 } },
    [115] = { TOKEN_DECIMAL_LITERAL, { 162, 115, 0 }, { 0 } },

    /* q116-q130 OPERATORS. Each one is "symbol" or "symbol followed by =":
         q116 +   then '=' -> q117 +=
       The longer one wins because we keep reading while there is an arrow.
       '/' (q122) has three ways on: '/' -> q123 //, '=' -> q124 /=,
       '*' -> q154 (start of a block comment). */
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
    /* q131 '['. (There is no q132.) */
    [131] = { TOKEN_LEFT_BRACKET, { 0, 0, 0 }, { 0 } },
    /* q133-q136 < <= > >= */
    [133] = { TOKEN_LESS_THAN, { 0, 0, 0 }, { ['='] = 134 } },
    [134] = { TOKEN_LESS_THAN_OR_EQUAL, { 0, 0, 0 }, { 0 } },
    [135] = { TOKEN_GREATER_THAN, { 0, 0, 0 }, { ['='] = 136 } },
    [136] = { TOKEN_GREATER_THAN_OR_EQUAL, { 0, 0, 0 }, { 0 } },
    /* q137 ':'   q138 ',' */
    [137] = { TOKEN_COLON, { 0, 0, 0 }, { 0 } },
    [138] = { TOKEN_COMMA, { 0, 0, 0 }, { 0 } },

    /* q139-q141 CHARACTER LITERAL 'A'.
         q0 -'-> q139 -A-> q140 -'-> q141 (CHAR_LIT)
       q139 and q140 are not accepting: stopping there = "not closed".
       A newline (\n or \r) is STOP: a char literal cannot go to the next line.
       Extra cases are q159-q161 below. */
    [139] = { TOKEN_ERROR, { 140, 140, 140 }, { ['\n'] = STOP, ['\r'] = STOP, ['\''] = 159 } },
    [140] = { TOKEN_ERROR, { 160, 160, 160 }, { ['\n'] = STOP, ['\r'] = STOP, ['\''] = 141 } },
    [141] = { TOKEN_CHARACTER_LITERAL, { 0, 0, 0 }, { 0 } },
    /* q142-q144 STRING LITERAL "text".
         q0 -"-> q142 -text-> q143 (loops) -"-> q144 (STR_LIT)
       q142 -"-> q144 directly gives the empty string "".
       A newline is STOP: a string cannot go to the next line. */
    [142] = { TOKEN_ERROR, { 143, 143, 143 }, { ['\n'] = STOP, ['\r'] = STOP, ['"'] = 144 } },
    [143] = { TOKEN_ERROR, { 143, 143, 143 }, { ['\n'] = STOP, ['\r'] = STOP, ['"'] = 144 } },
    [144] = { TOKEN_STRING_LITERAL, { 0, 0, 0 }, { 0 } },
    /* q145 {   q146 }   q147 (   q148 )   q149 ] */
    [145] = { TOKEN_LEFT_BRACE, { 0, 0, 0 }, { 0 } },
    [146] = { TOKEN_RIGHT_BRACE, { 0, 0, 0 }, { 0 } },
    [147] = { TOKEN_LEFT_PARENTHESIS, { 0, 0, 0 }, { 0 } },
    [148] = { TOKEN_RIGHT_PARENTHESIS, { 0, 0, 0 }, { 0 } },
    [149] = { TOKEN_RIGHT_BRACKET, { 0, 0, 0 }, { 0 } },

    /* q150-q151 LINE COMMENT: # and everything up to (not including) the
       end of the line. Both states accept. (There is no q152 or q153.) */
    [150] = { TOKEN_LINE_COMMENT, { 151, 151, 151 }, { ['\n'] = STOP, ['\r'] = STOP } },
    [151] = { TOKEN_LINE_COMMENT, { 151, 151, 151 }, { ['\n'] = STOP, ['\r'] = STOP } },

    /* q154-q157 BLOCK COMMENT, from slash-star to star-slash
       (entered from '/' on '*').
         q155 = inside the comment
         q156 = "the last character was *"; a '/' now closes it -> q157
       If the file ends while we are in q154-q156: "not terminated". */
    [154] = { TOKEN_ERROR, { 155, 155, 155 }, { ['*'] = 156 } },
    [155] = { TOKEN_ERROR, { 155, 155, 155 }, { ['*'] = 156 } },
    [156] = { TOKEN_ERROR, { 155, 155, 155 }, { ['*'] = 156, ['/'] = 157 } },
    [157] = { TOKEN_BLOCK_COMMENT, { 0, 0, 0 }, { 0 } },
    /* q158 NEWLINE: a \n, plus any blank lines and indentation after it,
       becomes ONE NEWLINE token (the statement terminator). */
    [158] = { TOKEN_NEWLINE, { 0, 0, 0 }, { ['\t'] = 158, ['\n'] = 158, ['\v'] = 158, ['\f'] = 158, ['\r'] = 158, [' '] = 158 } },

    /* q159-q161 BAD CHARACTER LITERALS. They keep reading so the whole bad
       literal becomes ONE error:
         q159 ''      (empty)
         q160 'ab...  (too many characters, still reading)
         q161 'ab'    (too many characters, closed) */
    [159] = { TOKEN_ERROR, { 0, 0, 0 }, { 0 } },
    [160] = { TOKEN_ERROR, { 160, 160, 160 }, { ['\n'] = STOP, ['\r'] = STOP, ['\''] = 161 } },
    [161] = { TOKEN_ERROR, { 0, 0, 0 }, { 0 } },

    /* q162 BAD NUMBER: a number followed by a letter or _, e.g. 3stats.
       It keeps reading so "3stats" is ONE error, not "3" and "stats". */
    [162] = { TOKEN_ERROR, { 162, 162, 0 }, { 0 } },
    /* q163 not= : reached from q69 ("not") on '='. */
    [163] = { TOKEN_NOT_EQUAL, { 0, 0, 0 }, { 0 } },
};

/*
 * ERROR MESSAGES, one per non-accepting state (the table in doc 5.1.1).
 * If lex() stops in one of these states, this is the message printed.
 * Also fixed at compile time.
 */
static const char *stateErrorMessage[STATE_COUNT] = {
    [114] = "Decimal point must be followed by a digit!",
    [139] = "Char literal not closed!",
    [140] = "Char literal not closed!",
    [142] = "String not terminated!",
    [143] = "String not terminated!",
    [154] = "Comment block not terminated!",
    [155] = "Comment block not terminated!",
    [156] = "Comment block not terminated!",
    [159] = "Empty char literal!",
    [160] = "Char literal not closed!",
    [161] = "A char literal can only hold exactly one character!",
    [162] = "A number can't be followed by a letter or '_'!",
};

/*
 * getChar(): READ ONE CHARACTER from the file into nextChar and put it in
 * its class (LETTER, DIGIT, UNKNOWN or END_OF_FILE).
 * This is the only place the program reads the source code, and it reads
 * exactly one character each time.
 */
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

/*
 * advance(): we are done with nextChar. Update the line/column counter
 * (a \n starts a new line) and read the next character.
 */
static void advance(void) {
    if (nextChar == '\n') {
        currentLine++;
        currentColumn = 1;
    } else {
        currentColumn++;
    }
    getChar();
}

/*
 * addChar(): KEEP nextChar as part of the current token (add it to lexeme),
 * then move on with advance(). The lexeme grows 100 characters at a time.
 */
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
    advance();
}

/*
 * getNonBlank(): SKIP spaces, tabs, \r, \v and \f between tokens.
 * This is the q0 self-loop in the diagrams. \n is NOT skipped, because a
 * newline is a token (it ends a statement).
 */
static void getNonBlank(void) {
    while (nextChar == ' ' || nextChar == '\t' || nextChar == '\r' || nextChar == '\v' || nextChar == '\f') {
        advance();
    }
}

/*
 * getNextState(state): ONE TRANSITION. "From this state, reading nextChar,
 * where do I go?" It only LOOKS UP the answer in the fixed table:
 *   1. Is there an arrow for this exact character?  (charNext)
 *   2. If not, is there an arrow for its group?     (classNext)
 * Returns the next state, or 0 / STOP if there is no arrow.
 * At the end of the file there are never any arrows.
 */
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

/*
 * lex(): FIND ONE TOKEN. This is the DFA simulation.
 *
 *   1. Skip blanks and remember where the token starts (line, column).
 *   2. Nothing left in the file? Then the token is EOF.
 *   3. Start in q0. While the next character has an arrow:
 *        go to that state and keep the character.
 *   4. No more arrows: STOP and look at the state we are in.
 *        - Still q0 (not even the first character had an arrow,
 *          e.g. '@'): take that one character as an
 *          "Unexpected character!" error.
 *        - Any other state: its "token" field is the answer. If that is
 *          TOKEN_ERROR, also take its message.
 *
 * Example "+=":  q0 -+-> q116 -=-> q117, next char ' ' has no arrow,
 *                stop in q117 -> PLUS_EQ.
 */
static int lex(void) {
    int state = START;
    int next;

    lexLen = 0;
    errorMessage = NULL;
    getNonBlank();
    tokenLine = currentLine;
    tokenColumn = currentColumn;
    if (charClass == END_OF_FILE) {
        nextToken = TOKEN_END_OF_FILE;
        return nextToken;
    }
    /* Step 3: follow arrows as long as there are any. */
    while ((next = getNextState(state)) > 0) {
        state = next;
        addChar();
    }
    /* Step 4: no more arrows - decide the token from the state. */
    if (state == START) {
        addChar();
        nextToken = TOKEN_ERROR;
        errorMessage = "Unexpected character!";
    } else {
        nextToken = dfa[state].token;
        if (nextToken == TOKEN_ERROR) {
            errorMessage = stateErrorMessage[state];
        }
    }
    return nextToken;
}

/*
 * printLexeme(): print the token's text. A newline is shown as \n and a tab
 * as \t so every token fits on one line of output; \r is left out.
 */
static void printLexeme(FILE *out) {
    for (int i = 0; i < lexLen; i++) {
        if (lexeme[i] == '\n') {
            fputs("\\n", out);
        } else if (lexeme[i] == '\t') {
            fputs("\\t", out);
        } else if (lexeme[i] != '\r') {
            fputc(lexeme[i], out);
        }
    }
}

/*
 * hasEpsilonExtension(): does the file name end in ".epl"?
 * Checked with a tiny state machine, not strcmp():
 *   '.' -> state 1, then 'e' -> 2, 'p' -> 3, 'l' -> 4.
 *   Anything else resets to 0. The name is accepted if it ends in state 4.
 * "i > 0" means a file called just ".epl" is rejected.
 */
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

/*
 * makeSymbolTablePath(): build "<folder of the .epl file>/Symbol Table.txt".
 * It finds the last / or \ in the path and copies the folder part, then
 * adds the file name, one character at a time.
 */
static char *makeSymbolTablePath(const char *sourcePath) {
    static const char fileName[] = "Symbol Table.txt";
    int folderLength = 0;
    for (int i = 0; sourcePath[i] != '\0'; i++) {
        if (sourcePath[i] == '/' || sourcePath[i] == '\\') {
            folderLength = i + 1;
        }
    }
    char *path = malloc(folderLength + sizeof fileName);
    if (path == NULL) {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }
    for (int i = 0; i < folderLength; i++) {
        path[i] = sourcePath[i];
    }
    for (int i = 0; i < (int)sizeof fileName; i++) {
        path[folderLength + i] = fileName[i];
    }
    return path;
}

/*
 * containsNulByte(): read the whole file once looking for a NUL byte (0).
 * Such files are rejected before scanning (doc 5.2.3). If the file is fine,
 * rewind() goes back to the start so scanning can begin.
 */
static int containsNulByte(FILE *file) {
    int character;
    while ((character = getc(file)) != EOF) {
        if (character == '\0') {
            return 1;
        }
    }
    rewind(file);
    return 0;
}

/*
 * main(): THE WHOLE PROGRAM.
 *   1. Check the file: given? ends in .epl? opens? no NUL byte?
 *   2. Create Symbol Table.txt next to the .epl file and print the headers.
 *   3. Read the first character (getChar), then call lex() again and again.
 *      Each token is printed to the screen and written to the Symbol Table.
 *      An ERROR token also prints a "^ lexical error" line and is counted.
 *      Errors do not stop the program; it keeps going until EOF.
 *   4. Print the summary and return 1 if there were errors, 0 if not.
 */
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
    if (containsNulByte(in_fp)) {
        fprintf(stderr, "Source file contains a NUL byte!\n");
        fclose(in_fp);
        return 1;
    }
    char *tablePath = makeSymbolTablePath(argv[1]);
    FILE *symbolTable = fopen(tablePath, "w");
    if (symbolTable == NULL) {
        fprintf(stderr, "Cannot create %s\n", tablePath);
        fclose(in_fp);
        free(tablePath);
        return 1;
    }

    printf("%-5s %-15s %s\n", "LINE:", "TOKEN:", "LEXEME:");
    printf("%-5s %-15s %s\n", "----", "-----", "------");
    fprintf(symbolTable, "%-5s | %-5s | %-15s | %s\n", "LINE", "COL", "TOKEN", "LEXEME");
    fprintf(symbolTable, "----------------------------------------\n");

    int tokenCount = 0;          /* every token except EOF                 */
    int errorCount = 0;          /* ERROR tokens only                      */
    int isFinished = 0;
    getChar();                   /* read the first character (lookahead)   */
    while (isFinished == 0) {
        lex();                   /* find the next token                    */

        printf("%-5d %-15s ", tokenLine, tokenName[nextToken]);
        printLexeme(stdout);
        putchar('\n');
        fprintf(symbolTable, "%-5d | %-5d | %-15s | ", tokenLine, tokenColumn, tokenName[nextToken]);
        printLexeme(symbolTable);
        fputc('\n', symbolTable);

        if (nextToken == TOKEN_END_OF_FILE) {
            isFinished = 1;
        } else {
            tokenCount++;
            if (nextToken == TOKEN_ERROR) {
                errorCount++;
                printf("^ lexical error (line %d, col %d): %s.\n", tokenLine, tokenColumn, errorMessage);
            }
        }
    }

    if (errorCount > 0) {
        printf("%d lexical error(s) found.\n", errorCount);
    } else {
        printf("No lexical error found. %d tokens.\n", tokenCount);
    }

    fclose(symbolTable);
    printf("Symbol table written to %s.\n", tablePath);
    free(tablePath);
    fclose(in_fp);
    free(lexeme);
    return errorCount > 0;
}
