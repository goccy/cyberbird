#include <cyberbird/index_tree.h>
#include <cyberbird/util.h>
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
    EXPECT_EQ(searchNode->_totalChildren, 1);
    for (size_t i = 0; i < CYBER_BIRD_MAX_ZOOM_LEVEL; ++i) {
        unsigned int hasChildNodeCount = 0;
        IndexNode *nextDepthNode = NULL;
        if (searchNode->_topLeftNode) {
            nextDepthNode = searchNode->_topLeftNode;
            hasChildNodeCount++;
        }
        if (searchNode->_topRightNode) {
            nextDepthNode = searchNode->_topRightNode;
            hasChildNodeCount++;
        }
        if (searchNode->_bottomLeftNode) {
            nextDepthNode = searchNode->_bottomLeftNode;
            hasChildNodeCount++;
        }
        if (searchNode->_bottomRightNode) {
            nextDepthNode = searchNode->_bottomRightNode;
            hasChildNodeCount++;
        }
        EXPECT_EQ(hasChildNodeCount, 1);
        searchNode = nextDepthNode;
        if (i == CYBER_BIRD_MAX_ZOOM_LEVEL - 1) {
            EXPECT_EQ(searchNode->_locations.size(), 1);
        } else {
            EXPECT_EQ(searchNode->_locations.size(), 0);
        }
    }
}
