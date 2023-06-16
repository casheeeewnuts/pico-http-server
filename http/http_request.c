//
// Created by casheeeewnuts on 6/16/23.
//

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "http_request.h"


typedef enum HttpRequestTokenKind {
    TK_Method,
    TK_Path,
    TK_Version,
} HttpRequestTokenKind;

typedef struct HttpRequestToken {
    HttpRequestTokenKind kind;
    struct HttpRequestToken *next;
    char *str;
    int len;
} HttpRequestToken;


HttpRequestToken *tokenize_http_request(const char *rawRequest);

HttpRequestToken *new_token(HttpRequestTokenKind kind, HttpRequestToken *token, char *str, int len);

void dispose_token(HttpRequestToken *token);

HttpRequest *parse_http_request(const char *rawRequest) {
    HttpRequestToken *token = tokenize_http_request(rawRequest);
    struct HttpRequest *request = calloc(1, sizeof(struct HttpRequest));

    request->version = V1_0;

    while (token->next) {
        if (token->kind == TK_Method) {
            if (strncmp(token->str, "GET", token->len) == 0) {
                request->method = Get;
            } else {
                return NULL;
            }
        }

        if (token->kind == TK_Path) {
            request->path = (char *) calloc(sizeof(char), token->len);
            strncpy(request->path, token->str, token->len);
        }

        token = token->next;
    }

    dispose_token(token);

    return request;
}

HttpRequestToken *tokenize_http_request(const char *rawRequest) {
    HttpRequestToken token;
    token.next = NULL;
    token.str = NULL;

    HttpRequestToken *cursor = &token;
    HttpRequestTokenKind mode = TK_Method;
    int tokenLen = 0;

    while (1) {
        if (rawRequest[tokenLen] == ' ') {
            cursor = new_token(mode, cursor, (char *)rawRequest, tokenLen);
            rawRequest += tokenLen + 1;
            mode++;
            tokenLen = 0;
            continue;
        }

        if (rawRequest[tokenLen] == '\n') {
            cursor = new_token(mode, cursor, (char *)rawRequest, tokenLen);
            break;
        }

        tokenLen += 1;
    }

    return token.next;
}

void dispose_token(HttpRequestToken *token) {
    HttpRequestToken *next;

    while (token) {
        next = token->next;
        free(token);

        token = next;
    }
}

HttpRequestToken *new_token(HttpRequestTokenKind kind, HttpRequestToken *token, char *str, int len) {
    HttpRequestToken *next = calloc(1, sizeof(HttpRequestToken));
    next->kind = kind;
    next->str = str;
    token->next = next;
    next->len = len;

    return next;
}