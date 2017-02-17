#ifndef __CYBERBIRD_STORAGE_H__
#define __CYBERBIRD_STORAGE_H__

#include "data.h"

namespace cyberbird {

class Table {
public:
    Table(const char *tableName);
    virtual ~Table(void);
    array select(double latitude, double longitude, unsigned int zoomLevel);
    array select(double latitude, double longitude);
    void insert(double latitude, double longitude, unsigned int zoomLevel, const value &value);
    void update(double latitude, double longitude, unsigned int zoomLevel, const value &value);
    void remove(double latitude, double longitude);

private:
    const char *_tableName;
};

class Storage {
public:
    Storage(const char *filename);
    virtual ~Storage(void);

    Table *table(const char *tableName);
private:

};

}

#endif
