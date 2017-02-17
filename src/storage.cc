#include <cyberbird/storage.h>

using namespace cyberbird;

Table::Table(const char *tableName) : _tableName(tableName)
{
}

Table::~Table(void)
{
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
}

Storage::~Storage(void)
{
}

Table *Storage::table(const char *tableName)
{
    return new Table(tableName);
}
