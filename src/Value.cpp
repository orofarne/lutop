#include "Value.hpp"

namespace lutop {

Value::invalid_cast::~invalid_cast() {

}

Value::invalid_cast::invalid_cast()
    : logic_error("Invalid cast")
{
}

Value::Value(Type v_type)
    : type_(v_type)
    , boolean_v_(false)
    , numeric_v_(0)
{
}

Value::Value()
    : type_(Type::Nil)
{
}

Value::Value(const Value& v)
    : type_(v.type_)
{
    switch(v.type_) {
        case Type::Nil:
            // do nothing
            break;
        case Type::Boolean:
            boolean_v_ = v.boolean_v_;
            break;
        case Type::Numeric:
            numeric_v_ = v.numeric_v_;
            break;
        case Type::String:
        case Type::Table:
            buf_v_ = v.buf_v_;
            break;
    }
}

Value::~Value() throw() {

}

const std::string &
Value::rawBuffer() {
    if(type_ != Type::Table)
        throw invalid_cast{};
    return buf_v_;
}

}
