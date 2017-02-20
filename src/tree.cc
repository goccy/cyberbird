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

std::vector<IndexLeaf *> IndexTree::select(uint64_t key, unsigned int zoomLevel)
{
    return select(key, zoomLevel, CYBER_BIRD_MAX_ZOOM_LEVEL);
}

std::vector<IndexLeaf *> IndexTree::select(uint64_t key, unsigned int zoomLevel, unsigned int maxZoomLevel)
{
    std::vector<IndexLeaf *> ret;
    if (zoomLevel > maxZoomLevel) return ret;

    uint64_t count = CYBER_BIRD_MAX_ZOOM_LEVEL << 1;
    IndexNode *currentNode = this->_rootNode;
    for (size_t i = CYBER_BIRD_MAX_ZOOM_LEVEL; i > 0; i--) {
        size_t currentZoomLevel = CYBER_BIRD_MAX_ZOOM_LEVEL - i + 1;
        count -= 2;
        IndexType type = (IndexType)((key & (uint64_t)((uint64_t)3 << count)) >> count);
        if (currentZoomLevel < zoomLevel) {
            switch (type) {
            case IndexTypeTopLeft:
                if (!currentNode->_topLeftNode) return ret;
                currentNode = currentNode->_topLeftNode;
                break;
            case IndexTypeTopRight:
                if (!currentNode->_topRightNode) return ret;
                currentNode = currentNode->_topRightNode;
                break;
            case IndexTypeBottomLeft:
                if (!currentNode->_bottomLeftNode) return ret;
                currentNode = currentNode->_bottomLeftNode;
                break;
            case IndexTypeBottomRight:
                if (!currentNode->_bottomRightNode) return ret;
                currentNode = currentNode->_bottomRightNode;
                break;
            default:
                break;
            }
        } else {
            ret = getAllLocation(currentNode, maxZoomLevel);
            break;
        }
    }
    return ret;
}

std::vector<IndexLeaf *> IndexTree::getAllLocation(IndexNode *node, unsigned int maxZoomLevel)
{
    std::vector<IndexLeaf *> ret;
    if (node->_zoomLevel > maxZoomLevel) return ret;

    size_t locationCount = node->_locations.size();
    for (size_t i = 0; i< locationCount; ++i) {
        ret.push_back(node->_locations[i]);
    }
    if (node->_topLeftNode) {
        std::vector<IndexLeaf *> locations = getAllLocation(node->_topLeftNode, maxZoomLevel);
        size_t locationCount = locations.size();
        for (size_t i = 0; i< locationCount; ++i) {
            ret.push_back(locations[i]);
        }
    }
    if (node->_topRightNode) {
        std::vector<IndexLeaf *> locations = getAllLocation(node->_topRightNode, maxZoomLevel);
        size_t locationCount = locations.size();
        for (size_t i = 0; i< locationCount; ++i) {
            ret.push_back(locations[i]);
        }
    }
    if (node->_bottomLeftNode) {
        std::vector<IndexLeaf *> locations = getAllLocation(node->_bottomLeftNode, maxZoomLevel);
        size_t locationCount = locations.size();
        for (size_t i = 0; i< locationCount; ++i) {
            ret.push_back(locations[i]);
        }
    }
    if (node->_bottomRightNode) {
        std::vector<IndexLeaf *> locations = getAllLocation(node->_bottomRightNode, maxZoomLevel);
        size_t locationCount = locations.size();
        for (size_t i = 0; i< locationCount; ++i) {
            ret.push_back(locations[i]);
        }
    }
    return ret;
}

void IndexTree::insert(uint64_t key, uint64_t offset, uint64_t size)
{
    insert(key, offset, size, 0);
}

void IndexTree::insert(uint64_t key, uint64_t offset, uint64_t size, unsigned int cacheZoomLevel)
{
    uint64_t count = CYBER_BIRD_MAX_ZOOM_LEVEL << 1;
    IndexNode *currentNode = this->_rootNode;
    currentNode->_totalChildren++;
    for (size_t i = CYBER_BIRD_MAX_ZOOM_LEVEL; i > 0; i--) {
        count -= 2;
        size_t currentZoomLevel = CYBER_BIRD_MAX_ZOOM_LEVEL - i + 1;
        IndexType type = (IndexType)((key & (uint64_t)((uint64_t)3 << count)) >> count);
        switch (type) {
        case IndexTypeTopLeft:
            if (!currentNode->_topLeftNode) {
                currentNode->_topLeftNode = new IndexNode(currentZoomLevel);
            }
            currentNode->_topLeftNode->_totalChildren++;
            currentNode = currentNode->_topLeftNode;
            break;
        case IndexTypeTopRight:
            if (!currentNode->_topRightNode) {
                currentNode->_topRightNode = new IndexNode(currentZoomLevel);
            }
            currentNode->_topRightNode->_totalChildren++;
            currentNode = currentNode->_topRightNode;
            break;
        case IndexTypeBottomLeft:
            if (!currentNode->_bottomLeftNode) {
                currentNode->_bottomLeftNode = new IndexNode(currentZoomLevel);
            }
            currentNode->_bottomLeftNode->_totalChildren++;
            currentNode = currentNode->_bottomLeftNode;
            break;
        case IndexTypeBottomRight:
            if (!currentNode->_bottomRightNode) {
                currentNode->_bottomRightNode = new IndexNode(currentZoomLevel);
            }
            currentNode->_bottomRightNode->_totalChildren++;
            currentNode = currentNode->_bottomRightNode;
            break;
        default:
            break;
        }
        if (0 < cacheZoomLevel && cacheZoomLevel < CYBER_BIRD_MAX_ZOOM_LEVEL) {
            // cache specific data for small zoom level
            currentNode->_locations.push_back(new IndexLeaf(key, offset, size));
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
