//
// Created by casheeeewnuts on 6/18/23.
//

#ifndef PICO_HTTP_SERVER_HTTP_HEADER_H
#define PICO_HTTP_SERVER_HTTP_HEADER_H

#include <glib-2.0/glib.h>

typedef struct Header {
    GHashTable *table;

    void (*add)(struct Header *, char *key, char *value);

    void (*remove)(struct Header *, char *key);

    char *(* get)(struct Header *, char *key);
    void (*destroy)(struct Header *);
} Header;

Header *new_header();
#endif //PICO_HTTP_SERVER_HTTP_HEADER_H
