#ifndef __CYBERBIRD_WRITER_H__
#define __CYBERBIRD_WRITER_H__

#include <zlib.h>

namespace cyberbird {

class Writer {
public:
    Writer(const char *filename);
    ~Writer(void);
    bool write(char *buf, uint64_t size);
private:
    const char *_filename;
    gzFile _filePtr;
};

};

#endif /* __CYBERBIRD_WRITER_H__ */
