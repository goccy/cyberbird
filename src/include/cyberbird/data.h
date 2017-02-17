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
    explicit value(double number);
    explicit value(const object &o);
    explicit value(const array &a);
    explicit value(const std::string &s);
    explicit value(const char *s);
    template<typename T> T &get();
private:
    union _storage {
        bool boolean_;
        double number_;
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

template<> inline double &value::get<double>() {
    return this->_storage.number_;
}

}

#endif
