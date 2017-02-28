#include <cyberbird/storage.h>
#include <cyberbird/util.h>
#include <cyberbird/indexer.h>
#include <stdint.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <zlib.h>

using namespace cyberbird;

Table::Column::Column(const char *name, Table::Column::Type type, size_t size) : _type(type), _size(size)
{
    size_t len  = strlen(name) + 1;
    this->_name = (char *)malloc(len);
    strncpy(this->_name, name, len);
}

Table::Column::Column(const Column &v) : _type(v._type), _size(v._size)
{
    size_t len  = strlen(v._name) + 1;
    this->_name = (char *)malloc(len);
    strncpy(this->_name, v._name, len);
}

Table::Column::~Column(void)
{
    CYBER_BIRD_SAFE_FREE(this->_name);
}

Table::Builder::Builder(const char *tableName) : _tableName(tableName)
{
    Table::Column column("id", Table::Column::Number, sizeof(uint64_t));
    this->_columns.push_back(column);
}

Table::Builder::~Builder()
{
}

Table::Builder *Table::Builder::addNumberColumn(const char *name)
{
    Table::Column column(name, Table::Column::Number, sizeof(double));
    this->_columns.push_back(column);
    return this;
}

Table::Builder *Table::Builder::addStringColumn(const char *name, size_t fixedSize)
{
    Column column(name, Column::String, fixedSize);
    this->_columns.push_back(column);
    return this;
}

Table *Table::Builder::build(void)
{
    return new Table(this->_tableName, this->_columns);
}

Table::Table(const char *tableName, std::vector<Column> columns) :
    _columns(columns),
    _lastId(0)
{
    size_t len  = strlen(tableName) + 1;
    this->_name = (char *)malloc(len);
    strncpy(this->_name, tableName, len);
    this->_indexPage = new IndexPage();
    pthread_mutex_init(&this->_lock, 0);
}

Table::~Table(void)
{
    CYBER_BIRD_SAFE_FREE(this->_name);
    CYBER_BIRD_SAFE_DELETE(this->_indexPage);
}

const char *Table::name(void)
{
    return const_cast<const char *>(this->_name);
}

const std::vector<Table::Column> &Table::columns(void)
{
    return this->_columns;
}

array Table::select(double latitude, double longitude, unsigned int zoomLevel, unsigned int maxZoomLevel)
{
    pthread_mutex_lock(&this->_lock);
    array ret;
    std::vector<IndexLeaf *> indexes = this->_indexPage->tree()->select(Indexer::index(latitude, longitude), zoomLevel, maxZoomLevel);
    for (size_t i = 0; i < indexes.size(); ++i) {
        object *o = this->_rows[indexes[i]->id];
        CYBER_BIRD_ASSERT(o, "object is NULL");
        double targetLatitude[1]  = {0};
        double targetLongitude[1] = {0};
        Indexer::toLocation(indexes[i]->key, targetLatitude, targetLongitude);
        o->insert(std::make_pair("id", value(indexes[i]->key)));
        o->insert(std::make_pair("latitude", value(targetLatitude[0])));
        o->insert(std::make_pair("longitude", value(targetLongitude[0])));
        ret.push_back(value(*o));
    }
    pthread_mutex_unlock(&this->_lock);
    return ret;
}

array Table::select(double latitude, double longitude, unsigned int zoomLevel)
{
    return select(latitude, longitude, zoomLevel, CYBER_BIRD_MAX_ZOOM_LEVEL);
}

uint64_t Table::insert(double latitude, double longitude, const object &o)
{
    return insert(latitude, longitude, 0, o);
}

uint64_t Table::insert(double latitude, double longitude, unsigned int zoomLevel, const object &o)
{
    if (!isValidData(o)) return 0;
    if (!this->_storage) {
        CYBER_BIRD_LOG_ERROR("must be created this table before insert. show Storage::createTable");
        return 0;
    }
    pthread_mutex_lock(&this->_lock);
    uint64_t dataId     = this->_lastId + 1;
    this->_rows[dataId] = copyObject(o);
    this->_indexPage->tree()->insert(Indexer::index(latitude, longitude), dataId, zoomLevel);
    this->_lastId = dataId;
    pthread_mutex_unlock(&this->_lock);
    return dataId;
}

void Table::update(double latitude, double longitude, const object &o)
{
    if (!isValidData(o)) return;

    pthread_mutex_lock(&this->_lock);
    array ret;
    std::vector<IndexLeaf *> indexes = this->_indexPage->tree()->select(Indexer::index(latitude, longitude), 0);
    if (indexes.size() > 1) {
        CYBER_BIRD_LOG_WARN("found duplicate entry by [latitude, longitude] = [%f, %f]", latitude, longitude);
        return;
    }
    IndexLeaf *leaf = indexes[0];
    object *origin  = this->_rows[leaf->id];
    CYBER_BIRD_ASSERT(origin, "object is NULL");
    this->_rows[leaf->id] = copyObject(o);
    CYBER_BIRD_SAFE_DELETE(origin);
    pthread_mutex_unlock(&this->_lock);
}

