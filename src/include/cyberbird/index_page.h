#include <cyberbird/index_tree.h>

namespace cyberbird {

typedef struct {
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
};

}
