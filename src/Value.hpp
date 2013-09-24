#pragma once

#include <string>
#include <stdexcept>

namespace lutop {

class Value {
    public:
        enum class Type {
            Nil,
            Boolean,
            Numeric,
            String,
            Table,
        };

        class invalid_cast : public std::logic_error {
            public:
                invalid_cast();
                virtual ~invalid_cast();;
        };

    public:
        Value(Type v_type);
        Value();
        Value(const Value& v);

        template<typename T>
        Value(Type v_type, T v);

        ~Value() throw();

        inline Type type() { return type_; }

        template<typename T>
        T as();

        const std::string &rawBuffer();

    private:
        Type type_;

        bool boolean_v_;
        double numeric_v_;
        std::string buf_v_;
};

template<typename T>
T
Value::as() {
    switch(type_) {
        case Type::Nil:
            throw invalid_cast{};
        case Type::Boolean:
            return (T)boolean_v_;
        case Type::Numeric:
            return (T)numeric_v_;
        case Type::String:
            return (T)buf_v_;
        case Type::Table:
            throw invalid_cast{};
    }
}

}
