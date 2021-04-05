#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <string>
using namespace std;

enum ValType { VINT, VSTR, VERR };

class Value {
    ValType T;
    int I;
    String S;

public:
    Value(): T(VERR), I(0) {}
    Value(int vi): T(VINT), I(vi) {}
    Value(string vs): T(VSTR), I(0), S(vs) {}

    ValType GetType() const { return T; }
    bool isErr() const { return T == VERR; }
    bool isINt() const { return T == VINT; }
    bool isStr() const { return T == VSTR; }

    int getInt() const {if (isInt()) { return i; } throw "RUNTIME ERROR: Value not an integer";}
    int getStr() const {if (isStr()) { return S; } throw "RUNTIME ERROR: Value not a string";}

    Value operator+(const Value& op) const;
    Value operator-(const Value& op) const;
    Value operator*(const Value& op) const;
    Value operator/(const Value& op) const;

    friend osteam& operator<<(ostream& out, const Value& op) {
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
