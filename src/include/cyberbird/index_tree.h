#include <stdint.h>
#include <vector>

namespace cyberbird {

typedef enum {
    IndexTypeTopLeft,
    IndexTypeTopRight,
    IndexTypeBottomLeft,
    IndexTypeBottomRight
} IndexType;

class IndexLeaf {
public:
    uint64_t _key;
    uint64_t _offset;
    uint64_t _size;

    IndexLeaf(uint64_t key, uint64_t offset, uint64_t size);
    ~IndexLeaf(void);
};

class IndexNode {
public:
    unsigned int _zoomLevel;
    IndexNode *_topLeftNode;
    IndexNode *_topRightNode;
    IndexNode *_bottomLeftNode;
    IndexNode *_bottomRightNode;
    std::vector<IndexLeaf *> _locations;
    uint64_t _totalChildren;

    IndexNode(void);
    IndexNode(unsigned int zoomLevel);
    ~IndexNode(void);
};

class IndexTree {
public:
    IndexTree(void);
    ~IndexTree(void);
    std::vector<IndexLeaf *> select(uint64_t key, unsigned int zoomLevel);
    std::vector<IndexLeaf *> select(uint64_t key, unsigned int zoomLevel, unsigned int maxZoomLevel);
    void insert(uint64_t key, uint64_t offest, uint64_t size);
    void insert(uint64_t key, uint64_t offest, uint64_t size, unsigned int cacheZoomLevel);
    void update(uint64_t key, uint64_t offest, uint64_t size);
    void remove(uint64_t key);
    IndexNode *rootNode(void);

private:
    IndexNode *_rootNode;
    std::vector<IndexLeaf *> getAllLocation(IndexNode *node, unsigned int maxZoomLevel);
};

}
