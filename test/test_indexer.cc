#include <cyberbird/indexer.h>
#include <gtest/gtest.h>

using namespace cyberbird;

class IndexerTest : public ::testing::Test{};

TEST_F(IndexerTest, index) {
    double latitude  = 35.65796;
    double longitude = 139.708928;
    uint64_t index = Indexer::index(latitude, longitude);
    EXPECT_EQ(index, 8945753372360179111);
}

TEST_F(IndexerTest, toLocation) {
    double latitude  = 35.65796;
    double longitude = 139.708928;
    uint64_t index   = 8945753372360179111;
    double lat[1];
    double lon[1];
    Indexer::toLocation(index, lat, lon);
    EXPECT_EQ((int)latitude, (int)lat[0]);
    EXPECT_EQ((int)longitude, (int)lon[0]);
}
