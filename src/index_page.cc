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

IndexPage::IndexPage(const char *filename)
{
    size_t len  = strlen(filename) + 1;
    this->_filename = (char *)malloc(len);
    strncpy(this->_filename, filename, len);
    this->_tree = new IndexTree();
}

IndexPage::~IndexPage(void)
{
    CYBER_BIRD_SAFE_DELETE(this->_tree);
    CYBER_BIRD_SAFE_FREE(this->_filename);
}

IndexTree *IndexPage::tree(void)
{
    return this->_tree;
}

void IndexPage::load(void)
{
    gzFile file = gzopen(this->_filename, "rb");
    if (!file) {
        CYBER_BIRD_LOG_ERROR("cannot open %s. %s", this->_filename, strerror(errno));
        return;
    }
    char headerBuf[sizeof(IndexPageHeader)] = {0};
    size_t headerSize = sizeof(IndexPageHeader);
    int readError = 0;
    if ((readError = gzread(file, headerBuf, headerSize)) < 0) {
        CYBER_BIRD_LOG_ERROR("cannot read %s. %s", this->_filename, gzerror(file, &readError));
        return;
    }
    IndexPageHeader *header = (IndexPageHeader *)headerBuf;
    EncodeBuffer nodePoolBuffer;
    nodePoolBuffer.size = header->nodePoolSize;
    nodePoolBuffer.data = (char *)malloc(header->nodePoolSize);
    if ((readError = gzread(file, nodePoolBuffer.data, header->nodePoolSize)) < 0) {
        CYBER_BIRD_LOG_ERROR("cannot read %s. %s", this->_filename, gzerror(file, &readError));
        return;
    }
    EncodeBuffer leafPoolBuffer;
    leafPoolBuffer.size = header->leafPoolSize;
    leafPoolBuffer.data = (char *)malloc(header->leafPoolSize);
    if ((readError = gzread(file, leafPoolBuffer.data, header->leafPoolSize)) < 0) {
        CYBER_BIRD_LOG_ERROR("cannot read %s. %s", this->_filename, gzerror(file, &readError));
        return;
    }
    gzclose(file);
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
    gzFile file = gzopen(this->_filename, "wb");
    if (!file) {
        CYBER_BIRD_LOG_ERROR("cannot create|open %s. %s", this->_filename, strerror(errno));
        return;
    }
    int writeError = 0;
    if ((writeError = gzwrite(file, (char *)&header, headerSize)) < 0) {
        CYBER_BIRD_LOG_ERROR("cannot write to %s. %s", this->_filename, gzerror(file, &writeError));
        return;
    }
    if ((writeError = gzwrite(file, nodePoolBuffer.data, nodePoolBuffer.size)) < 0) {
        CYBER_BIRD_LOG_ERROR("cannot write to %s. %s", this->_filename, gzerror(file, &writeError));
        return;
    }
    if ((writeError = gzwrite(file, leafPoolBuffer.data, leafPoolBuffer.size)) < 0) {
        CYBER_BIRD_LOG_ERROR("cannot write to %s %s", this->_filename, gzerror(file, &writeError));
        return;
    }
    gzclose(file);
}
