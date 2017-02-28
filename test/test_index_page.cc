#include <cyberbird/index_page.h>
#include <cyberbird/util.h>
#include <gtest/gtest.h>
#include "fixture/fixture.h"

using namespace cyberbird;

class IndexPageTest : public ::testing::Test, public Fixture {};

TEST_F(IndexPageTest, save_load) {
    IndexPage page;
    IndexTree *tree = page.tree();
    tree->insert(SIBUYA_1ST_TOWER_INDEX, 0, 0);
    tree->insert(SIBUYA_STATION_INDEX, 0, 0);

    {
        Writer writer("index.db");
        page.flush(&writer);
    }
    {
        Reader reader("index.db");
        page.load(&reader);
    }
    IndexTree *loadedTree = page.tree();
    EXPECT_EQ(loadedTree->select(SIBUYA_1ST_TOWER_INDEX, 14).size(), 1);
    EXPECT_EQ(loadedTree->select(SIBUYA_1ST_TOWER_INDEX, 13).size(), 2);

    EXPECT_EQ(loadedTree->select(SIBUYA_1ST_TOWER_INDEX, 10).size(), 2);
    EXPECT_EQ(loadedTree->select(SIBUYA_1ST_TOWER_INDEX, 10, 13).size(), 0);

    unlink("index.db");
}
