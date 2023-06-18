#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "http_header.h"
#include "http_response.h"

char *status_message(unsigned short status);
void add_header(Header *headers, char *key, char *value);
void del_header(Header *headers, char *key);

HttpResponse *new_response() {
    HttpResponse *response = calloc(1, sizeof(HttpResponse));

    response->headers = new_header();
    response->body = calloc(sizeof(char), 65535);

    return response;
}

char *build_response_header(const HttpResponse *response) {
    char *raw_header = malloc(sizeof(char) * 65535);

    sprintf(raw_header, "HTTP/1.0 %3d %s\n", response->status, status_message(response->status));
    sprintf(&raw_header[strlen(raw_header)], "Content-Length: %d\n", response->contentLength);

    if (response->contentType != NULL) {
        sprintf(&raw_header[strlen(raw_header)], "Content-Type: %s\n", response->contentType);
    }

    GList *header = g_hash_table_get_keys(response->headers->table);
    char *headerValue;

    while (header) {
        headerValue = g_hash_table_lookup(response->headers->table, header->data);
        sprintf(&raw_header[strlen(raw_header)], "%s: %s\n", (char *)header->data, headerValue);
        header = header->next;
    }

    raw_header[strlen(raw_header)] = '\n';
    return raw_header;
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

    res->headers->add(res->headers, "Location", location);

    return res;
}

HttpResponse *not_found(HttpResponse *res) {
    strcpy(res->body, "404 Not Found");
    res->status = 404;
    res->contentType = "text/plain; charset=utf-8";
    res->contentLength = strlen(res->body);

    return res;
}

HttpResponse *method_not_allowed(HttpResponse *res) {
    strcpy(res->body, "405 Method Not Allowed");
    res->status = 405;
    res->contentType = "text/plain; charset=utf-8";
    res->contentLength = strlen(res->body);

    return res;
}

HttpResponse *forbidden(HttpResponse *res) {
    strcpy(res->body, "403 Forbidden");

    res->status = 403;
    res->contentType = "text/plain; charset=utf-8";
    res->contentLength = strlen(res->body);

    return res;
};

HttpResponse *internal_server_error(HttpResponse *res) {
    strcpy(res->body, "500 InternalServerError");
    res->status = 500;
    res->contentType = "text/plain; charset=utf-8";
    res->contentLength = strlen(res->body);

    return res;
}

void dispose_response(HttpResponse *response) {
    g_hash_table_destroy(response->headers->table);
    response->headers->destroy(response->headers);
    free(response->body);
    free(response);
}