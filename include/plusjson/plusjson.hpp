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
typedef struct {} Null;
typedef bool Boolean;
typedef std::string String;
typedef double Number;
typedef std::vector<Value> Array;
typedef std::map<String, Value> Object;

class Value {
public:
    Value(const Object & object) {
        type_ = TYPE_OBJECT;
        value_.object = new Object(object);
    }

    Value(const Array & array) {
        type_ = TYPE_ARRAY;
        value_.array = new Array(array);
    }

    Value(const Number & number) {
        type_ = TYPE_NUMBER;
        value_.number = number;
    }

    Value(const String & string) {
        type_ = TYPE_STRING;
        value_.string = new String(string);
    }

    Value(const char * string) {
        type_ = TYPE_STRING;
        value_.string = new String(string);
    }

    Value(const Boolean & boolean) {
        type_ = TYPE_BOOLEAN;
        value_.boolean = boolean;
    }

    Value(const Null & null) {
        type_ = TYPE_NULL;
        value_.null = Null();
    }

    Value() {
        type_ = TYPE_NULL;
        value_.null = Null();
    }

    Value(const Value & v) {
        init(v);
    }

    const Value & operator=(const Value & v) {
        if (this != &v) {
            del();
            init(v);
        }

        return *this;
    }

    ~Value() {
        del();
    }

private:
    inline void init(const Value & v) {
        type_ = v.type_;

        switch (type_) {
        case TYPE_OBJECT: value_.object = new Object(*(v.value_.object)); break;
        case TYPE_ARRAY:  value_.array = new Array(*(v.value_.array));   break;
        case TYPE_STRING: value_.string = new String(*(v.value_.string));  break;
        default: value_ = v.value_;
        }
    }

    inline void del() {
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

    /** Only object. */
    Value & operator[](const char * key) throw(std::bad_cast);

    /** Only object. */
    Value & operator[](const String key) throw(std::bad_cast);

    /** Only array. */
    Value & operator[](const std::size_t index) throw(std::bad_cast);

    operator Object() const;
    operator Array() const;
    operator Number() const;
    operator String() const;
    operator Boolean() const;

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

inline Value & Value::operator[](const char * key) throw(std::bad_cast) {
    return get<Object>()[key];
}

inline Value & Value::operator[](const String key) throw(std::bad_cast) {
    return (*this)[key.c_str()];
}

inline Value & Value::operator[](const std::size_t index) throw(std::bad_cast) {
    return get<Array>()[index];
}

#define DEF_CAST(Type) \
inline Value::operator Type() const { \
    return get<Type>(); \
}

DEF_CAST(Object)
DEF_CAST(Array)
DEF_CAST(Number)
DEF_CAST(String)
DEF_CAST(Boolean)

#undef DEF_CAST

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
    ++pos; // skip first '\"'
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
            ++pos; // skip end '\"'
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

        js += to_json_string(*i, readable);
    }

    js += "]";
    return js;
}

std::string to_json_string(const Object & obj, const bool readable, unsigned int & indent) {

    std::vector<char> indent_str((indent + 1) * 2, ' ');
    indent_str.push_back(0); // null terminater

    std::string js = (readable) ? "{\n" : "{";
    for (Object::const_iterator i = obj.begin(); i != obj.end(); ++i) {
        if (i != obj.begin()) {
            js += (readable) ? ",\n" : ",";
        }

        js += (readable) ? &indent_str[0] : "";
        js += to_json_string(i->first);
        js += (readable) ? ": " : ":";
        if (i->second.is<Object>()) {
            js += to_json_string(i->second, readable, ++indent);
        }
        else {
            js += to_json_string(i->second, readable);
        }
    }

    if (readable) {
        indent_str.insert(indent_str.end() - 3, 0);

        js += "\n";
        js += &indent_str[0];
        js += "}";
    }
    else {
        js += "}";
    }

    return js;
}

Value to_value(const std::string & json_str, std::size_t * offset) {
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
            arr.push_back(to_value(json_str, offset));
            *offset = json_str.find_first_not_of(' ', *offset);
            *offset = json_str.find_first_not_of(',', *offset);
        } while (*offset < json_str.size() && json_str[*offset] != ']');

        *offset += 1; // skip end ']'

        return Value(arr);
    }
    else if (c == '{') { // Object
        *offset += 1;
        Object obj;
        do {
            const Value k = to_value(json_str, offset);
            if (!k.is<String>()) {
                break;
            }
            *offset = json_str.find_first_of(':', *offset);
            if (*offset >= json_str.size()) {
                throw std::invalid_argument("Can not parse json string");
            }

            *offset += 1; // skip ':'

            const Value v = to_value(json_str, offset);

            obj[k.get<String>()] = v;

            *offset = json_str.find_first_not_of(' ', *offset);
            *offset = json_str.find_first_not_of(',', *offset);
        } while (*offset < json_str.size() && json_str[*offset] != '}');

        *offset += 1; // skip end '}'

        return Value(obj);
    }
    else {
        throw std::invalid_argument("Can not parse json string.");
    }
}

} // namespace detail

Value from_json_string(const std::string & json_str) {
    std::string str = json_str;
    for (auto i = str.begin(); i != str.end(); ++i) {
        if (*i == '\r' || *i == '\n' || *i == '\t') {
            *i = ' ';
        }
    }
    std::size_t p = 0;
    return detail::to_value(str, &p);
}

std::string to_json_string(const Value & v, const bool readable) {
    unsigned int indent = 0;
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
        return detail::to_json_string(v.get<Object>(), readable, indent);
    default:
        return "";
    }
}

} // namespace plusjson

#endif // PLUSJSON_HPP__
