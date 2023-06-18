//
// Created by casheeeewnuts on 6/17/23.
//

#ifndef PICO_HTTP_SERVER_HTTP_SERVER_H
#define PICO_HTTP_SERVER_HTTP_SERVER_H

#include <glib-2.0/glib.h>
#include "http_request.h"
#include "http_response.h"

struct ServerOption {
    unsigned short port;
    char *host;
    char *rootDir;
};

typedef struct HttpServer {
    struct ServerOption serverOption;
    int socket;
    GHashTable *mimeType;
    void (*listen)(struct HttpServer*);
} HttpServer;

HttpServer new_http_server(struct ServerOption option);

typedef void (*RequestHandler) (HttpRequest*, HttpResponse*);

#endif //PICO_HTTP_SERVER_HTTP_SERVER_H