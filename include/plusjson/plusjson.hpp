// Copyright (C) 2015 kkAyataka
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef PLUSJSON_HPP__
#define PLUSJSON_HPP__

#include <algorithm>
#include <exception>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace plusjson {

typedef enum {
    TYPE_NULL,
    TYPE_BOOLEAN,
    TYPE_STRING,
    TYPE_NUMBER,
    TYPE_ARRAY,
    TYPE_OBJECT
} ValueType;

class Value;
struct Null {};
typedef bool Boolean;
typedef std::string String;
typedef double Number;
typedef std::vector<Value> Array;
typedef std::map<String, Value> Object;

class Value {
public:
    explicit Value(const Object & object) {
        type_ = TYPE_OBJECT;
        value_.object = new Object(object);
    }

    explicit Value(const Array & array) {
        type_ = TYPE_ARRAY;
        value_.array = new Array(array);
    }

    explicit Value(const Number & number) {
        type_ = TYPE_NUMBER;
        value_.number = number;
    }

    explicit Value(const String & string) {
        type_ = TYPE_STRING;
        value_.string = new String(string);
    }

    explicit Value(const char * string) {
        type_ = TYPE_STRING;
        value_.string = new String(string);
    }

    explicit Value(const Boolean & boolean) {
        type_ = TYPE_BOOLEAN;
        value_.boolean = boolean;
    }

    Value() {
        type_ = TYPE_NULL;
        value_.null = Null();
    }

    Value(const Value & v) {
        *this = v;
    }

    const Value & operator=(const Value & v) {
        if (this != &v) {
            type_ = v.type_;

            switch (type_) {
            case TYPE_OBJECT: value_.object = new Object(*(v.value_.object)); break;
            case TYPE_ARRAY:  value_.array = new Array(*(v.value_.array));   break;
            case TYPE_STRING: value_.string = new String(*(v.value_.string));  break;
            default: value_ = v.value_;
            }
        }

        return *this;
    }

    ~Value() {
        switch (type_) {
        case TYPE_OBJECT: delete value_.object; break;
        case TYPE_ARRAY:  delete value_.array;  break;
        case TYPE_STRING: delete value_.string; break;
        default: break;
        }
    }

public:
    bool operator==(const Value & v) const {
        if (type_ == v.type_) {
            switch (type_) {
            case TYPE_OBJECT: return *(value_.object) == *(v.value_.object);
            case TYPE_ARRAY: return *(value_.array) == *(v.value_.array);
            case TYPE_STRING: return *(value_.string) == *(v.value_.string);
            case TYPE_NUMBER: return value_.number == v.value_.number;
            case TYPE_BOOLEAN: return value_.boolean == v.value_.boolean;
            default:
                return true;
            }
        }
        else {
            return false;
        }
    }

    bool operator!=(const Value & v) const {
        return !((*this) == v);
    }

    ValueType get_type() const {
        return type_;
    }

    template <class T>
    bool is() const;

public:
    template <class T>
    const T & get() const throw(std::bad_cast);

    template <class T>
    T & get() throw (std::bad_cast);

private:
    ValueType type_;
    union {
        Object * object;
        Array * array;
        Number number;
        String * string;
        Boolean boolean;
        Null null;
    } value_;
};

template <>
inline bool Value::is<Object>() const {
    return type_ == TYPE_OBJECT;
}

template <>
inline bool Value::is<Array>() const {
    return type_ == TYPE_ARRAY;
}

template <>
inline bool Value::is<Number>() const {
    return type_ == TYPE_NUMBER;
}

template <>
inline bool Value::is<String>() const {
    return type_ == TYPE_STRING;
}

template <>
inline bool Value::is<Boolean>() const {
    return type_ == TYPE_BOOLEAN;
}

template <>
inline bool Value::is<Null>() const {
    return type_ == TYPE_NULL;
}

#define DEF_GET(Type, val) \
template <> inline const Type & Value::get() const throw(std::bad_cast) {  \
    if (!is<Type>()) {  \
        throw std::bad_cast(); \
    } \
    return (val); \
} \
template <> inline Type & Value::get() throw(std::bad_cast) { \
    if (!is<Type>()) {  \
        throw std::bad_cast(); \
    } \
    return (val); \
}

DEF_GET(Object, *(value_.object))
DEF_GET(Array, *(value_.array))
DEF_GET(Number, value_.number)
DEF_GET(String, *(value_.string))
DEF_GET(Boolean, value_.boolean)

#undef DEF_GET

std::string to_json_string(const Value & v, const bool readable = true);

