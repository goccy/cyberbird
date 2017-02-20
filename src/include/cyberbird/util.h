#define CYBER_BIRD_MAX_ZOOM_LEVEL (32)

#define CYBER_BIRD_SAFE_DELETE(ptr) do {        \
        if (ptr != NULL) {                      \
            delete ptr;                         \
            ptr = NULL;                         \
        }                                       \
    } while(0)
