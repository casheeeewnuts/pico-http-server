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

typedef enum RedirectType {
    Temporary,
    Permanent
} RedirectType;

char *build_response_header(const HttpResponse *response);
HttpResponse *redirect(HttpResponse *res, RedirectType type, char *location);
HttpResponse *not_found(HttpResponse *res);
HttpResponse *method_not_allowed(HttpResponse *res);
HttpResponse *forbidden(HttpResponse *res);
HttpResponse *internal_server_error(HttpResponse *res);

#endif //PICO_HTTP_SERVER_HTTP_RESPONSE_H
