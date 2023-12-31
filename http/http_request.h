//
// Created by casheeeewnuts on 6/16/23.
//

#ifndef PICO_HTTP_SERVER_HTTP_REQUEST_H
#define PICO_HTTP_SERVER_HTTP_REQUEST_H

#include <netinet/in.h>
#include "http_header.h"

typedef enum HttpMethod {
    Head,
    Get,
    Post
} HttpMethod;

typedef enum HttpVersion {
    V1_0,
} HttpVersion;

typedef struct HttpRequest {
    int socket;
    struct sockaddr_in remoteAddr;
    HttpMethod method;
    HttpVersion version;
    char *path;
    Header *header;
    unsigned long bodySize;
    char *body;
} HttpRequest;

HttpRequest *accept_http_request(int sock, struct sockaddr_in remoteAddr);
void dispose_request(HttpRequest *request);
#endif //PICO_HTTP_SERVER_HTTP_REQUEST_H
