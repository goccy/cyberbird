#include <cyberbird/index_page.h>
#include <cyberbird/util.h>
#include <cyberbird/indexer.h>
#include <gtest/gtest.h>

using namespace cyberbird;

class IndexPageTest : public ::testing::Test{};

TEST_F(IndexPageTest, save_load) {
    IndexPage page("index.db");
    IndexTree *tree = page.tree();

    double latitude  = 35.65796;
    double longitude = 139.708928;
    uint64_t index   = 8945753372360179111;

    double sibuyaStationLatitude  = 35.65795;
    double sibuyaStationLongitude = 139.701547;
    uint64_t stationIndex = Indexer::index(sibuyaStationLatitude, sibuyaStationLongitude);

    tree->insert(index, 0, 0);
    tree->insert(stationIndex, 0, 0);

    page.save();
    page.load();

    IndexTree *loadedTree = page.tree();
    EXPECT_EQ(loadedTree->select(index, 14).size(), 1);
    EXPECT_EQ(loadedTree->select(index, 13).size(), 2);

    EXPECT_EQ(loadedTree->select(index, 10).size(), 2);
    EXPECT_EQ(loadedTree->select(index, 10, 13).size(), 0);
}
