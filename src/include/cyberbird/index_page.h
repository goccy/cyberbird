#ifndef __CYBERBIRD_INDEX_PAGE__
#define __CYBERBIRD_INDEX_PAGE__

#include <cyberbird/index_tree.h>
#include <cyberbird/reader.h>
#include <cyberbird/writer.h>

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
    bool load(Reader *reader);
    bool flush(Writer *writer);
    IndexTree *tree(void);

private:
    IndexPageHeader _header;
    IndexTree *_tree;
};

}

#endif /* __CYBERBIRD_INDEX_PAGE__ */
