#include <cyberbird/index_tree.h>
#include <cyberbird/util.h>
#include <gtest/gtest.h>
#include "fixture/fixture.h"

using namespace cyberbird;

class IndexTreeTest : public ::testing::Test, public Fixture{};

TEST_F(IndexTreeTest, insert) {
    IndexTree tree;
    tree.insert(SIBUYA_1ST_TOWER_INDEX, 0, 0);
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
    IndexTree tree;
    tree.insert(SIBUYA_1ST_TOWER_INDEX, 0, 0);
    tree.insert(SIBUYA_STATION_INDEX, 0, 0);

    EXPECT_EQ(tree.select(SIBUYA_1ST_TOWER_INDEX, 14).size(), 1);
    EXPECT_EQ(tree.select(SIBUYA_1ST_TOWER_INDEX, 13).size(), 2);

    EXPECT_EQ(tree.select(SIBUYA_1ST_TOWER_INDEX, 10).size(), 2);
    EXPECT_EQ(tree.select(SIBUYA_1ST_TOWER_INDEX, 10, 13).size(), 0);
}

TEST_F(IndexTreeTest, encode_decode) {
    IndexTree tree;
    tree.insert(SIBUYA_1ST_TOWER_INDEX, 0, 0);
    tree.insert(SIBUYA_STATION_INDEX, 0, 0);

    EncodeBuffer nodePoolBuffer = tree.encodeNodePool();
    EncodeBuffer leafPoolBuffer = tree.encodeLeafPool();
    IndexTree newTree(&nodePoolBuffer, &leafPoolBuffer);
    EXPECT_EQ(newTree.select(SIBUYA_1ST_TOWER_INDEX, 14).size(), 1);
    EXPECT_EQ(newTree.select(SIBUYA_1ST_TOWER_INDEX, 13).size(), 2);

    EXPECT_EQ(newTree.select(SIBUYA_1ST_TOWER_INDEX, 10).size(), 2);
    EXPECT_EQ(newTree.select(SIBUYA_1ST_TOWER_INDEX, 10, 13).size(), 0);
}
