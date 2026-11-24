# How the εpsilon Lexical Analyzer Works

A guide to `εpsilonLexicalAnalyzer.c` (the commented copy in this folder).
Read it next to the code: every function named here has a comment block above it.

---

## 1. The big picture

The lexer takes a `.epl` file and cuts it into **tokens** (words and symbols
with a type, such as `KW_IF`, `IDENT`, `PLUS_EQ`). It does this with a
**DFA**: a fixed set of states and arrows, the same one drawn in the
documentation's diagrams.

```
 .epl file ──► getChar() ──► lex() ──► one token ──► printed on screen
 (one char      reads 1       walks the              + written to
  at a time)    character     DFA table              Symbol Table.txt
                                 ▲                        │
                                 └──── repeat until EOF ◄─┘
```

---

## 2. What happens when you run it

`main()` does everything in this order:

1. **Checks the file.** Was a file given? Does the name end in `.epl`
   (`hasEpsilonExtension()`)? Does it open? Does it contain a NUL byte
   (`containsNulByte()`)? If any check fails, it prints why and stops.
2. **Creates `Symbol Table.txt`** in the same folder as the `.epl` file
   (`makeSymbolTablePath()`) and prints the column headers.
3. **Reads the first character** with `getChar()`. This character is the
   *lookahead*: the next character not yet used, kept in `nextChar`.
4. **Calls `lex()` again and again.** Each call returns one token. The token is
   printed to the screen and written to the Symbol Table. An `ERROR` token also
   prints a `^ lexical error (line, col): message.` line and adds 1 to the
   error count. Errors never stop the program.
5. **Stops at EOF**, prints the summary (`No lexical error found. N tokens.`
   or `N lexical error(s) found.`) and returns 1 if there were errors.

---

## 3. How one token is found: `lex()`

This is the heart of the program, and it is the DFA simulation:

1. Skip blanks (`getNonBlank()`): spaces, tabs, `\r`, `\v`, `\f`.
   Remember the line and column where the token starts.
2. If the file is finished, the token is `EOF`.
3. Start in **q0**. Look at `nextChar` and ask the table: *from this state, on
   this character, which state is next?* (`getNextState()`).
   - **There is an arrow:** go to that state, keep the character
     (`addChar()`), read the next one, and ask again.
   - **There is no arrow:** stop.
4. Look at the state where it stopped:
   - **Still q0** (the very first character had no arrow, e.g. `@`): take that
     one character as an `Unexpected character!` error.
   - **Any other state:** that state's `token` field is the answer. If it is
     `TOKEN_ERROR`, the state's message from `stateErrorMessage` is used.

That is exactly the rule you described: *read one character, see the next,
transition; when nothing more can be read, the state you are in decides the
lexeme.*

---

## 4. How the table works

Each state is one row of `dfa[]`:

| Field | Meaning | Example |
|---|---|---|
| `token` | What you get if you **stop** here | `dfa[117].token = TOKEN_PLUS_ASSIGN` |
| `charNext[c]` | Arrow for one exact character | `dfa[116].charNext['='] = 117` |
| `classNext[g]` | Arrow for a whole group (letter / digit / other) | `dfa[1].classNext[LETTER] = 1` |

`getNextState()` checks `charNext` first and then `classNext`. The groups are
just a short way of writing many arrows at once, like `[a-z] | [A-Z] | '_'`
in the diagrams.

`0` means "no arrow" (no state ever goes back to q0, so 0 is never a real
destination). `STOP` (-1) means "definitely no arrow for this character",
used for example so a string stops at a newline.

---

## 5. Worked examples

### `x += 1`

| Step | nextChar | State | What happens |
|---|---|---|---|
| lex() #1 | `x` | q0 → q1 | `x` is a letter, not the start of any keyword → identifier |
| | ` ` | q1 | no arrow for space → **stop in q1 → `IDENT x`** |
| lex() #2 | ` ` | | skipped by getNonBlank() |
| | `+` | q0 → q116 | |
| | `=` | q116 → q117 | |
| | ` ` | q117 | no arrow → **stop in q117 → `PLUS_EQ +=`** |
| lex() #3 | `1` | q0 → q113 | digit |
| | `\n` | q113 | no arrow → **stop in q113 → `INT_LIT 1`** |
| lex() #4 | `\n` | q0 → q158 | **`NEWLINE`** |

### `while` vs `whilst`

