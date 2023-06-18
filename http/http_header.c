//
// Created by casheeeewnuts on 6/18/23.
//

#include <glib-2.0/glib.h>
#include "http_header.h"

void add_header(Header *headers, char *key, char *value);

void del_header(Header *headers, char *key);

char *lookup(Header *headers, char *key);

void destroy_header(Header *headers);

Header *new_header() {
    Header *header = calloc(1, sizeof(Header));

    header->table = g_hash_table_new(g_str_hash, g_str_equal);
    header->add = add_header;
    header->remove = del_header;
    header->destroy = destroy_header;
    header->get = lookup;

    return header;
}

void add_header(Header *headers, char *key, char *value) {
    g_hash_table_insert(headers->table, g_strdup(key), g_strdup(value));
}

void del_header(Header *headers, char *key) {
    g_hash_table_remove(headers->table, key);
}

void destroy_header(Header *headers) {
    g_hash_table_destroy(headers->table);
    free(headers);
}

char *lookup(Header *header, char *key) {
    return g_hash_table_lookup(header->table, key);
}