namespace detail {

Number to_number(const std::string & json_str, std::size_t & pos) {
    const std::size_t bp = pos;
    for (; pos < json_str.size(); ++pos) {
        const char c = json_str[pos];
        if (c == ' ' || c == '}' || c == ']' || c == ',') {
            break;
        }
    }

    Number v = 0;
    std::stringstream sstm;
    sstm << json_str.substr(bp, pos - bp);
    sstm >> v;

    return v;
}

std::string to_json_string(const Number v) {
    std::stringstream sstm;
    sstm.precision(16);
    sstm << v;

    return sstm.str();
}

String to_string(const std::string & json_str, std::size_t & pos) {
    std::string str;
    pos += 1; // skip first '\"'
    for (; pos < json_str.size(); ++pos) {
        const char c1 = json_str[pos];
        if (c1 == '\\') {
            const char c2 = json_str[++pos];
            switch (c2) {
            case '\"': str += '\"'; break;
            case '\\': str += '\\'; break;
            case '/':  str += '/';  break;
            case 'b':  str += '\b'; break;
            case 'f':  str += '\f'; break;
            case 'n':  str += '\n'; break;
            case 'r':  str += '\r'; break;
            case 't':  str += '\t'; break;
            case 'u':
            default:
                // error
                break;
            }
        }
        else if (c1 == '\"') {
            break;
        }
        else {
            str += c1;
        }
    }

    return str;
}

std::string to_json_string(const String & str) {
    std::string jstr = "\"";
    for (std::string::const_iterator i = str.begin(); i < str.end(); ++i) {
        switch (*i) {
        case '\"': jstr += "\\\""; break;
        case '\\': jstr += "\\\\"; break;
        case '/':  jstr += "\\/";  break;
        case '\b': jstr += "\\b"; break;
        case '\f': jstr += "\\f"; break;
        case '\n': jstr += "\\n"; break;
        case '\r': jstr += "\\r"; break;
        case '\t': jstr += "\\t"; break;
        default:
            jstr += *i;
            break;
        }
    }

    jstr += "\"";
    return jstr;
}

std::string to_json_string(const Array & arr, const bool readable) {
    std::string js = "[";
    for (Array::const_iterator i = arr.begin(); i != arr.end(); ++i) {
        if (i != arr.begin()) {
            js += (readable) ? ", " : ",";
        }

        js += to_json_string(*i);
    }

    js += "]";
    return js;
}

std::string to_json_string(const Object & obj, const bool readable) {
    std::string js = (readable) ? "{\n" : "{";
    for (Object::const_iterator i = obj.begin(); i != obj.end(); ++i) {
        if (i != obj.begin()) {
            js += (readable) ? ", \n" : ", ";
        }

        js += (readable) ? "  " : "";
        js += to_json_string(i->first);
        js += ": ";
        js += to_json_string(i->second);
    }

    js += (readable) ? "\n}\n" : "}";
    return js;
}

Value from_json_string(const std::string & json_str, std::size_t * offset) {
    *offset = json_str.find_first_not_of(' ', *offset);
    const char c = json_str.at(*offset);

    if (c == 'n') { // null
        *offset += 4;
        return Value();
    }
    else if (c == 'f') { // false
        *offset += 5;
        return Value(false);
    }
    else if (c == 't') { // true
        *offset += 4;
        return Value(true);
    }
    else if (c == '-' || (c >= '0' && c <= '9')) { // number
        return Value(to_number(json_str, *offset));
    }
    else if (c == '\"') { // string
        return Value(to_string(json_str, *offset));
    }
    else if (c == '[') { // Array
        *offset += 1;
        Array arr;
        do {
            arr.push_back(from_json_string(json_str, offset));
            *offset = json_str.find_first_not_of(' ', *offset);
        } while (*offset < json_str.size() && json_str[*offset] != ']');

        return Value(arr);
    }
    else if (c == '{') { // Object
        *offset += 1;
        Object obj;
        do {
            const Value k = from_json_string(json_str, offset);
            if (!k.is<String>()) {
                break;
            }
            *offset = json_str.find_first_of(':', *offset) + 1;
            const Value v = from_json_string(json_str, offset);

            obj[k.get<String>()] = v;

            *offset += 1;
            *offset = json_str.find_first_not_of(' ', *offset);
        } while (*offset < json_str.size() && json_str[*offset] != '}');

        return Value(obj);
    }
    else {
        throw std::invalid_argument("Can not parse json string.");
    }
}

} // namespace detail

Value from_json_string(const std::string & json_str) {
    std::string str = json_str;
    std::replace(str.begin(), str.end(), '\r', ' ');
    std::replace(str.begin(), str.end(), '\n', ' ');
    std::replace(str.begin(), str.end(), '\t', ' ');
    std::size_t p = 0;
    return detail::from_json_string(str, &p);
}

std::string to_json_string(const Value & v, const bool readable) {
    switch (v.get_type()) {
    case TYPE_NULL:
        return "null";
    case TYPE_BOOLEAN:
        return (v.get<Boolean>()) ? "true" : "false";
    case TYPE_NUMBER:
        return detail::to_json_string(v.get<Number>());
    case TYPE_STRING:
        return detail::to_json_string(v.get<String>());
    case TYPE_ARRAY:
        return detail::to_json_string(v.get<Array>(), readable);
    case TYPE_OBJECT:
        return detail::to_json_string(v.get<Object>(), readable);
    default:
        return "";
    }
}

} // namespace plusjson

#endif // PLUSJSON_HPP__
