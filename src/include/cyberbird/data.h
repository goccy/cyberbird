#ifndef __CYBERBIRD_DATA_H__
#define __CYBERBIRD_DATA_H__

#include <string>
#include <vector>
#include <map>

namespace cyberbird {

typedef enum {
    type_null,
    type_number,
    type_string,
    type_array,
    type_object
} value_type;

class value;
typedef std::vector<value> array;
typedef std::map<std::string, value> object;

class value {
public:
    value(void);
    ~value(void);
    value(const value &v);
    explicit value(long long int number);
    explicit value(const object &o);
    explicit value(const array &a);
    explicit value(const std::string &s);
    explicit value(const char *s);
    template<typename T> T &get();
    template<typename T> bool is();
private:
    union _storage {
        bool boolean_;
        long long int number_;
        std::string *string_;
        array *array_;
        object *object_;
    } _storage;
    value_type _type;
};

template<> inline object &value::get<object>() {
    return *this->_storage.object_;
}

template<> inline array &value::get<array>() {
    return *this->_storage.array_;
}

template<> inline std::string &value::get<std::string>() {
    return *this->_storage.string_;
}

template<> inline long long int &value::get<long long int>() {
    return this->_storage.number_;
}

template<> inline bool value::is<object>() {
    return this->_type == type_object;
}

template<> inline bool value::is<array>() {
    return this->_type == type_array;
}

template<> inline bool value::is<std::string>() {
    return this->_type == type_string;
}

template<> inline bool value::is<long long int>() {
    return this->_type == type_number;
}

}

#endif
