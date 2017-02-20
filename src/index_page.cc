#include <cyberbird/index_page.h>
#include <cyberbird/util.h>

using namespace cyberbird;

IndexPage::IndexPage(void)
{
    this->_tree = new IndexTree();
}

IndexPage::~IndexPage(void)
{
    CYBER_BIRD_SAFE_DELETE(this->_tree);
}

IndexTree *IndexPage::tree(void)
{
    return this->_tree;
}

void IndexPage::load(void)
{
}

void IndexPage::save(void)
{
}
