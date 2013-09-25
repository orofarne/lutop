#pragma once

#include <string>
#include <stdexcept>

#include <boost/lexical_cast.hpp>

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

    private:
        template<typename T1, typename T2>
        class MyCast {
            public:
                T1 operator()(const T2 &v) {
                    return (T1)v;
                }
        };
};

template<typename T2>
class Value::MyCast<std::string, T2> {
    public:
        std::string operator()(const T2 &v) {
            return std::to_string(v);
        }
};

template<typename T1>
class Value::MyCast<T1, std::string> {
    public:
        T1 operator()(const std::string &v) {
            return boost::lexical_cast<T1>(v);
        }
};


template<>
class Value::MyCast<std::string, std::string> {
    public:
        std::string operator()(const std::string &v) {
            return v;
        }
};

template<typename T>
Value::Value(Type v_type, T v)
    : type_(v_type)
{
    switch(type_) {
        case Type::Nil:
            break;
        case Type::Boolean:
            boolean_v_ = MyCast<decltype(boolean_v_), T>()(v);
            break;
        case Type::Numeric:
            numeric_v_ = MyCast<decltype(numeric_v_), T>()(v);
            break;
        case Type::String:
            buf_v_ = MyCast<decltype(buf_v_), T>()(v);
            break;
        case Type::Table:
            buf_v_ = MyCast<decltype(buf_v_), T>()(v);
    }
}

template<typename T>
T
Value::as() {
    switch(type_) {
        case Type::Nil:
            throw invalid_cast{};
        case Type::Boolean:
            return MyCast<T, decltype(boolean_v_)>()(boolean_v_);
        case Type::Numeric:
            return MyCast<T, decltype(numeric_v_)>()(numeric_v_);
        case Type::String:
            return MyCast<T, decltype(buf_v_)>()(buf_v_);
        case Type::Table:
            if(typeid(T) == typeid(std::string))
                return MyCast<T, decltype(buf_v_)>()(buf_v_);
            throw invalid_cast{};
    }
}

}
