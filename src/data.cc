#include <cyberbird/data.h>

using namespace cyberbird;

value::value(void)
{
}

value::~value(void)
{
    switch (this->_type) {
    case type_number:
        break;
    case type_string:
        delete this->_storage.string_;
        break;
    case type_array:
        delete this->_storage.array_;
        break;
    case type_object:
        delete this->_storage.object_;
        break;
    default:
        break;
    }
}

value::value(const value &v) : _type(v._type)
{
    switch (v._type) {
    case type_number:
        this->_storage.number_ = v._storage.number_;
        break;
    case type_string:
        this->_storage.string_ = new std::string(*v._storage.string_);
        break;
    case type_array:
        this->_storage.array_ = new array(*v._storage.array_);
        break;
    case type_object:
        this->_storage.object_ = new object(*v._storage.object_);
        break;
    default:
        break;
    }
}

value::value(double number) : _type(type_number)
{
    this->_storage.number_ = number;
}

value::value(const object &o) : _type(type_object)
{
    this->_storage.object_ = new object(o);
}

value::value(const array &a) : _type(type_array)
{
    this->_storage.array_ = new array(a);
}

value::value(const std::string &s) : _type(type_string)
{
    this->_storage.string_ = new std::string(s);
}

value::value(const char *s) : _type(type_string)
{
    this->_storage.string_ = new std::string(s);
}
