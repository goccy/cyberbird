#include <cyberbird/index_tree.h>
#include <cyberbird/util.h>
#include <cyberbird/indexer.h>
#include <gtest/gtest.h>

using namespace cyberbird;

class IndexTreeTest : public ::testing::Test{};

TEST_F(IndexTreeTest, insert) {
    double latitude  = 35.65796;
    double longitude = 139.708928;
    uint64_t index   = 8945753372360179111;
    IndexTree tree;
    tree.insert(index, 0, 0);
    IndexNode *searchNode = tree.rootNode();
    EXPECT_EQ(searchNode->totalChildren, 1);
    for (size_t i = 0; i < CYBER_BIRD_MAX_ZOOM_LEVEL; ++i) {
        unsigned int hasChildNodeCount = 0;
        IndexNode *nextDepthNode = NULL;
        if (searchNode->topLeft.node) {
            nextDepthNode = searchNode->topLeft.node;
            hasChildNodeCount++;
        }
        if (searchNode->topRight.node) {
            nextDepthNode = searchNode->topRight.node;
            hasChildNodeCount++;
        }
        if (searchNode->bottomLeft.node) {
            nextDepthNode = searchNode->bottomLeft.node;
            hasChildNodeCount++;
        }
        if (searchNode->bottomRight.node) {
            nextDepthNode = searchNode->bottomRight.node;
            hasChildNodeCount++;
        }
        EXPECT_EQ(hasChildNodeCount, 1);
        searchNode = nextDepthNode;
        if (i == CYBER_BIRD_MAX_ZOOM_LEVEL - 1) {
            EXPECT_EQ(searchNode->locationCount, 1);
        } else {
            EXPECT_EQ(searchNode->locationCount, 0);
        }
    }
}

TEST_F(IndexTreeTest, select) {
    double latitude  = 35.65796;
    double longitude = 139.708928;
    uint64_t index   = 8945753372360179111;

    double sibuyaStationLatitude  = 35.65795;
    double sibuyaStationLongitude = 139.701547;
    uint64_t stationIndex = Indexer::index(sibuyaStationLatitude, sibuyaStationLongitude);

    IndexTree tree;
    tree.insert(index, 0, 0);
    tree.insert(stationIndex, 0, 0);

    EXPECT_EQ(tree.select(index, 14).size(), 1);
    EXPECT_EQ(tree.select(index, 13).size(), 2);

    EXPECT_EQ(tree.select(index, 10).size(), 2);
    EXPECT_EQ(tree.select(index, 10, 13).size(), 0);
    
}
