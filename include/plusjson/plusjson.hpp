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
    TYPE_UNKNOWN,
    TYPE_OBJECT,
    TYPE_ARRAY,
    TYPE_NUMBER,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_NULL,
} ValueType;

class Value;
typedef std::map<std::string, Value> Object;
typedef std::vector<Value> Array;
typedef double Number;
typedef std::string String;
typedef bool Boolean;
struct Null {};

class Value {
public:
    explicit Value(const Object &object) {
        type_ = TYPE_OBJECT;
        value_.object = new Object(object);
    }

    explicit Value(const Array &array) {
        type_ = TYPE_ARRAY;
        value_.array = new Array(array);
    }

    explicit Value(const Number &number) {
        type_ = TYPE_NUMBER;
        value_.number = number;
    }

    explicit Value(const String &string) {
        type_ = TYPE_STRING;
        value_.string = new String(string);
    }

    explicit Value(const Boolean &boolean) {
        type_ = TYPE_BOOL;
        value_.boolean = boolean;
    }

    Value() {
        type_ = TYPE_NULL;
        value_.null = Null();
    }

    Value(const Value& v) {
        *this = v;
    }

    const Value& operator=(const Value& v) {
        if (this != &v) {
            type_ = v.type_;

            switch (type_) {
            case TYPE_OBJECT: value_.object = new Object(*(v.value_.object)); break;
            case TYPE_ARRAY:  value_.array = new Array(*(v.value_.array));   break;
            case TYPE_STRING: value_.string = new String(*(v.value_.string));  break;
            }
        }

        return *this;
    }

    ~Value() {
        switch (type_) {
        case TYPE_OBJECT: delete value_.object; break;
        case TYPE_ARRAY:  delete value_.array;  break;
        case TYPE_STRING: delete value_.string; break;
        }
    }

public:
    ValueType type() const {
        return type_;
    }

    bool is_object() const {
        return type_ == TYPE_OBJECT;
    }

    bool is_array() const {
        return type_ == TYPE_ARRAY;
    }

    bool is_number() const {
        return type_ == TYPE_NUMBER;
    }

    bool is_string() const {
        return type_ == TYPE_STRING;
    }

    bool is_bool() const {
        return type_ == TYPE_BOOL;
    }

    bool is_null() const {
        return type_ == TYPE_NULL;
    }

public:
    template <class T>
    T get() const throw(std::bad_cast);

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

template<>
Object Value::get() const throw(std::bad_cast) {
    if (type_ == TYPE_OBJECT) {
        return *(value_.object);
    }
    else {
        throw std::bad_cast();
    }
}

template<>
Array Value::get() const throw(std::bad_cast) {
    if (type_ == TYPE_ARRAY) {
        return *(value_.array);
    }
    else {
        throw std::bad_cast();
    }
}

template<>
double Value::get() const throw(std::bad_cast) {
    if (type_ == TYPE_NUMBER) {
        return value_.number;
    }
    else {
        throw std::bad_cast();
    }
}

template <>
std::string Value::get() const throw(std::bad_cast) {
    if (type_ == TYPE_STRING) {
        return *(value_.string);
    }
    else {
        throw std::bad_cast();
    }
}

template <>
bool Value::get() const throw(std::bad_cast) {
    if (type_ == TYPE_BOOL) {
        return value_.boolean;
    }
    else {
        throw std::bad_cast();
    }
}

std::string json_string_from_value(const Value &v, const bool readable = true);

namespace detail {

Number number_from_json_string(const std::string &json_str, std::size_t &pos) {
    const std::size_t bp = pos;
    for (; pos < json_str.size(); ++pos) {
        const char c = json_str[pos];
        if (c == ' ' || c == '}' || c == ']' || c == ',') {
            break;
        }
    }

    double v = 0;
    std::stringstream sstm;
    sstm << json_str.substr(bp, pos - bp);
    sstm >> v;

    return v;
}

std::string json_string_from_number(const Number v) {
    std::stringstream sstm;
    sstm.precision(16);
    sstm << v;

    return sstm.str();
}

String string_from_json_string(const std::string &json_str, std::size_t &pos) {
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

std::string json_string_from_string(const String &str) {
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

std::string json_string_from_array(const Array &arr) {
    std::string js = "[";
    for (Array::const_iterator i = arr.begin(); i != arr.end(); ++i) {
        if (i != arr.begin()) {
            js += ", ";
        }

        js += json_string_from_value(*i);
    }

    js += "]";
    return js;
}

std::string json_string_from_object(const Object &obj, const bool readable) {
    std::string js = (readable) ? "{\n" : "{";
    for (Object::const_iterator i = obj.begin(); i != obj.end(); ++i) {
        if (i != obj.begin()) {
            js += (readable) ? ", \n" : ", ";
        }

        js += (readable) ? "  " : "";
        js += json_string_from_string(i->first);
        js += ": ";
        js += json_string_from_value(i->second);
    }

    js += (readable) ? "\n}\n" : "}";
    return js;
}

Value value_from_json_string(const std::string &json_str, std::size_t* offset) {
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
        return Value(number_from_json_string(json_str, *offset));
    }
    else if (c == '\"') { // string
        return Value(string_from_json_string(json_str, *offset));
    }
    else if (c == '[') { // Array
        *offset += 1;
        Array arr;
        do {
            arr.push_back(value_from_json_string(json_str, offset));
            *offset = json_str.find_first_not_of(' ', *offset);
        } while (*offset < json_str.size() && json_str[*offset] != ']');

        return Value(arr);
    }
    else if (c == '{') { // Object
        *offset += 1;
        Object obj;
        do {
            const Value k = value_from_json_string(json_str, offset);
            if (!k.is_string()) {
                break;
            }
            *offset = json_str.find_first_of(':', *offset) + 1;
            const Value v = value_from_json_string(json_str, offset);

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

Value value_from_json_string(const std::string &json_str) {
    std::string str = json_str;
    std::replace(str.begin(), str.end(), '\r', ' ');
    std::replace(str.begin(), str.end(), '\n', ' ');
    std::replace(str.begin(), str.end(), '\t', ' ');
    std::size_t p = 0;
    return detail::value_from_json_string(str, &p);
}

std::string json_string_from_value(const Value &v, const bool readable) {
    switch (v.type()) {
    case TYPE_NULL:
        return "null";
    case TYPE_BOOL:
        return (v.get<bool>()) ? "true" : "false";
    case TYPE_NUMBER:
        return detail::json_string_from_number(v.get<double>());
    case TYPE_STRING:
        return detail::json_string_from_string(v.get<std::string>());
    case TYPE_ARRAY:
        return detail::json_string_from_array(v.get<Array>());
    case TYPE_OBJECT:
        return detail::json_string_from_object(v.get<Object>(), readable);
    default:
        return "";
    }
}

} // namespace plusjson

#endif // PLUSJSON_HPP__
