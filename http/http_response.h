//
// Created by casheeeewnuts on 6/17/23.
//

#ifndef PICO_HTTP_SERVER_HTTP_RESPONSE_H
#define PICO_HTTP_SERVER_HTTP_RESPONSE_H

typedef struct HttpResponse {
    unsigned short status;
    unsigned int contentLength;
    char *contentType;
    char *body;
} HttpResponse;

char *build_response_header(const HttpResponse *response);

#endif //PICO_HTTP_SERVER_HTTP_RESPONSE_H