- `while`: q0 -w-> q108 -h-> q109 -i-> q110 -l-> q111 -e-> **q112** →
  `KW_WHILE`.
- `whilst`: q0 -w-> q108 -h-> q109 -i-> q110 -l-> q111, then `s` has no arrow
  in the keyword path, so it falls back to q1 (identifier) and stays there for
  `t` → `IDENT whilst`.

No string was ever compared. The word was recognized **by the states it passed
through.**

### `3stats` (an error)

q0 -3-> q113 (integer), then `s` is a letter → q162, which keeps reading
`tats`. It stops in q162, whose token is `TOKEN_ERROR` → one error
`3stats`: *A number can't be followed by a letter or '_'!*

### `@`

q0 has no arrow for `@`, so `lex()` is still in q0 → takes `@` as
*Unexpected character!*, and the next `lex()` call continues right after it.

---

## 6. Does it do what the professor asked?

| Requirement | Met? | Where to show it |
|---|---|---|
| Reads the input one character at a time | Yes | `getChar()` is the only place that reads the source, and it calls `getc()` once: one character. |
| Simulates a finite automaton: read a character, transition, repeat | Yes | The `while` loop in `lex()`: `getNextState()` → `state = next` → `addChar()`. |
| When no more transitions are possible, the current state decides the token | Yes | The end of `lex()` reads `dfa[state].token`. |
| Transitions are **not created dynamically** | Yes | `dfa[]` is `static const` and written out in full in the source. Nothing in the program adds, builds or changes a transition, and the compiler would refuse to, because it is `const`. |
| **No string comparison** to assign tokens | Yes | Keywords are state paths (q2–q112). The program does not even include `string.h`, so `strcmp()` is not available. The `tokenName[]` strings are only used for **printing**. |

### Questions the professor might ask, and the answers

- **"Why are blanks skipped by `getNonBlank()` and not by the table?"**
  It is the q0 self-loop from the diagrams, written as a small loop before each
  token so the blanks are not stored in the lexeme. It does the same job as the
  arrow; it only skips the five blank characters.
- **"What are `LETTER`, `DIGIT`, `UNKNOWN`?"** Character groups. `getChar()`
  sorts each character into one by comparing it with `'a'`-`'z'` etc. This is
  comparing **one character**, not comparing strings, and it is the same as
  the `[a-z]` labels on the diagram arrows.
- **"`hasEpsilonExtension()` checks `.epl` - isn't that a string compare?"**
  It is a 5-state mini automaton that reads the file name one character at a
  time. It only checks the file type; it has nothing to do with tokens.
- **"`containsNulByte()` reads the whole file first?"** Yes, once, one
  character at a time, only to reject broken files before scanning (doc
  section 5.2.3). Scanning itself starts from the beginning afterwards.
- **"How is this different from the version you were rejected for?"** The old
  version built its transition table while running (functions like
  `addKeyword()` and `createState()` filled in the arrows at startup). Here
  every arrow already exists in the source code before the program runs.

---

## 7. Function map

| Function | Job | Doc section |
|---|---|---|
| `getChar()` | Read one character, set its group | 3.1 Input Buffering |
| `advance()` | Move past a character, update line/column | 5.2.1 (line, col) |
| `addChar()` | Keep the character in the current lexeme | |
| `getNonBlank()` | Skip blanks (q0 self-loop) | 1.3 Ignored Elements |
| `getNextState()` | One transition, looked up in `dfa[]` | 2.3, 3.2 DFA |
| `lex()` | Find one token (the DFA simulation) | 2.2 Matching Rules, 5.1 |
| `printLexeme()` | Print text with `\n` / `\t` shown | 5.2.1 |
| `hasEpsilonExtension()` | Accept only `.epl` files | 5.2.3 |
| `makeSymbolTablePath()` | Put Symbol Table.txt beside the source | 6.2.2 |
| `containsNulByte()` | Reject files with a NUL byte | 5.2.3 |
| `main()` | Run everything, print, count, summarize | 5.2, 6 |

---

## 8. Running it

```
gcc -std=c99 -Wall -Wextra -o lexer εpsilonLexicalAnalyzer.c
./lexer "Sample Programs/sampleValidProgram.epl"
```

Expected: `No lexical error found. 193 tokens.` and a new
`Sample Programs/Symbol Table.txt`. The invalid sample gives
`16 lexical error(s) found.`, the complete sample `530 tokens`.
