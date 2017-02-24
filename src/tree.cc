#include <cyberbird/index_tree.h>
#include <cyberbird/util.h>
#include <stdlib.h>
#include <map>

using namespace cyberbird;

static const int DEFAULT_LOCATION_COUNT = 8;

IndexTree::IndexTree(void)
{
    static const int DEFAULT_NODE_POOL_CAPACITY = 8192;
    static const int DEFAULT_LEAF_POOL_CAPACITY = 8192;

    this->_currentNodePoolCapacity = DEFAULT_NODE_POOL_CAPACITY;
    this->_currentLeafPoolCapacity = DEFAULT_LEAF_POOL_CAPACITY;
    this->_nodePool = (IndexNodePool *)calloc(this->_currentNodePoolCapacity, sizeof(IndexNode));
    this->_leafPool = (IndexLeafPool *)calloc(this->_currentLeafPoolCapacity, sizeof(IndexLeaf));
    this->_currentNodeCount = 0;
    this->_currentLeafCount = 0;
    this->_rootNode = newNode();
}

#define SET_PTR_FROM_INDEX(pool, node, position) (node->position.node = node->position.index > 0 ? pool + node->position.index : NULL)

IndexTree::IndexTree(EncodeBuffer *nodePoolBuffer, EncodeBuffer *leafPoolBuffer)
{
    static const int EXTEND_BUFFER_SIZE = 1024;

    this->_currentNodePoolCapacity = nodePoolBuffer->size + EXTEND_BUFFER_SIZE;
    this->_currentLeafPoolCapacity = leafPoolBuffer->size + EXTEND_BUFFER_SIZE;
    this->_nodePool = (IndexNodePool *)calloc(this->_currentNodePoolCapacity, sizeof(IndexNode));
    this->_leafPool = (IndexLeafPool *)calloc(this->_currentLeafPoolCapacity, sizeof(IndexLeaf));
    memcpy(this->_nodePool, nodePoolBuffer->data, nodePoolBuffer->size);
    memcpy(this->_leafPool, leafPoolBuffer->data, leafPoolBuffer->size);
    this->_currentNodeCount = nodePoolBuffer->size / sizeof(IndexNode);
    this->_currentLeafCount = leafPoolBuffer->size / sizeof(IndexLeaf);
    for (size_t i = 0; i < this->_currentNodeCount; ++i) {
        IndexNode *node = this->_nodePool + i;
        SET_PTR_FROM_INDEX(this->_nodePool, node, topLeft);
        SET_PTR_FROM_INDEX(this->_nodePool, node, topRight);
        SET_PTR_FROM_INDEX(this->_nodePool, node, bottomLeft);
        SET_PTR_FROM_INDEX(this->_nodePool, node, bottomRight);
    }
    std::map<uint64_t, size_t> nodeLocationMap;
    for (size_t i = 0; i < this->_currentLeafCount; ++i) {
        IndexLeaf *leaf = this->_leafPool + i;
        IndexNode *node = this->_nodePool + leaf->node.index;
        leaf->node.ptr  = node;
        node->locations = (IndexLeaf **)calloc(node->locationCapacity, sizeof(IndexLeaf *));
        size_t locationCount = nodeLocationMap[leaf->node.index];
        node->locations[locationCount] = leaf;
        nodeLocationMap[leaf->node.index] = locationCount + 1;
    }
    this->_rootNode = (IndexNode *)this->_nodePool;
}

IndexTree::~IndexTree(void)
{
    for (size_t i = 0; i < this->_currentNodeCount; ++i) {
        IndexNode *node = this->_nodePool + i;
        CYBER_BIRD_SAFE_FREE(node->locations);
    }
    CYBER_BIRD_SAFE_FREE(this->_nodePool);
    CYBER_BIRD_SAFE_FREE(this->_leafPool);
}

IndexNode *IndexTree::newNode(void)
{
    return newNode(0);
}

