//
// Created by casheeeewnuts on 6/17/23.
//

#include <stdlib.h>
#include <stdio.h>
#include "http_response.h"

char *build_response_header(const HttpResponse *response) {
    char *header = malloc(sizeof(char) * 65535);

    sprintf(header, "HTTP/1.0 %3d OK\nContent-Type: %s\nContent-Length: %d\n\n", response->status, response->contentType, response->contentLength);

    return header;
}