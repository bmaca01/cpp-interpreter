#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <string>
using namespace std;

enum ValType { VINT, VSTR, VERR };

class Value {
    ValType T;
    int I;
    string S;

public:
    Value(): T(VERR), I(0) {}
    Value(int vi): T(VINT), I(vi) {}
    Value(string vs): T(VSTR), I(0), S(vs) {}

    ValType GetType() const { return T; }
    bool isErr() const { return T == VERR; }
    bool isInt() const { return T == VINT; }
    bool isStr() const { return T == VSTR; }

    int getInt() const {if (isInt()) { return I; } throw string("RUNTIME ERROR: Value not an integer");}
    string getStr() const {if (isStr()) { return S; } throw string("RUNTIME ERROR: Value not a string");}

    Value operator+(const Value& op) const;
    Value operator-(const Value& op) const;
    Value operator*(const Value& op) const;
    Value operator/(const Value& op) const;

    friend ostream& operator<<(ostream& out, const Value& op) {
        if (op.isInt()) {
            out << op.I;
        }
        else if (op.isStr()) {
            out << op.S;
        }
        else {
            out << "ERROR";
        }
        return out;
    }
};

#endif
