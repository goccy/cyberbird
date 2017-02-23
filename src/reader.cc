#include <cyberbird/reader.h>
#include <cyberbird/util.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>

using namespace cyberbird;

Reader::Reader(const char *filename) : _filename(filename)
{
    this->_filePtr = gzopen(this->_filename, "rb");
    if (!this->_filePtr) {
        CYBER_BIRD_LOG_ERROR("cannot open %s. %s", this->_filename, strerror(errno));
    }
}

Reader::~Reader(void)
{
    gzclose(this->_filePtr);
}

bool Reader::read(char *buf, uint64_t size)
{
    int readError = 0;
    memset(buf, 0, size);
    if (gzread(this->_filePtr, buf, size) < size) {
        CYBER_BIRD_LOG_ERROR("cannot read %s. %s", this->_filename, gzerror(this->_filePtr, &readError));
        return false;
    }
    return true;
}

bool Reader::readNumber(size_t *number)
{
    int readError = 0;
    size_t size = sizeof(size_t);
    if (gzread(this->_filePtr, number, size) < size) {
        CYBER_BIRD_LOG_ERROR("cannot read %s. %s", this->_filename, gzerror(this->_filePtr, &readError));
        return false;
    }
    return true;
}

bool Reader::readNumber(uint64_t *number)
{
    int readError = 0;
    size_t size = sizeof(uint64_t);
    if (gzread(this->_filePtr, number, size) < size) {
        CYBER_BIRD_LOG_ERROR("cannot read %s. %s", this->_filename, gzerror(this->_filePtr, &readError));
        return false;
    }
    return true;
}

bool Reader::readNumber(long long int *number)
{
    int readError = 0;
    size_t size = sizeof(long long int);
    if (gzread(this->_filePtr, number, size) < size) {
        CYBER_BIRD_LOG_ERROR("cannot read %s. %s", this->_filename, gzerror(this->_filePtr, &readError));
        return false;
    }
    return true;
}

bool Reader::isEOF(void)
{
    return gzeof(this->_filePtr);
}
