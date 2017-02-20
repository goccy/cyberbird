#include <cyberbird/index_tree.h>
#include <cyberbird/util.h>

using namespace cyberbird;

IndexLeaf::IndexLeaf(uint64_t key, uint64_t offset, uint64_t size) :
    _key(key),
    _offset(offset),
    _size(size)
{
}

IndexLeaf::~IndexLeaf(void)
{
}

IndexNode::IndexNode(void) :
    _topLeftNode(NULL),
    _topRightNode(NULL),
    _bottomLeftNode(NULL),
    _bottomRightNode(NULL),
    _zoomLevel(0),
    _totalChildren(0)
{
}

IndexNode::IndexNode(unsigned int zoomLevel) :
    _topLeftNode(NULL),
    _topRightNode(NULL),
    _bottomLeftNode(NULL),
    _bottomRightNode(NULL),
    _zoomLevel(zoomLevel),
    _totalChildren(0)
{
}

IndexNode::~IndexNode(void)
{
    CYBER_BIRD_SAFE_DELETE(this->_topLeftNode);
    CYBER_BIRD_SAFE_DELETE(this->_topRightNode);
    CYBER_BIRD_SAFE_DELETE(this->_bottomLeftNode);
    CYBER_BIRD_SAFE_DELETE(this->_bottomRightNode);
    size_t locationCount = this->_locations.size();
    for (size_t i = 0; i < locationCount; ++i) {
        delete this->_locations[i];
    }
    this->_locations.clear();
}

IndexTree::IndexTree(void)
{
    this->_rootNode = new IndexNode();
}

IndexTree::~IndexTree(void)
{
    CYBER_BIRD_SAFE_DELETE(this->_rootNode);
}

std::vector<IndexLeaf *> IndexTree::select(uint64_t key)
{
    std::vector<IndexLeaf *> ret;
    return ret;
}

std::vector<IndexLeaf *> IndexTree::select(uint64_t key, unsigned int zoomLevel)
{
    std::vector<IndexLeaf *> ret;
    return ret;
}

void IndexTree::insert(uint64_t key, uint64_t offset, uint64_t size)
{
    uint64_t count = CYBER_BIRD_MAX_ZOOM_LEVEL << 1;
    IndexNode *currentNode = this->_rootNode;
    currentNode->_totalChildren++;
    for (uint64_t i = CYBER_BIRD_MAX_ZOOM_LEVEL; i > 0; i--) {
        count -= 2;
        IndexType type = (IndexType)((key & (uint64_t)((uint64_t)3 << count)) >> count);
        switch (type) {
        case IndexTypeTopLeft:
            if (!currentNode->_topLeftNode) {
                currentNode->_topLeftNode = new IndexNode(CYBER_BIRD_MAX_ZOOM_LEVEL - i + 1);
            }
            currentNode->_topLeftNode->_totalChildren++;
            currentNode = currentNode->_topLeftNode;
            break;
        case IndexTypeTopRight:
            if (!currentNode->_topRightNode) {
                currentNode->_topRightNode = new IndexNode(CYBER_BIRD_MAX_ZOOM_LEVEL - i + 1);
            }
            currentNode->_topRightNode->_totalChildren++;
            currentNode = currentNode->_topRightNode;
            break;
        case IndexTypeBottomLeft:
            if (!currentNode->_bottomLeftNode) {
                currentNode->_bottomLeftNode = new IndexNode(CYBER_BIRD_MAX_ZOOM_LEVEL - i + 1);
            }
            currentNode->_bottomLeftNode->_totalChildren++;
            currentNode = currentNode->_bottomLeftNode;
            break;
        case IndexTypeBottomRight:
            if (!currentNode->_bottomRightNode) {
                currentNode->_bottomRightNode = new IndexNode(CYBER_BIRD_MAX_ZOOM_LEVEL - i + 1);
            }
            currentNode->_bottomRightNode->_totalChildren++;
            currentNode = currentNode->_bottomRightNode;
            break;
        default:
            break;
        }
        if (i == 1) {
            currentNode->_locations.push_back(new IndexLeaf(key, offset, size));
        }
    }
}

void IndexTree::update(uint64_t key, uint64_t offset, uint64_t size)
{
    
}

void IndexTree::remove(uint64_t key)
{
    
}

IndexNode *IndexTree::rootNode(void)
{
    return this->_rootNode;
}
