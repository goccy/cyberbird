#include <cyberbird/index_tree.h>

namespace cyberbird {

typedef struct {
    uint64_t nodePoolPosition;
    uint64_t nodePoolSize;
    uint64_t leafPoolPosition;
    uint64_t leafPoolSize;
} IndexPageHeader;

class IndexPage {
public:
    IndexPage(void);
    ~IndexPage(void);
    void load(void);
    void save(void);
    IndexTree *tree(void);

private:
    IndexPageHeader _header;
    IndexTree *_tree;
    const char *_filename;
};

}
