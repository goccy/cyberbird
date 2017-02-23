#ifndef __CYBERBIRD_H__
#define __CYBERBIRD_H__

#include <cyberbird/data.h>
#include <cyberbird/index_tree.h>
#include <cyberbird/index_page.h>
#include <cyberbird/indexer.h>
#include <cyberbird/reader.h>
#include <cyberbird/storage.h>
#include <cyberbird/util.h>
#include <cyberbird/writer.h>

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
