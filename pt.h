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
            else if (left->isEq()) {
                «»
            }
            «»

        }
        catch («»)
        {
            «»
        }
        «»
        return Value();
    }
};

class Println : public Pt {
public:
    Println(int line, Pt *e) : Pt(line, e) {}
    Value Eval(map<string, Value>& syms) {
        // Placeholder
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
            if (left->Eval(syms).isErr()) {
                // Identifier not defined
                if (left->isEq()) {
                    // If multiple variables are defined in one expression
                    // eg: x = y = z = w = 1;
                    vector<string> idents;
                    vector<Pt *> visited;
                    vector<Pt *>::iterator it;
                    Pt *tmp = left;
                    // Start at the left node
                    while (true) {
                        // if the current node has a left node and it hasn't
                        // been visited yet
                        if (tmp->getLeft() && 
                            !(find(visited.begin(), visited.end(), tmp->getLeft()) != visited.end())) 
                        {
                            visited.push_back(tmp);
                            tmp = tmp->getLeft();
                            continue;
                        }

                        // if the current node has a right node and it hasn't
                        // been visited yet
                        else if (tmp->getRight() && 
                            !(find(visited.begin(), visited.end(), tmp->getRight()) != visited.end())) 
                        {
                            idents.push_back(tmp->getRight()->getId());
                            it--;
                            tmp = *it;
                        }

                        // if the current node is an identifier
                        // This case is only valid if the current node is the
                        // bottom left-most node
                        else if (tmp->isIdent()) {
                            idents.push_back(tmp->getId());
                            tmp = *visited.end();
                            it = visited.end();
                        }

                        else if (it == visited.begin()) {
                            idents.push_back(tmp->getRight()->getId());
                            break;
                        }
                    }

                    // Assign Values to the identifiers found
                    for (auto it = idents.begin(); it != idents.end(); it++) {
                        syms.insert(pair<string, Value>(*it, rVal));
                    }
                }

                else {
                    // If its a single variable definition
                    syms.insert(pair<string, Value>(left->getId(), rVal));
                }
            }

            else {
                // Update the variable
                if (left->getId().size() == 0) {
                    throw string("Cannot assign constant to constant");
                }
                syms[left->getId()] = rVal;
            }

            // If successful, returns the identifier's name as a Value
            return Value(syms[left->getId()]);
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
