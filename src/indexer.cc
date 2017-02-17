#include <cyberbird/indexer.h>
#include <math.h>
#include <stdio.h>

using namespace cyberbird;

static const uint64_t MAX_DETAIL = 32;

uint64_t Indexer::index(double latitude, double longitude)
{
    double slatitude = sin(latitude * (M_PI / 180));
    double x = (longitude + 180) / 360;
    double y = 0.5 - log((1 + slatitude) / (1 - slatitude)) / (M_PI * 4);
    uint64_t mapSize = (uint64_t)256 << MAX_DETAIL;
    uint64_t xx = ((uint64_t)(x * (double)mapSize + 0.5)) >> 8;
    uint64_t yy = ((uint64_t)(y * (double)mapSize + 0.5)) >> 8;
    uint64_t count = MAX_DETAIL << 1;
    uint64_t ret   = 0;
    for (int i = MAX_DETAIL; i > 0; i--) {
        uint64_t n = i - 1;
        int mask   = 1 << n;
        count -= 2;
        ret |= (((xx & mask) >> n) + (((yy & mask) >> n) << 1)) << count;
    }
    return ret;
}

void Indexer::toLocation(uint64_t index, double *latitude, double *longitude)
{
    uint64_t xx = 0;
    uint64_t yy = 0;
    uint64_t count = MAX_DETAIL << 1;
    for (uint64_t i = MAX_DETAIL; i > 0; i--) {
        count -= 2;
        uint64_t mask   = (uint64_t)1 << (i - 1);
        int number = int((index & (uint64_t)((uint64_t)3 << count)) >> count);
        switch (number) {
        case 1:
            xx |= mask;
            break;
        case 2:
            yy |= mask;
            break;
        case 3:
            xx |= mask;
            yy |= mask;
            break;
        }
    }
    uint64_t mapSize = (uint64_t)256 << MAX_DETAIL;
    double x = ((double)(xx << 8) / mapSize) - 0.5;
    double y = 0.5 - ((double)(yy << 8) / mapSize);
    *latitude  = 90 - 360 * atan(exp(-y * 2 * M_PI)) / M_PI;
    *longitude = 360 * x;
}
