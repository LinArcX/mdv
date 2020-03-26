#ifndef UTIL_H
#define UTIL_H

#define ABUF_INIT \
    {             \
        NULL, 0   \
    }
struct abuf {
    char* b;
    int len;
};

#endif
