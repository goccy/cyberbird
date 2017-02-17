#ifndef __CYBERBIRD_H__
#define __CYBERBIRD_H__

#include "data.h"
#include "storage.h"

namespace cyberbird {

class CyberBird {
public:
    CyberBird(void);
    virtual ~CyberBird(void);
    void wake(const char *filename);
    void sleep(void);
    array fly(const char *tableName, double latitude, double longitude, unsigned int zoomLevel);
    Storage *storage(void);

private:
    Storage *_storage;
};

}

#endif
