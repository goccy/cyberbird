#include <cyberbird/storage.h>
#include <cyberbird/util.h>
#include <stdint.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
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
}

Table::Builder::~Builder()
{
}

Table::Builder *Table::Builder::addNumberColumn(const char *name)
{
    Table::Column column(name, Table::Column::Number, sizeof(long long int));
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

Table::Table(const char *tableName)
{
    size_t len  = strlen(tableName) + 1;
    this->_name = (char *)malloc(len);
    strncpy(this->_name, tableName, len);
}

Table::Table(const char *tableName, std::vector<Column> columns) :
    _columns(columns)
{
    size_t len  = strlen(tableName) + 1;
    this->_name = (char *)malloc(len);
    strncpy(this->_name, tableName, len);
}

Table::~Table(void)
{
    CYBER_BIRD_SAFE_FREE(this->_name);
}

const char *Table::name(void)
{
    return const_cast<const char *>(this->_name);
}

const std::vector<Table::Column> &Table::columns(void)
{
    return this->_columns;
}

array Table::select(double latitude, double longitude, unsigned int zoomLevel)
{
    array a;
    return a;
}

array Table::select(double latitude, double longitude)
{
    array a;
    return a;
}

void Table::insert(double latitude, double longitude, unsigned int zoomLevel, const value &value)
{
}

void Table::update(double latitude, double longitude, unsigned int zoomLevel, const value &value)
{
}

void Table::remove(double latitude, double longitude)
{
}

Storage::Storage(const char *filename)
{
    size_t len      = strlen(filename) + 1;
    this->_filename = (char *)malloc(len);
    strncpy(this->_filename, filename, len);
    loadTables();
}

Storage::~Storage(void)
{
}

void Storage::loadTables(void)
{
    gzFile file = gzopen(this->_filename, "rb");
    if (!file) {
        CYBER_BIRD_LOG_ERROR("cannot open storage file");
        return;
    }

    while (!gzeof(file)) {
        TableHeader tableHeader;
        if (gzread(file, &tableHeader, sizeof(TableHeader)) < sizeof(TableHeader)) {
            return;
        }

        char *tableName = (char *)malloc(tableHeader.tableNameSize + 1);
        memset(tableName, 0, tableHeader.tableNameSize + 1);
        if (gzread(file, tableName, tableHeader.tableNameSize) < tableHeader.tableNameSize) {
            CYBER_BIRD_LOG_ERROR("cannot read table name");
            return;
        }
        CYBER_BIRD_LOG_INFO("load TableHeader. table name = [%s]", tableName);

        std::vector<Table::Column> columns;
        for (size_t i = 0; i < tableHeader.columnCount; ++i) {
            ColumnHeader columnHeader;
            if (gzread(file, &columnHeader, sizeof(ColumnHeader)) < sizeof(ColumnHeader)) {
                CYBER_BIRD_LOG_ERROR("cannot read ColumnHeader");
                return;
            }

            char *columnName = (char *)malloc(columnHeader.nameLength + 1);
            memset(columnName, 0, columnHeader.nameLength + 1);
            if (gzread(file, columnName, columnHeader.nameLength) < columnHeader.nameLength) {
                CYBER_BIRD_LOG_ERROR("cannot read column name");
                return;
            }

            Table::Column column(columnName, (Table::Column::Type)columnHeader.type, columnHeader.size);
            columns.push_back(column);
            CYBER_BIRD_SAFE_FREE(columnName);
        }
        this->_tables.push_back(new Table(tableName, columns));
        CYBER_BIRD_SAFE_FREE(tableName);
    }
    gzclose(file);
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
    this->_tables.push_back(table);
    return flush();
}

bool Storage::flush(void)
{
    gzFile file = gzopen(this->_filename, "wb");
    if (!file) {
        CYBER_BIRD_LOG_ERROR("cannot open %s. %s", this->_filename, strerror(errno));
        return false;
    }

    size_t tableCount = this->_tables.size();
    for (size_t i = 0; i < tableCount; ++i) {
        Table *table = this->_tables[i];
        const char *name = table->name();
        const std::vector<Table::Column> &columns = table->columns();
        TableHeader tableHeader;
        tableHeader.tableNameSize = strlen(name);
        tableHeader.columnCount   = columns.size();
        int writeError = 0;

        if ((writeError = gzwrite(file, (char *)&tableHeader, sizeof(TableHeader))) < 0) {
            CYBER_BIRD_LOG_ERROR("cannot write to %s. %s", this->_filename, gzerror(file, &writeError));
            return false;
        }

        if ((writeError = gzwrite(file, name, tableHeader.tableNameSize)) < 0) {
            CYBER_BIRD_LOG_ERROR("cannot write to %s. %s", this->_filename, gzerror(file, &writeError));
            return false;
        }

        for (size_t i = 0; i < columns.size(); ++i) {
            ColumnHeader columnHeader;
            const Table::Column &column = columns[i];
            const char *columnName = ((Table::Column)column).name();
            columnHeader.nameLength = strlen(columnName);
            columnHeader.type       = ((Table::Column)column).type();
            columnHeader.size       = ((Table::Column)column).size();

            if ((writeError = gzwrite(file, (char *)&columnHeader, sizeof(ColumnHeader))) < 0) {
                CYBER_BIRD_LOG_ERROR("cannot write to %s. %s", this->_filename, gzerror(file, &writeError));
                return false;
            }

            if ((writeError = gzwrite(file, columnName, columnHeader.nameLength)) < 0) {
                CYBER_BIRD_LOG_ERROR("cannot write to %s. %s", this->_filename, gzerror(file, &writeError));
                return false;
            }
        }
    }
    gzclose(file);
    return true;
}
