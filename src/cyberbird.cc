#include <cyberbird/cyberbird.h>

using namespace cyberbird;

CyberBird::CyberBird(void)
{
}

CyberBird::~CyberBird(void)
{
}

void CyberBird::wake(const char *filename)
{
    this->_storage = new Storage(filename);
}

void CyberBird::sleep(void)
{
    CYBER_BIRD_SAFE_DELETE(this->_storage);
}

Storage *CyberBird::storage()
{
    return this->_storage;
}

array CyberBird::fly(const char *tableName, double latitude, double longitude, unsigned int zoomLevel)
{
    return this->_storage->table(tableName)->select(latitude, longitude, zoomLevel);
}
