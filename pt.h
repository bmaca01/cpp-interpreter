/*
 * pt.h
 */

#ifndef PT_H_
#define PT_H_

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

using std::vector;
using std::map;
using std::string;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;

#include "parse.h"
#include "lex.h"
#include "val.h"

class Pt {
protected:
    int	linenum;
    Pt	*left;
    Pt	*right;

public:
    Pt(int linenum, Pt *l = 0, Pt *r = 0)
        : linenum(linenum), left(l), right(r) {}

    virtual ~Pt() {
        delete left;
        delete right;
    }

    int GetLineNumber() const { return linenum; }

    virtual bool isPlus() const { return false; }
    virtual bool isEq() const { return false; }
    virtual bool isIdent() const { return false; }
    virtual bool isVar() const { return false; }
    virtual bool isConst() const { return false; }
    
    virtual string getId() const { return ""; }
    virtual int get_val() const { return 0; }
    virtual string get_str() const { return ""; }

    virtual Pt* getLeft() { return 0; }
    virtual Pt* getRight() { return 0; }

    virtual Value Eval(map<string, Value>& syms) = 0;

    int NodeCount() const;
    int MaxDepth() const;
    int CountPlus() const;
    int CountEq() const;

};

class StmtList : public Pt {
public:
    StmtList(Pt *l, Pt *r) : Pt(0, l, r) {}
    Value Eval(map<string, Value>& syms) {
        left->Eval(syms);
        if (right) {
            right->Eval(syms);
        }
        return Value();
    }
};

class Print : public Pt {
public:
    Print(int line, Pt *e) : Pt(line, e) {}
    Value Eval(map<string, Value>& syms) {
        try
        {
            Value lVal = left->Eval(syms);
            if (lVal.isErr()) {
                throw string("Invalid expression to print");
            }
            
            cout << lVal;
        }
        catch (string& e)
        {
            cout << e << endl;
        }
        return Value();
    }
};

class Println : public Pt {
public:
    Println(int line, Pt *e) : Pt(line, e) {}
    Value Eval(map<string, Value>& syms) {
        try
        {
            Value lVal = left->Eval(syms);
            if (lVal.isErr()) {
                throw string("Invalid expression to print");
            }
            
            cout << lVal << endl;
        }
        catch (string& e)
        {
            cout << e << endl;
        }
        return Value();
    }
};

class Repeat : public Pt {
public:
    Repeat(int line, Pt *e, Pt *stmt) :
        Pt(line, e, stmt) {}
    Value Eval(map<string, Value>& syms) {
        Value lVal = left->Eval(syms);
        int i = 0;
        try
        {
            if (lVal.isErr()) {
                throw string("Repeat expression is not an integer");
            }
            else if (lVal.isStr() && (left->isEq())) {
                if (!(syms.find(lVal.getStr()) != syms.end())) {
                    throw string("Variable not defined");
                }
                else if (syms[lVal.getStr()].isStr()) {
                    throw string("Repeat expression is not an integer");
                }
                i = syms[lVal.getStr()].getInt();
            }
            else if (lVal.isStr() && (left->isConst())) {
                throw string("Repeat expression is not an integer");
            }
            else {
                i = lVal.getInt();
            }

            if (right->Eval(syms).isErr()) {
                throw string("Repeat statement invalid");
            }
            for (int x=0; x < i; x++) {
                right->Eval(syms);
            }
        }
        catch (string& e)
        {
            cout << e << endl;
        }
        return Value();
    }
};

class Assign : public Pt {
public:
    Assign(int line, Pt *l, Pt *r) :
        Pt(line, l, r) {}
    bool isEq() const { return true; }
    Pt* getLeft() { return left; }
    Pt* getRight() { return right; }

    Value Eval(map<string, Value>& syms) {
        try
        {
            Value rVal = right->Eval(syms);
            if (rVal.isErr()) {
                throw string("Invalid Expression");
            }

            // If variable is not defined
            if (left->Eval(syms).isErr()) {
                syms.insert(pair<string, Value>(left->getId(), rVal));
            }

            else {
                // Update the variable
                syms[left->getId()] = rVal;
            }

            // If successful, returns the value that was assigned to the
            // identifier
            return syms[left->getId()];
        }
        catch (string& e)
        {
            cout << e << endl;
        }
        return Value();
    }

};

class PlusExpr : public Pt {
public:
    PlusExpr(int line, Pt *l, Pt *r) :
        Pt(line, l, r) {}

    bool isPlus() const { return true; }
    Value Eval(map<string, Value>& syms) {
        Value lhs = left->Eval(syms);
        Value rhs = right->Eval(syms);
        try
        {
            Value res = lhs + rhs;
            return res;
        }
        catch (string& e)
        {
            cout << e << endl;
        }
        return Value();
    }
};

class MinusExpr : public Pt {
public:
    MinusExpr(int line, Pt *l, Pt *r) :
        Pt(line, l, r) {}

    Value Eval(map<string, Value>& syms) {
        Value lhs = left->Eval(syms);
        Value rhs = right->Eval(syms);
        try
        {
            Value res = lhs - rhs;
            return res;
        }
        catch (string& e)
        {
            cout << e << endl;
        }
        return Value();
    }
};

class TimesExpr : public Pt {
public:
    TimesExpr(int line, Pt *l, Pt *r) : 
        Pt(line, l, r) {}

    Value Eval(map<string, Value>& syms) {
        Value lhs = left->Eval(syms);
        Value rhs = right->Eval(syms);
        try
        {
            Value res = lhs * rhs;
            return res;
        }
        catch (string& e)
        {
            cout << e << endl;
        }
        return Value();
    }
};

class DivideExpr : public Pt {
public:
    DivideExpr(int line, Pt *l, Pt *r) :
        Pt(line, l, r) {}

    Value Eval(map<string, Value>& syms) {
        Value lhs = left->Eval(syms);
        Value rhs = right->Eval(syms);
        try
        {
            Value res = lhs / rhs;
            return res;
        }
        catch (string& e)
        {
            cout << e << endl;
        }
        return Value();
    }
};

class IConst : public Pt {
    int val;

public:
    IConst(Tok& t) : Pt(t.GetLinenum()) {
        val = stoi(t.GetLexeme());
    }
    int get_val() const { return val; }
    bool isConst() const { return true; }
    Value Eval(map<string, Value>& syms) {
        return Value(val);
    }
};

class SConst : public Pt {
    string val;

public:
    SConst(Tok& t) : Pt(t.GetLinenum()) {
        val = t.GetLexeme();
    }
    string get_str() const { return val; }
    bool isConst() const { return true; }
    Value Eval(map<string, Value>& syms) {
        return Value(val);
    }
};

class Ident : public Pt {
    string val;

public:
    Ident(Tok& t) : Pt(t.GetLinenum()) {
        val = t.GetLexeme();
    }

    bool isIdent() const { return true; }
    string getId() const { return val; }
    Value Eval(map<string, Value>& syms) {
        if (syms.find(val) == syms.end()) {
            // Return an error if the Identifier is not defined
            return Value();
        }
        return Value(syms[val]);
    }
};

#endif /* PT_H_ */
