#include "parse.h"

namespace Parser {
    bool pushed_back = false;
    Tok	pushed_token;

    static Tok GetNextToken(istream& in, int& line) {
        if (pushed_back) {
            pushed_back = false;
            return pushed_token;
        }
        return getNextToken(in, line);
    }

    static void PushBackToken(Tok& t) {
        if (pushed_back) {
            abort();
        }
        pushed_back = true;
        pushed_token = t;
    }
}

    /* Globals */
static int error_count = 0;
static bool called_from_REPEAT = false;
    /* End Globals */

void ParseError(int line, string msg) {
    ++error_count;
    cout << line << ": " << msg << endl;
}

Pt *Prog(istream& in, int& line) {
    Pt *sl = Sl(in, line);
    if (sl == 0) {
        ParseError(line, "No statements in program");
    }

    if (error_count) {
        return 0;
    }

    return sl;
}

// Sl is a Statement followed by a Statement List
Pt *Sl(istream& in, int& line) {
    Pt *s = Stmt(in, line);
    if (s == 0) {
        return 0;
    }

    // advance stream pointer
    if (Parser::GetNextToken(in, line) != SC) {
        ParseError(line, "Missing semicolon");
        delete s;
        return 0;
    }

    return new StmtList(s, Sl(in,line));
}

Pt *Stmt(istream& in, int& line) {
    int l = line;

    // advance stream pointer
    Tok t = Parser::GetNextToken(in, line);
    Pt *t1;

    switch (t.GetToken()) {
        case PRINT:
            t1 = PrintStmt(in, l);
            break;
        case PRINTLN:
            t1 = PrintlnStmt(in, l);
            break;
        case REPEAT:
            t1 = RepeatStmt(in, l);
            break;
        default:
            Parser::PushBackToken(t);
            t1 = Expr(in, l);
    }

    if (called_from_REPEAT) {
        called_from_REPEAT = false;
        if (t1->isConst() || t1->isIdent()) {
            ParseError(line, "Invalid statement");
            return 0;
        }
    }

    return t1;

}

Pt *PrintStmt(istream& in, int& line) {
    int l = line;

    Pt *ex = Expr(in, line);
    if (ex == 0) {
        ParseError(line, "Missing expression after print");
        return 0;
    }
    
    return new Print(l, ex);
}

Pt *PrintlnStmt(istream& in, int& line) {
    int l = line;

    Pt *ex = Expr(in, line);
    if (ex == 0) {
        ParseError(line, "Missing expression after println");
        return 0;
    }

    return new Println(l, ex);
}

Pt *RepeatStmt(istream& in, int& line) {
    int l = line;

    Pt *t1 = Expr(in, l);
    if (t1 == 0) {
        ParseError(line, "Missing expression after repeat");
        return 0;
    }
    if (!t1->isConst() || t1->get_val() == 0) {
        ParseError(line, "Invalid Expression");
        return 0;
    }
    

    while (true) {
        Tok t = Parser::GetNextToken(in, line);
        if (t == END) {
            return t1;
        }

        if (t != BEGIN) {
            ParseError(line, "Missing BEGIN token");
            return 0;
        }

        called_from_REPEAT = true;
        Pt *t2 = Stmt(in, line);
        if (t2 == 0) {
            ParseError(line, "Missing statement for repeat");
            return 0;
        }
        t1 = new Repeat(t.GetLinenum(), t1, t2);
    }
}

Pt *Expr(istream& in, int& line) {

    Pt *t1 = Sum(in, line);
    if (t1 == 0) {
        return 0;
    }
    while (true) {
        Tok t = Parser::GetNextToken(in, line);

        if (t != EQ) {
            Parser::PushBackToken(t);
            return t1;
        }

        Pt *t2 = Sum(in, line);
        if (t2 == 0) {
            ParseError(line, "Missing expression after equals");
            return 0;
        }
        t1 = new Assign(t.GetLinenum(), t1, t2);  
    }

}

Pt *Sum(istream& in, int& line) {

    Pt *t1 = Prod(in, line);
    if (t1 == 0) {
        return 0;
    }

    while (true) {
        Tok t = Parser::GetNextToken(in, line);

        if (t != PLUS && t != MINUS) {
            Parser::PushBackToken(t);
            return t1;
        }

        Pt *t2 = Prod(in, line);
        if (t2 == 0) {
            ParseError(line, "Missing expression after operator");
            return 0;
        }

        if (t == PLUS) {
            t1 = new PlusExpr(t.GetLinenum(), t1, t2);
        }
        else {
            t1 = new MinusExpr(t.GetLinenum(), t1, t2);
        }
    }
}

Pt *Prod(istream& in, int& line) {

    Pt *t1 = Primary(in, line);
    if (t1 == 0) {
        return 0;
    }

    while (true) {
        Tok t = Parser::GetNextToken(in, line);

        if (t != STAR && t != SLASH) {
            Parser::PushBackToken(t);
            return t1;
        }

        Pt *t2 = Primary(in, line);
        if (t2 == 0) {
            ParseError(line, "Missing expression after operator");
            return 0;
        }

        if (t == STAR) {
            t1 = new TimesExpr(t.GetLinenum(), t1, t2);
        }
        else {
            t1 = new DivideExpr(t.GetLinenum(), t1, t2);
        }
    }
}

Pt *Primary(istream& in, int& line) {
    Tok t = Parser::GetNextToken(in, line);

    if (t == LPAREN) {
        while (true) {

            Pt *e = Expr(in, line);
            if (e == 0) {
                ParseError(line, "Missing statement in parens");
                return 0;
            }
            else {
                t = Parser::GetNextToken(in, line);
                if (t != RPAREN) {
                    ParseError(line, "Missing RPAREN");
                    return 0;
                }
                return e;
            }
        }
    }

    if (t == IDENT) {
        return new Ident(t);
    }
    else if (t == SCONST) {
        return new SConst(t);
    }
    else if (t == ICONST) {
        return new IConst(t);
    }
    else if (t == DONE) {
        return 0;
    }
    else {
        Parser::PushBackToken(t);
        return 0;
    }
}

