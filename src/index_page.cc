#include <cyberbird/index_page.h>
#include <cyberbird/util.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <zlib.h>

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

bool IndexPage::load(Reader *reader)
{
    char headerBuf[sizeof(IndexPageHeader)] = {0};
    size_t headerSize = sizeof(IndexPageHeader);
    if (!reader->read(headerBuf, headerSize)) return false;

    IndexPageHeader *header = (IndexPageHeader *)headerBuf;
    EncodeBuffer nodePoolBuffer;
    nodePoolBuffer.size = header->nodePoolSize;
    nodePoolBuffer.data = (char *)malloc(header->nodePoolSize);

    if (!reader->read(nodePoolBuffer.data, header->nodePoolSize)) return false;

    EncodeBuffer leafPoolBuffer;
    leafPoolBuffer.size = header->leafPoolSize;
    leafPoolBuffer.data = (char *)malloc(header->leafPoolSize);

    if (!reader->read(leafPoolBuffer.data, header->leafPoolSize)) return false;

    CYBER_BIRD_SAFE_DELETE(this->_tree);
    this->_tree = new IndexTree(&nodePoolBuffer, &leafPoolBuffer);
    CYBER_BIRD_SAFE_FREE(nodePoolBuffer.data);
    CYBER_BIRD_SAFE_FREE(leafPoolBuffer.data);
    return true;
}

bool IndexPage::flush(Writer *writer)
{
    EncodeBuffer nodePoolBuffer = this->_tree->encodeNodePool();
    EncodeBuffer leafPoolBuffer = this->_tree->encodeLeafPool();
    IndexPageHeader header;
    size_t headerSize = sizeof(IndexPageHeader);
    header.nodePoolPosition = headerSize;
    header.nodePoolSize     = nodePoolBuffer.size;
    header.leafPoolPosition = headerSize + nodePoolBuffer.size;
    header.leafPoolSize     = leafPoolBuffer.size;

    if (!writer->write((char *)&header, headerSize)) return false;
    if (!writer->write(nodePoolBuffer.data, nodePoolBuffer.size)) return false;
    if (!writer->write(leafPoolBuffer.data, leafPoolBuffer.size)) return false;
    return true;
}
