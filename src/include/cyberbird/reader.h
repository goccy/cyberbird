#ifndef __CYBERBIRD_READER_H__
#define __CYBERBIRD_READER_H__

#include <zlib.h>

namespace cyberbird {

class Reader {
public:
    Reader(const char *filename);
    ~Reader(void);
    bool read(char *buf, uint64_t size);
    bool readNumber(size_t *number);
    bool readNumber(uint64_t *number);
    bool readNumber(long long int *number);
    bool isEOF(void);
private:
    gzFile _filePtr;
    const char *_filename;
};

};

#endif /* __CYBERBIRD_READER_H__ */
