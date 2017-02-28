#include <stdint.h>
#include <string>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>

class Fixture {
public:
    double SIBUYA_1ST_TOWER_LAT;
    double SIBUYA_1ST_TOWER_LON;
    uint64_t SIBUYA_1ST_TOWER_INDEX;

    double SIBUYA_STATION_LAT;
    double SIBUYA_STATION_LON;
    uint64_t SIBUYA_STATION_INDEX;

    Fixture() {
        SIBUYA_1ST_TOWER_LAT   = 35.65796;
        SIBUYA_1ST_TOWER_LON   = 139.708928;
        SIBUYA_1ST_TOWER_INDEX = 8945753372360179111;
        SIBUYA_STATION_LAT     = 35.65795;
        SIBUYA_STATION_LON     = 139.701547;
        SIBUYA_STATION_INDEX   = 8945753184703206720;
    }

    std::string toString(double value) {
        return std::to_string(value);
    }

    char *dbpath() {
        static char path[PATH_MAX] = {0};
        std::string pathTemplate = "/tmp/storage.dbXXXXXX";
        int fd = mkstemp((char *)pathTemplate.c_str());
        if (fcntl(fd, F_GETPATH, path) == -1) {
            return NULL;
        }
        return path;
    }
};
