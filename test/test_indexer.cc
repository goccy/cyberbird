#include <cyberbird/indexer.h>
#include "fixture/fixture.h"
#include <gtest/gtest.h>

using namespace cyberbird;

class IndexerTest : public ::testing::Test, public Fixture {};

TEST_F(IndexerTest, index) {
    uint64_t index = Indexer::index(SIBUYA_1ST_TOWER_LAT, SIBUYA_1ST_TOWER_LON);
    EXPECT_EQ(index, SIBUYA_1ST_TOWER_INDEX);
}

TEST_F(IndexerTest, toLocation) {
    double lat[1];
    double lon[1];
    Indexer::toLocation(SIBUYA_1ST_TOWER_INDEX, lat, lon);
    EXPECT_EQ(toString(SIBUYA_1ST_TOWER_LAT), toString(lat[0]));
    EXPECT_EQ(toString(SIBUYA_1ST_TOWER_LON), toString(lon[0]));
}
