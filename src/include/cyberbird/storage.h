#ifndef __CYBERBIRD_STORAGE_H__
#define __CYBERBIRD_STORAGE_H__

#include "data.h"

namespace cyberbird {

/*
 * | <- TableHeader -> | <- tableName -> | <- columnHeader -> | <- columnName -> | <- columnHeader -> | <- columnName -> | ...
 */
typedef struct {
    size_t tableNameSize;
    size_t columnCount;
} TableHeader;

typedef struct {
    size_t nameLength;
    unsigned int type;
    size_t size;
} ColumnHeader;

class Storage;

class Table {
    friend class Storage;
public:

    class Column {
    public:
        typedef enum {
            Number,
            String
        } Type;

        Column(const char *name, Type type, size_t size);
        Column(const Column &v);
        ~Column(void);
        const char *name(void) { return _name; }
        Type type(void) { return _type; }
        size_t size(void) { return _size; }
    private:
        char *_name;
        Type _type;
        size_t _size;
    };

    class Builder {
    public:
        Builder(const char *tableName);
        ~Builder(void);
        Builder *addNumberColumn(const char *name);
        Builder *addStringColumn(const char *name, size_t fixedSize);
        Table *build(void);
    private:
        const char *_tableName;
        std::vector<Column> _columns;
    };

    Table(const char *tableName);
    virtual ~Table(void);
    const char *name(void);
    const std::vector<Column> &columns();

    array select(double latitude, double longitude, unsigned int zoomLevel);
    array select(double latitude, double longitude);
    void insert(double latitude, double longitude, unsigned int zoomLevel, const value &value);
    void update(double latitude, double longitude, unsigned int zoomLevel, const value &value);
    void remove(double latitude, double longitude);

private:
    char *_name;
    std::vector<Column> _columns;
    Table(const char *tableName, std::vector<Column> columns);
};

class Storage {
public:
    Storage(const char *filename);
    virtual ~Storage(void);

    Table *table(const char *tableName);
    bool createTable(Table *table);
private:
    char *_filename;
    std::vector<Table *> _tables;

    bool existsTable(Table *table);
    void loadTables(void);
    bool flush(void);
};

}

#endif
