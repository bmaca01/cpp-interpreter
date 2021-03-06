#include<cctype>
#include<map>

using std::map;

#include "lex.h"

static map<string, Token> kwmap = {
    { "print", PRINT },
    { "println", PRINTLN },
    { "repeat", REPEAT },
    { "begin", BEGIN },
    { "end", END },
};

Tok id_or_kw(const string& lexeme, int linenum) {
    Token tt = IDENT;

    auto kIt = kwmap.find(lexeme);
    // look for the TOKEN in kwmap
    // if found, set tt to it
    if (kIt != kwmap.end())
        tt = kIt->second;

    return Tok(tt, lexeme, linenum);
}

Tok getNextToken(istream& in, int& linenum) {
    enum TokState { BEGIN, INID, INSTRING, ININT, INCOMMENT };
    TokState lexstate = BEGIN;
    string lexeme;
    char ch;
    bool sawEscape;

    while (in.get(ch)) {
        switch (lexstate) {
            case BEGIN:
                if (ch == '\n')
                    linenum++;

                if (isspace(ch))
                    continue;

                lexeme = ch;

                if (isalpha(ch))
                    lexstate = INID;
                else if (ch == '"') {
                    lexstate = INSTRING;
                    sawEscape = false;
                }
                else if (isdigit(ch)) {
                    lexstate = ININT;
                }
                else if (ch == '/' && in.peek() == '/') 
                    lexstate = INCOMMENT;
                else {
                    Token tt = ERR;
                    switch (ch) {
                        case '+':
                            tt = PLUS;
                            break;
                        case '-':
                            tt = MINUS;
                            break;
                        case '*':
                            tt = STAR;
                            break;
                        case '/':
                            tt = SLASH;
                            break;
                        case '=':
                            tt = EQ;
                            break;
                        case '(':
                            tt = LPAREN;
                            break;
                        case ')':
                            tt = RPAREN;
                            break;
                        case ';':
                            tt = SC;
                            break;
                    }

                    return Tok(tt, lexeme, linenum);
                }
                break;

            case INID:
                if (isalpha(ch) || isdigit(ch)) {
                    lexeme += ch;
                }
                else {
                    in.putback(ch);
                    return id_or_kw(lexeme, linenum);
                }
                break;
            case INSTRING:
                if (sawEscape) {
                    sawEscape = false;
                    if (ch == 'n') ch = '\n';
                    lexeme += ch;
                    break;
                }
                if (ch == '\\') {
                    sawEscape = true;
                    break;
                }
                lexeme += ch;
                if (ch == '\n') {
                    return Tok(ERR, lexeme, linenum);
                }
                if (ch == '"') {
                    lexeme = lexeme.substr(1, lexeme.length()-2);
                    return Tok(SCONST, lexeme, linenum);
                }
                break;
            case ININT:
                if (isdigit(ch)) {
                    lexeme += ch;
                }
                else {
                    in.putback(ch);
                    return Tok(ICONST, lexeme, linenum);
                }
                break;
            case INCOMMENT:
                if (ch == '\n') {
                    ++linenum;
                    lexstate = BEGIN;
                }
                break;
        }
    }
    if (in.eof())
        return Tok(DONE, "", linenum);
    return Tok(ERR, "some strange I/O error", linenum);
}
