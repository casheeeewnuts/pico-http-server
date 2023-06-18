#ifndef PICO_HTTP_SERVER_HTTP_RESPONSE_H
#define PICO_HTTP_SERVER_HTTP_RESPONSE_H

#include <glib-2.0/glib.h>

typedef struct Header {
    GHashTable *table;

    void (*add)(struct Header *, char *key, char *value);

    void (*remove)(struct Header *, char *key);
} Header;

//typedef GHashTable Cookie;

typedef struct HttpResponse {
    unsigned short status;
    unsigned int contentLength;
    char *contentType;
    Header *headers;
//    Cookie *cookie;
    char *body;
} HttpResponse;

typedef enum RedirectType {
    Temporary,
    Permanent
} RedirectType;


HttpResponse *new_response();
void dispose_response(HttpResponse *response);
char *build_response_header(const HttpResponse *response);
HttpResponse *redirect(HttpResponse *res, RedirectType type, char *location);
HttpResponse *not_found(HttpResponse *res);
HttpResponse *method_not_allowed(HttpResponse *res);
HttpResponse *forbidden(HttpResponse *res);
HttpResponse *internal_server_error(HttpResponse *res);

#endif //PICO_HTTP_SERVER_HTTP_RESPONSE_H