IndexNode *IndexTree::newNode(unsigned int zoomLevel)
{
    if (this->_currentNodeCount + 1 == this->_currentNodePoolCapacity) {
        // expand nodePool size
        size_t expandCapacity = this->_currentNodePoolCapacity * 2;
        IndexNodePool *expandedNodePool = (IndexNodePool *)calloc(expandCapacity, sizeof(IndexNode));
        memset(expandedNodePool, 0, expandCapacity);
        memcpy(expandedNodePool, this->_nodePool, this->_currentNodePoolCapacity);
        CYBER_BIRD_ASSERT(expandedNodePool, "cannot expand index node pool");
        CYBER_BIRD_SAFE_FREE(this->_nodePool);
        this->_currentNodePoolCapacity = expandCapacity;
        this->_nodePool = expandedNodePool;
    }
    IndexNode *node = this->_nodePool + this->_currentNodeCount;
    node->zoomLevel = zoomLevel;
    this->_currentNodeCount++;
    return node;
}

IndexLeaf *IndexTree::newLeaf(uint64_t key, uint64_t id, IndexNode *node)
{
    if (this->_currentLeafCount + 1 == this->_currentLeafPoolCapacity) {
        // expand leafPool size
        this->_currentLeafPoolCapacity *= 2;
        this->_leafPool = (IndexLeafPool *)realloc(this->_leafPool, this->_currentLeafPoolCapacity);
    }
    IndexLeaf *leaf = (IndexLeaf *)this->_leafPool + this->_currentLeafCount;
    leaf->key       = key;
    leaf->id        = id;
    leaf->node.ptr  = node;
    this->_currentLeafCount++;
    return leaf;
}

#define NODE_PTR_TO_INDEX(node, position) (node->position.node ? (node->position.node - this->_nodePool) : 0)

EncodeBuffer IndexTree::encodeNodePool(void)
{
    EncodeBuffer ret;
    size_t size         = this->_currentNodeCount * sizeof(IndexNode);
    IndexNodePool *data = (IndexNodePool *)calloc(this->_currentNodeCount, sizeof(IndexNode));
    memcpy(data, this->_nodePool, size);
    for (size_t i = 0; i < this->_currentNodeCount; ++i) {
        IndexNode *node    = this->_nodePool + i;
        IndexNode *retNode = data + i;
        retNode->topLeft.index     = NODE_PTR_TO_INDEX(node, topLeft);
        retNode->topRight.index    = NODE_PTR_TO_INDEX(node, topRight);
        retNode->bottomLeft.index  = NODE_PTR_TO_INDEX(node, bottomLeft);
        retNode->bottomRight.index = NODE_PTR_TO_INDEX(node, bottomRight);
    }
    ret.data = (char *)data;
    ret.size = size;
    return ret;
}

EncodeBuffer IndexTree::encodeLeafPool(void)
{
    EncodeBuffer ret;
    IndexLeafPool *data = (IndexLeafPool *)calloc(this->_currentLeafCount, sizeof(IndexLeaf));
    size_t size         = this->_currentLeafCount * sizeof(IndexLeaf);
    memcpy(data, this->_leafPool, size);
    for (size_t i = 0; i < this->_currentLeafCount; ++i) {
        IndexLeaf *leaf     = this->_leafPool + i;
        IndexLeaf *retLeaf  = data + i;
        retLeaf->node.index = leaf->node.ptr - this->_nodePool;
    }
    ret.data = (char *)data;
    ret.size = size;
    return ret;
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
                if (!currentNode->topLeft.node) return ret;
                currentNode = currentNode->topLeft.node;
                break;
            case IndexTypeTopRight:
                if (!currentNode->topRight.node) return ret;
                currentNode = currentNode->topRight.node;
                break;
            case IndexTypeBottomLeft:
                if (!currentNode->bottomLeft.node) return ret;
                currentNode = currentNode->bottomLeft.node;
                break;
            case IndexTypeBottomRight:
                if (!currentNode->bottomRight.node) return ret;
                currentNode = currentNode->bottomRight.node;
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
    if (node->zoomLevel > maxZoomLevel) return ret;

    size_t locationCount = node->locationCount;
    for (size_t i = 0; i < locationCount; ++i) {
        CYBER_BIRD_ASSERT(node->locations[i], "node->location is NULL");
        ret.push_back(node->locations[i]);
    }
    if (node->topLeft.node) {
        std::vector<IndexLeaf *> locations = getAllLocation(node->topLeft.node, maxZoomLevel);
        size_t locationCount = locations.size();
        for (size_t i = 0; i < locationCount; ++i) {
            CYBER_BIRD_ASSERT(locations[i], "location is NULL");
            ret.push_back(locations[i]);
        }
    }
    if (node->topRight.node) {
        std::vector<IndexLeaf *> locations = getAllLocation(node->topRight.node, maxZoomLevel);
        size_t locationCount = locations.size();
        for (size_t i = 0; i< locationCount; ++i) {
            CYBER_BIRD_ASSERT(locations[i], "location is NULL");
            ret.push_back(locations[i]);
        }
    }
    if (node->bottomLeft.node) {
        std::vector<IndexLeaf *> locations = getAllLocation(node->bottomLeft.node, maxZoomLevel);
        size_t locationCount = locations.size();
        for (size_t i = 0; i< locationCount; ++i) {
            CYBER_BIRD_ASSERT(locations[i], "location is NULL");
            ret.push_back(locations[i]);
        }
    }
    if (node->bottomRight.node) {
        std::vector<IndexLeaf *> locations = getAllLocation(node->bottomRight.node, maxZoomLevel);
        size_t locationCount = locations.size();
        for (size_t i = 0; i< locationCount; ++i) {
            CYBER_BIRD_ASSERT(locations[i], "location is NULL");
            ret.push_back(locations[i]);
        }
    }
    return ret;
}

