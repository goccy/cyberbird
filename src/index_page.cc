#include <cyberbird/index_page.h>
#include <cyberbird/util.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

using namespace cyberbird;

IndexPage::IndexPage(void) : _filename("index.db")
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
    int fd = open(this->_filename, O_RDONLY);
    if (fd < 0) {
        CYBER_BIRD_LOG_ERROR("cannot open %s. %s", this->_filename, strerror(errno));
        return;
    }
    char headerBuf[sizeof(IndexPageHeader)] = {0};
    size_t headerSize = sizeof(IndexPageHeader);
    if (read(fd, headerBuf, headerSize) < 0) {
        CYBER_BIRD_LOG_ERROR("cannot read %s. %s", this->_filename, strerror(errno));
        return;
    }
    IndexPageHeader *header = (IndexPageHeader *)headerBuf;
    EncodeBuffer nodePoolBuffer;
    nodePoolBuffer.size = header->nodePoolSize;
    nodePoolBuffer.data = (char *)malloc(header->nodePoolSize);
    if (read(fd, nodePoolBuffer.data, header->nodePoolSize) < 0) {
        CYBER_BIRD_LOG_ERROR("cannot read %s. %s", this->_filename, strerror(errno));
        return;
    }
    EncodeBuffer leafPoolBuffer;
    leafPoolBuffer.size = header->leafPoolSize;
    leafPoolBuffer.data = (char *)malloc(header->leafPoolSize);
    if (read(fd, leafPoolBuffer.data, header->leafPoolSize) < 0) {
        CYBER_BIRD_LOG_ERROR("cannot read %s. %s", this->_filename, strerror(errno));
        return;
    }
    CYBER_BIRD_SAFE_DELETE(this->_tree);
    this->_tree = new IndexTree(&nodePoolBuffer, &leafPoolBuffer);
    CYBER_BIRD_SAFE_FREE(nodePoolBuffer.data);
    CYBER_BIRD_SAFE_FREE(leafPoolBuffer.data);
}

void IndexPage::save(void)
{
    EncodeBuffer nodePoolBuffer = this->_tree->encodeNodePool();
    EncodeBuffer leafPoolBuffer = this->_tree->encodeLeafPool();
    IndexPageHeader header;
    size_t headerSize = sizeof(IndexPageHeader);
    header.nodePoolPosition = headerSize;
    header.nodePoolSize     = nodePoolBuffer.size;
    header.leafPoolPosition = headerSize + nodePoolBuffer.size;
    header.leafPoolSize     = leafPoolBuffer.size;
    int fd = open(this->_filename, O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
    if (fd < 0) {
        CYBER_BIRD_LOG_ERROR("cannot create|open %s. %s", this->_filename, strerror(errno));
        return;
    }
    if (write(fd, (char *)&header, headerSize) < 0) {
        CYBER_BIRD_LOG_ERROR("cannot write to %s. %s", this->_filename, strerror(errno));
        return;
    }
    if (write(fd, nodePoolBuffer.data, nodePoolBuffer.size) < 0) {
        CYBER_BIRD_LOG_ERROR("cannot write to %s. %s", this->_filename, strerror(errno));
        return;
    }
    if (write(fd, leafPoolBuffer.data, leafPoolBuffer.size) < 0) {
        CYBER_BIRD_LOG_ERROR("cannot write to %s %s", this->_filename, strerror(errno));
        return;
    }
    close(fd);
}
