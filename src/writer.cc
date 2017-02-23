#include <cyberbird/writer.h>
#include <cyberbird/util.h>
#include <errno.h>
#include <string.h>


using namespace cyberbird;

Writer::Writer(const char *filename) : _filename(filename)
{
    this->_filePtr = gzopen(this->_filename, "wb");
    if (!this->_filePtr) {
        CYBER_BIRD_LOG_ERROR("cannot open %s. %s", this->_filename, strerror(errno));
    }
}

Writer::~Writer(void)
{
    gzclose(this->_filePtr);
}

bool Writer::write(char *buf, uint64_t size)
{
    int writeError = 0;
    if ((writeError = gzwrite(this->_filePtr, buf, size)) < size) {
        CYBER_BIRD_LOG_ERROR("cannot write to %s. %s", this->_filename, gzerror(this->_filePtr, &writeError));
        return false;
    }
    return true;
}
