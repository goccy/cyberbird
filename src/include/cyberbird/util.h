#define CYBER_BIRD_MAX_ZOOM_LEVEL (32)

#define CYBER_BIRD_LOG_ERROR(...) do {                  \
        fprintf(stderr, "[%s:%d] ", __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__);                   \
        fprintf(stderr, "\n");                          \
    } while (0)

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
