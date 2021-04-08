#include "val.h"

using namespace std;

Value Value::operator+(const Value& op) const {
    if (isInt() && op.isInt()) {
        return getInt() + op.getInt();
    }
    else if (isStr() && op.isStr()) {
        return getStr() + op.getStr();
    }
    throw string("RUNTIME ERROR: Can only add int + int or str + str");
}

Value Value::operator-(const Value& op) const {
    if (isInt() && op.isInt()) {
        return getInt() - op.getInt();
    }
    else if (isStr() && op.isStr()) {
        if (getStr().find(op.getStr()) == string::npos) {
            // Returns the original string if the operand is not a substring of
            // the original string
            return Value(getStr());
        }
        return Value(getStr().replace(getStr().find(op.getStr()), op.getStr().length(), ""));
    }
    throw string("RUNTIME ERROR: Can only subtract int - int or str - str");
}

Value Value::operator*(const Value& op) const {
    if (isInt() && op.isInt()) {
        return Value(getInt() * op.getInt());
    }
    else if ((isStr() && op.isInt()) || (isInt() && op.isStr())) {
        string rtn = "";
        if (isStr()) {
            for (int i=0; i<op.getInt(); i++) {
                rtn.append(getStr());
            }
        }
        else if (op.isStr()) {
            for (int i=0; i<getInt(); i++) {
                rtn.append(op.getStr());
            }
        }
        return Value(rtn);
    }
    throw string("RUNTIME ERROR: Can only multiply int * int or str * int");
}

Value Value::operator/(const Value& op) const {
    if (isInt() && op.isInt()) {
        return Value(getInt() / op.getInt());
    }
    throw string("RUNTIME ERROR: Can only divide int / int");
}
