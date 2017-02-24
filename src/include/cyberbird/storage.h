#ifndef __CYBERBIRD_STORAGE_H__
#define __CYBERBIRD_STORAGE_H__

#include <cyberbird/data.h>
#include <cyberbird/index_page.h>
#include <cyberbird/reader.h>
#include <cyberbird/writer.h>

namespace cyberbird {

/*
 * | <- TableHeader -> | <- tableName -> | <- columnHeader -> | <- columnName -> | <- columnHeader -> | <- columnName -> | ...
 */
typedef struct {
    size_t tableNameSize;
    size_t columnCount;
    uint64_t lastId;
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

    virtual ~Table(void);
    const char *name(void);
    const std::vector<Column> &columns();

    array select(double latitude, double longitude, unsigned int zoomLevel, unsigned int maxZoomLevel);
    array select(double latitude, double longitude, unsigned int zoomLevel);
    uint64_t insert(double latitude, double longitude, const object &value);
    uint64_t insert(double latitude, double longitude, unsigned int zoomLevel, const object &value);
    void update(double latitude, double longitude, unsigned int zoomLevel, const object &value);
    void remove(double latitude, double longitude);

private:
    char *_name;
    std::vector<Column> _columns;
    uint64_t _lastId;
    Storage *_storage;
    IndexPage *_indexPage;
    std::map<uint64_t, object*> _rows;

    Table(const char *tableName, std::vector<Column> columns);
    size_t rowSize(void);
    bool isValidData(const object &object);
    bool flush(Writer *writer);
    object *copyObject(const object &o);
    bool load(Reader *reader);
};

class Storage {
    friend class Table;
public:
    Storage(const char *filename);
    virtual ~Storage(void);

    Table *table(const char *tableName);
    bool createTable(Table *table);
    bool flush(void);
private:
    char *_filename;
    std::vector<Table *> _tables;

    bool existsTable(Table *table);
    bool load(void);
    size_t headerPageSize(void);
    bool writeHeader(Writer *writer);
    bool readHeader(Reader *reader);
    bool canLoad(void);
};

}

#endif
