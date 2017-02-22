#include <stdint.h>
#include <vector>

namespace cyberbird {

typedef enum {
    IndexTypeTopLeft,
    IndexTypeTopRight,
    IndexTypeBottomLeft,
    IndexTypeBottomRight
} IndexType;

struct _IndexNode;

typedef struct {
    uint64_t key;
    uint64_t id;
    union {
        struct _IndexNode *ptr;
        uint64_t index;
    } node;
} IndexLeaf;

typedef struct _IndexNode {
    unsigned int zoomLevel;
    uint64_t totalChildren;
    union {
        struct _IndexNode *node;
        uint64_t index;
    } topLeft;
    union {
        struct _IndexNode *node;
        uint64_t index;
    } topRight;
    union {
        struct _IndexNode *node;
        uint64_t index;
    } bottomLeft;
    union {
        struct _IndexNode *node;
        uint64_t index;
    } bottomRight;
    IndexLeaf **locations;
    uint32_t locationCount;
    uint64_t locationCapacity;
} IndexNode;


typedef IndexNode IndexNodePool;
typedef IndexLeaf IndexLeafPool;

typedef struct {
    char *data;
    uint64_t size;
} EncodeBuffer;

class IndexTree {
public:
    IndexTree(void);
    IndexTree(EncodeBuffer *nodePoolBuffer, EncodeBuffer *leafPoolBuffer);
    ~IndexTree(void);
    std::vector<IndexLeaf *> select(uint64_t key, unsigned int zoomLevel);
    std::vector<IndexLeaf *> select(uint64_t key, unsigned int zoomLevel, unsigned int maxZoomLevel);
    void insert(uint64_t key, uint64_t id);
    void insert(uint64_t key, uint64_t id, unsigned int cacheZoomLevel);
    void remove(uint64_t key);
    IndexNode *rootNode(void);
    EncodeBuffer encodeNodePool(void);
    EncodeBuffer encodeLeafPool(void);

private:
    IndexNode *_rootNode;
    IndexNodePool *_nodePool;
    IndexLeafPool *_leafPool;
    uint64_t _currentNodeCount;
    uint64_t _currentLeafCount;
    uint64_t _currentNodePoolCapacity;
    uint64_t _currentLeafPoolCapacity;

    std::vector<IndexLeaf *> getAllLocation(IndexNode *node, unsigned int maxZoomLevel);
    IndexNode *newNode();
    IndexNode *newNode(unsigned int zoomLevel);
    IndexLeaf *newLeaf(uint64_t key, uint64_t id, IndexNode *node);
    void allocNodeLocations(IndexNode *node);
    void expandNodeLocations(IndexNode *node);
};

}
