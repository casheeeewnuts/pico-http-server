//
// Created by casheeeewnuts on 6/16/23.
//

#ifndef PICO_HTTP_SERVER_HTTP_REQUEST_H
#define PICO_HTTP_SERVER_HTTP_REQUEST_H

#include <netinet/in.h>

typedef enum HttpMethod {
    Get
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
    char *body;
} HttpRequest;

HttpRequest *parse_http_request(const char *rawRequest);
void dispose_request(HttpRequest *request);
#endif //PICO_HTTP_SERVER_HTTP_REQUEST_H
