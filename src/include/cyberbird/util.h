#ifndef __CYBERBIRD_UTIL_H__
#define __CYBERBIRD_UTIL_H__

#include <assert.h>

#define CYBER_BIRD_MAX_ZOOM_LEVEL (32)

#define CYBER_BIRD_LOG_DEVELOP(LOG_TYPE, ...) do {                      \
        fprintf(stderr, "["#LOG_TYPE"][%s:%d] ", __FILE__, __LINE__);   \
        fprintf(stderr, __VA_ARGS__);                                   \
        fprintf(stderr, "\n");                                          \
    } while (0)

#define CYBER_BIRD_LOG_RELEASE(LOG_TYPE, ...) do {  \
        fprintf(stderr, "["#LOG_TYPE"] ");          \
        fprintf(stderr, __VA_ARGS__);               \
        fprintf(stderr, "\n");                      \
    } while (0)

#if DEBUG

#define CYBER_BIRD_LOG_DEBUG(...) CYBER_BIRD_LOG_DEVELOP(DEBUG, __VA_ARGS__)

#else

#define CYBER_BIRD_LOG_DEBUG(...)

#endif /* DEBUG */

#define CYBER_BIRD_LOG_INFO(...) CYBER_BIRD_LOG_RELEASE(INFO, __VA_ARGS__)
#define CYBER_BIRD_LOG_WARN(...) CYBER_BIRD_LOG_RELEASE(WARN, __VA_ARGS__)
#define CYBER_BIRD_LOG_ERROR(...) CYBER_BIRD_LOG_RELEASE(ERROR, __VA_ARGS__)

#define CYBER_BIRD_SAFE_DELETE(ptr) do {        \
        if (ptr != NULL) {                      \
            delete ptr;                         \
            ptr = NULL;                         \
        }                                       \
    } while(0)

#define CYBER_BIRD_SAFE_FREE(ptr) do {          \
        if (ptr != NULL) {                      \
            free(ptr);                          \
            ptr = NULL;                         \
        }                                       \
    } while (0)

#define CYBER_BIRD_ASSERT(cond, msg) do {       \
        if (!cond) {                            \
            CYBER_BIRD_LOG_ERROR(msg);          \
        }                                       \
        assert(cond);                           \
    } while (0)

#endif /* __CYBERBIRD_UTIL_H__ */