void Table::remove(double latitude, double longitude)
{
    pthread_mutex_lock(&this->_lock);
    pthread_mutex_unlock(&this->_lock);
}

object *Table::copyObject(const object &from)
{
    object *to = new object();
    size_t columnCount = this->_columns.size();
    for (size_t i = 1; i < columnCount; ++i) {
        std::string columnName = std::string(this->_columns[i].name());
        to->insert(std::make_pair(columnName, value(((object)from)[columnName])));
    }
    return to;
}

bool Table::isValidData(const object &object)
{
    size_t columnCount = this->_columns.size();
    // columns[0] is 'id' column
    for (size_t i = 1; i < columnCount; ++i) {
        if (object.find(std::string(this->_columns[i].name())) == object.end()) {
            CYBER_BIRD_LOG_ERROR("cannot find '%s' from object.", this->_columns[i].name());
            return false;
        }
    }
    return true;
}

size_t Table::rowSize(void)
{
    size_t ret = 0;
    size_t columnCount = this->_columns.size();
    for (size_t i = 0; i < columnCount; ++i) {
        ret += this->_columns[i].size();
    }
    return ret;
}

bool Table::load(Reader *reader)
{
    this->_indexPage->load(reader);

    size_t rowCount = 0;
    if (!reader->readNumber(&rowCount)) return false;
    size_t dataSize = rowSize();
    size_t columnCount = this->_columns.size();
    for (size_t i = 0; i < rowCount; ++i) {
        uint64_t id = 0;
        if (!reader->readNumber(&id)) return false;
        object *o   = new object();
        for (size_t i = 1; i < columnCount; ++i) {
            const Table::Column &column = this->_columns[i];
            const char *name = ((Table::Column)column).name();
            size_t size      = ((Table::Column)column).size();
            switch (((Table::Column)column).type()) {
            case Column::Number: {
                double number = 0;
                if (!reader->readNumber(&number)) return false;
                o->insert(std::make_pair(std::string(name), value(number)));
                break;
            }
            case Column::String: {
                char *buf = (char *)malloc(size + 1);
                memset(buf, 0, size + 1);
                if (!reader->read(buf, size)) return false;
                std::string str = std::string(buf);
                o->insert(std::make_pair(std::string(name), value(str)));
                CYBER_BIRD_SAFE_FREE(buf);
                break;
            }
            default:
                break;
            }
        }
        this->_rows[id] = o;
    }
    return true;
}

bool Table::flush(Writer *writer)
{
    if (!this->_indexPage->flush(writer)) return false;

    std::map<uint64_t, object*>::iterator it = this->_rows.begin();
    size_t dataSize = rowSize();
    char *rowData  = (char *)malloc(dataSize);
    size_t columnCount = this->_columns.size();
    int writeError = 0;
    size_t rowCount = this->_rows.size();
    if (!writer->write((char *)&rowCount, sizeof(size_t))) return false;

    for (; it != this->_rows.end(); ++it) {
        uint64_t id = it->first;
        object &o = *it->second;
        memset(rowData, 0, dataSize);
        char *writePtr = rowData;
        memcpy(writePtr, &id, this->_columns[0].size());
        writePtr += this->_columns[0].size();
        for (size_t i = 1; i < columnCount; ++i) {
            const Table::Column &column = this->_columns[i];
            value v = o[std::string(((Table::Column)column).name())];
            size_t size = ((Table::Column)column).size();
            switch (((Table::Column)column).type()) {
            case Column::Number: {
                double number = v.get<double>();
                memcpy(writePtr, &number, size);
                writePtr += size;
                break;
            }
            case Column::String: {
                std::string str = v.get<std::string>();
                memcpy(writePtr, str.c_str(), size);
                writePtr += size;
                break;
            }
            default:
                break;
            }
        }

        if (!writer->write(rowData, dataSize)) return false;
    }
    return true;
}

Storage::Storage(const char *filename)
{
    size_t len      = strlen(filename) + 1;
    this->_filename = (char *)malloc(len);
    strncpy(this->_filename, filename, len);
    if (canLoad()) load();
}

Storage::~Storage(void)
{
}

bool Storage::canLoad(void)
{
    FILE *fp = fopen(this->_filename, "rb");
    if (!fp) return false;
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fclose(fp);
    return size > 0;
}