void IndexTree::insert(uint64_t key, uint64_t id)
{
    insert(key, id, 0);
}

void IndexTree::insert(uint64_t key, uint64_t id, unsigned int cacheZoomLevel)
{
    uint64_t count = CYBER_BIRD_MAX_ZOOM_LEVEL << 1;
    IndexNode *currentNode = this->_rootNode;
    currentNode->totalChildren++;
    for (size_t i = CYBER_BIRD_MAX_ZOOM_LEVEL; i > 0; i--) {
        count -= 2;
        size_t currentZoomLevel = CYBER_BIRD_MAX_ZOOM_LEVEL - i + 1;
        IndexType type = (IndexType)((key & (uint64_t)((uint64_t)3 << count)) >> count);
        switch (type) {
        case IndexTypeTopLeft:
            if (!currentNode->topLeft.node) {
                currentNode->topLeft.node = newNode(currentZoomLevel);
            }
            currentNode->topLeft.node->totalChildren++;
            currentNode = currentNode->topLeft.node;
            break;
        case IndexTypeTopRight:
            if (!currentNode->topRight.node) {
                currentNode->topRight.node = newNode(currentZoomLevel);
            }
            currentNode->topRight.node->totalChildren++;
            currentNode = currentNode->topRight.node;
            break;
        case IndexTypeBottomLeft:
            if (!currentNode->bottomLeft.node) {
                currentNode->bottomLeft.node = newNode(currentZoomLevel);
            }
            currentNode->bottomLeft.node->totalChildren++;
            currentNode = currentNode->bottomLeft.node;
            break;
        case IndexTypeBottomRight:
            if (!currentNode->bottomRight.node) {
                currentNode->bottomRight.node = newNode(currentZoomLevel);
            }
            currentNode->bottomRight.node->totalChildren++;
            currentNode = currentNode->bottomRight.node;
            break;
        default:
            break;
        }
        if ((0 < cacheZoomLevel && cacheZoomLevel < CYBER_BIRD_MAX_ZOOM_LEVEL) ||
            i == 1) {
            // cache specific data for small zoom level
            if (!currentNode->locations) {
                allocNodeLocations(currentNode);
            } else if (currentNode->locationCount == currentNode->locationCapacity) {
                expandNodeLocations(currentNode);
            }
            currentNode->locations[currentNode->locationCount] = newLeaf(key, id, currentNode);
            currentNode->locationCount++;
        }
    }
}

void IndexTree::allocNodeLocations(IndexNode *node)
{
    node->locationCapacity = DEFAULT_LOCATION_COUNT;
    node->locations = (IndexLeaf **)calloc(node->locationCapacity, sizeof(IndexLeaf *));
}

void IndexTree::expandNodeLocations(IndexNode *node)
{
    node->locationCapacity *= 2;
    node->locations = (IndexLeaf **)realloc(node->locations, node->locationCapacity);
}

void IndexTree::remove(uint64_t key)
{
    
}

IndexNode *IndexTree::rootNode(void)
{
    return this->_rootNode;
}
