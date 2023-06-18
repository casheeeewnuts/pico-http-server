//
// Created by casheeeewnuts on 6/17/23.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "http_response.h"

char *status_message(unsigned short status);

char *build_response_header(const HttpResponse *response) {
    char *header = malloc(sizeof(char) * 65535);

    sprintf(header, "HTTP/1.0 %3d %s\n", response->status, status_message(response->status));
    sprintf(&header[strlen(header)], "Content-Length: %d\n", response->contentLength);

    if (response->contentType != NULL) {
        sprintf(&header[strlen(header)], "Content-Type: %s\n", response->contentType);
    }

    header[strlen(header)] = '\n';
    return header;
}

char *status_message(unsigned short status) {
    switch (status) {
        case 200:
            return "OK";
        case 201:
            return "Created";
        case 307:
            return "Temporary Redirect";
        case 308:
            return "Permanent Redirect";
        case 400:
            return "Bad Request";
        case 401:
            return "Unauthorized";
        case 403:
            return "Forbidden";
        case 404:
            return "Not Found";
        case 405:
            return "Method Not Allowed";
        case 500:
            return "Internal Server Error";
        default:
            return "";
    }
}

HttpResponse *redirect(HttpResponse *res, RedirectType type, char *location) {
    switch (type) {
        case Temporary:
            res->status = 307;
            break;
        default:
            res->status = 308;
            break;
    }

    res->contentLength = 0;
    res->contentType = NULL;
    res->body = NULL;

    return res;
}

HttpResponse *not_found(HttpResponse *res) {
    res->body = "404 Not Found";
    res->status = 404;
    res->contentType = "text/plain; charset=utf-8";
    res->contentLength = strlen(res->body);

    return res;
}

HttpResponse *method_not_allowed(HttpResponse *res) {
    res->body = "405 Method Not Allowed";
    res->status = 405;
    res->contentType = "text/plain; charset=utf-8";
    res->contentLength = strlen(res->body);

    return res;
}

HttpResponse *forbidden(HttpResponse *res) {
    res->body = "403 Forbidden";
    res->status = 403;
    res->contentType = "text/plain; charset=utf-8";
    res->contentLength = strlen(res->body);

    return res;
};

HttpResponse *internal_server_error(HttpResponse *res) {
    res->body = "500 InternalServerError";
    res->status = 500;
    res->contentType = "text/plain; charset=utf-8";
    res->contentLength = strlen(res->body);

    return res;
}