bool Storage::readHeader(Reader *reader)
{
    size_t headerSize = 0;
    if (!reader->readNumber(&headerSize)) return false;

    size_t readBytes = 0;
    while (readBytes < headerSize) {
        TableHeader tableHeader;
        if (!reader->read((char *)&tableHeader, sizeof(TableHeader))) return false;

        char *tableName = (char *)malloc(tableHeader.tableNameSize + 1);
        memset(tableName, 0, tableHeader.tableNameSize + 1);
        if (!reader->read(tableName, tableHeader.tableNameSize)) return false;

        readBytes += sizeof(TableHeader) + tableHeader.tableNameSize;

        std::vector<Table::Column> columns;
        for (size_t i = 0; i < tableHeader.columnCount; ++i) {
            ColumnHeader columnHeader;
            if (!reader->read((char *)&columnHeader, sizeof(ColumnHeader))) return false;

            char *columnName = (char *)malloc(columnHeader.nameLength + 1);
            memset(columnName, 0, columnHeader.nameLength + 1);
            if (!reader->read(columnName, columnHeader.nameLength)) return false;

            Table::Column column(columnName, (Table::Column::Type)columnHeader.type, columnHeader.size);
            columns.push_back(column);
            CYBER_BIRD_SAFE_FREE(columnName);

            readBytes += sizeof(ColumnHeader) + columnHeader.nameLength;
        }
        Table *table    = new Table(tableName, columns);
        table->_storage = this;
        table->_lastId  = tableHeader.lastId;
        this->_tables.push_back(table);
        CYBER_BIRD_SAFE_FREE(tableName);
    }
    return true;
}

bool Storage::load(void)
{
    Reader reader(this->_filename);
    if (!readHeader(&reader)) return false;
    size_t tableCount = this->_tables.size();
    for (size_t i = 0; i < tableCount; ++i) {
        if (!this->_tables[i]->load(&reader)) return false;
    }
    return true;
}

Table *Storage::table(const char *tableName)
{
    size_t tableCount = this->_tables.size();
    for (size_t i = 0; i < tableCount; ++i) {
        Table *table = this->_tables[i];
        if (std::string(table->name()) == tableName) return table;
    }
    return NULL;
}

bool Storage::existsTable(Table *targetTable)
{
    size_t tableCount = this->_tables.size();
    std::string tableName = std::string(targetTable->name());
    for (size_t i = 0; i < tableCount; ++i) {
        Table *table = this->_tables[i];
        if (std::string(table->name()) == tableName) return true;
    }
    return false;
}

bool Storage::createTable(Table *table)
{
    if (existsTable(table)) {
        CYBER_BIRD_LOG_WARN("%s is already exists.", table->name());
        return false;
    }
    table->_storage = this;
    this->_tables.push_back(table);
    return flush();
}

bool Storage::flush(void)
{
    Writer writer(this->_filename);
    if (!writeHeader(&writer)) return false;
    size_t tableCount = this->_tables.size();
    for (size_t i = 0; i < tableCount; ++i) {
        if (!this->_tables[i]->flush(&writer)) return false;
    }
    return true;
}

size_t Storage::headerPageSize(void)
{
    size_t pageSize = 0;
    size_t tableCount = this->_tables.size();
    for (size_t i = 0; i < tableCount; ++i) {
        Table *table = this->_tables[i];
        pageSize += sizeof(TableHeader) + strlen(table->name());
        const std::vector<Table::Column> &columns = table->columns();
        for (size_t j = 0; j < columns.size(); ++j) {
            pageSize += sizeof(ColumnHeader) + strlen(((Table::Column)columns[j]).name());
        }
    }
    return pageSize;
}

bool Storage::writeHeader(Writer *writer)
{
    size_t pageSize = headerPageSize();
    if (!writer->write((char *)&pageSize, sizeof(size_t))) return false;

    size_t tableCount = this->_tables.size();
    for (size_t i = 0; i < tableCount; ++i) {
        Table *table = this->_tables[i];
        const char *name = table->name();
        const std::vector<Table::Column> &columns = table->columns();
        TableHeader tableHeader;
        tableHeader.tableNameSize = strlen(name);
        tableHeader.columnCount   = columns.size();

        if (!writer->write((char *)&tableHeader, sizeof(TableHeader))) return false;
        if (!writer->write((char *)name, tableHeader.tableNameSize)) return false;

        for (size_t j = 0; j < columns.size(); ++j) {
            ColumnHeader columnHeader;
            const Table::Column &column = columns[j];
            const char *columnName = ((Table::Column)column).name();
            columnHeader.nameLength = strlen(columnName);
            columnHeader.type       = ((Table::Column)column).type();
            columnHeader.size       = ((Table::Column)column).size();

            if (!writer->write((char *)&columnHeader, sizeof(ColumnHeader))) return false;
            if (!writer->write((char *)columnName, columnHeader.nameLength)) return false;
        }
    }
    return true;
}
