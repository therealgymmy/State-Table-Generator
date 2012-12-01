#include "Command.h"

// Dependencies
#include "Exception.h"
#include "../src/StateTableGenerator.h"

#define REGISTER        '@'
#define COMMENTS        '#'

#define MAKE_IS(func) constexpr bool is##func (char ARG)
#define IS(func, ARG) is##func(ARG)
#define ARG c

#define INPUT_UPPER_LIMIT 128     // ASCII goes up to 128

namespace cmd {

enum State {
    ST_NUL,         // NULL State
    ST_START,       // START State

    ST_ID,          // Numbers that prefixed by REGISTER
    ST_NUMBER,      // Numbers

    ST_KEYWORD,     // Keyword - temporary state during tokenization

    ST_LBRACE,      // {
    ST_RBRACE,      // }
    ST_COLON,       // :
    ST_SEMICOLON,   // ;

    ST_COMMENT,     // COMMENT
    ST_WHITESPACE,  // WHITESPACE

    // This should always be the last member
    // as it indicates the length.
    NUMBER_OF_STATES,
};

inline MAKE_IS (whitespace)
{
    return (ARG == '\t' || ARG == '\n' || ARG == '\r' || ARG == ' ');
}

inline MAKE_IS (letter)     // A==0x41, Z==0x5A, a==0x61, z==0x7A
{
    return ((ARG >= 0x41 && ARG <= 0x5A) ||
            (ARG >= 0x61 && ARG <= 0x7A));
}

inline MAKE_IS (number)    // 0==0x30, 9==0x39
{
    return (ARG >= 0x30 && ARG <= 0x39);
}

inline MAKE_IS (everything)
{
    return (ARG >= 0x0 && ARG <= 0x7F);
}

constexpr State next (State from, char c)
{
    return from == ST_START      && IS(whitespace, c) ? ST_WHITESPACE :
           from == ST_WHITESPACE && IS(whitespace, c) ? ST_WHITESPACE :
           from == ST_START      && IS(letter,     c) ? ST_KEYWORD:
           from == ST_KEYWORD    && IS(letter,     c) ? ST_KEYWORD:
           from == ST_START      && IS(number,     c) ? ST_NUMBER:
           from == ST_NUMBER     && IS(number,     c) ? ST_NUMBER:
           from == ST_START      && (c == REGISTER)   ? ST_ID:
           from == ST_ID         && IS(number,     c) ? ST_ID:
           from == ST_START      && (c == '{')        ? ST_LBRACE:
           from == ST_START      && (c == '}')        ? ST_RBRACE:
           from == ST_START      && (c == ':')        ? ST_COLON:
           from == ST_START      && (c == ';')        ? ST_SEMICOLON:
           from == ST_START      && (c == COMMENTS)   ? ST_COMMENT:
           from == ST_COMMENT    && IS(everything, c) ? ST_COMMENT:
           ST_NUL;
}

// State table
constexpr auto table = stg::create<INPUT_UPPER_LIMIT,
                                   NUMBER_OF_STATES,
                                   State>(next);

#define delta table

constexpr State stateKinds[] = { ST_NUL,        // NULL State
                                 ST_NUL,        // START state

                                 ST_ID,         // ID
                                 ST_NUMBER,     // NUMBER

                                 ST_KEYWORD,    // KEYWORD

                                 ST_LBRACE,     // {
                                 ST_RBRACE,     // }
                                 ST_COLON,      // :
                                 ST_SEMICOLON,  // ;

                                 ST_COMMENT,    // COMMENT
                                 ST_WHITESPACE, // WHITESPACE
                               };

Token::Type getType (const std::string &lexeme, State state)
{
    if (state != ST_KEYWORD) {
        switch (state) {
            case ST_ID:         return Token::ID;
            case ST_NUMBER:     return Token::NUMBER;
            case ST_LBRACE:     return Token::LBRACE;
            case ST_RBRACE:     return Token::RBRACE;
            case ST_COLON:      return Token::COLON;
            case ST_SEMICOLON:  return Token::SEMICOLON;
            default:
                throw RuntimeExcept();
        }
    }

    if      (lexeme == "REPEAT")    { return Token::REPEAT; }
    else if (lexeme == "CREATE")    { return Token::CREATE; }
    else if (lexeme == "REMOVE")    { return Token::REMOVE; }
    else if (lexeme == "JOIN")      { return Token::JOIN; }
    else if (lexeme == "DISJOIN")   { return Token::DISJOIN; }
    else if (lexeme == "MERGE")     { return Token::MERGE; }
    else if (lexeme == "UNMERGE")   { return Token::UNMERGE; }
    else if (lexeme == "HAS")       { return Token::HAS; }
    else if (lexeme == "GRAPH")     { return Token::GRAPH; }
    else if (lexeme == "VERTEX")    { return Token::VERTEX; }
    else if (lexeme == "CYCLE")     { return Token::CYCLE; }
    else if (lexeme == "PATH")      { return Token::PATH; }
    else if (lexeme == "AT")        { return Token::AT; }
    else if (lexeme == "AND")       { return Token::AND; }
    else {
        throw RuntimeExcept();
    }
}

std::vector<Token> tokenize (const std::string &line)
{
    std::vector<Token> list;

    unsigned i = 0;
    unsigned startIndex = 0;
    State state = ST_START;

    if (line.empty()) {
        return list;
    }

    while (true) {
        State nextState = ST_NUL;

        if (i < line.length()) {
            nextState = delta[state][(unsigned) line[i]];
        }

        if (nextState == ST_NUL) {
            // No more transitions possible
            if (stateKinds[state] == ST_NUL) {
                throw BadTokenExcept();
            }

            if (stateKinds[state] != ST_WHITESPACE &&
                stateKinds[state] != ST_COMMENT) {
                std::string lexeme = line.substr(startIndex, i - startIndex);
                Token token(getType(lexeme, stateKinds[state]), lexeme);
                list.push_back(token);
            }

            startIndex = i;
            state = ST_START;

            if (i >= line.length()) {
                break;
            }
        }
        else {
            state = nextState;
            ++i;
        }
    }

    return list;
}

#undef delta

}
