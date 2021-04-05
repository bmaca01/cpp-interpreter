#include "val.h"

using namespace std;

Value operator+(const Value& op) const {
    if (isInt() && op.isInt()) {
        return getInt() + op.getInt();
    }
    if (isStr() && op.isStr()) {
        return getStr() + op.getStr();
    }
    throw "RUNTIME ERROR";
}

Value operator-(const Value& op) const {
    
}

Value operator*(const Value& op) const {
    
}

Value operator/(const Value& op) const {
    
}
