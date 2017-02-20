#ifndef __CYBERBIRD_INDEXER_H__
#define __CYBERBIRD_INDEXER_H__

#include <stdint.h>

namespace cyberbird {

class Indexer {
public:
    static uint64_t index(double latitude, double longitude);
    static void toLocation(uint64_t index, double *latitude, double *longitude);
private:
    static uint64_t index(double latitude, double longitude, unsigned int zoomLevel);
};

}

#endif
