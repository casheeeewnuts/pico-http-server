//
// Created by casheeeewnuts on 6/16/23.
//

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "http_request.h"


typedef enum HttpRequestTokenKind {
    TK_Method,
    TK_Path,
    TK_Version,
    TK_Header,
    TK_Body
} HttpRequestTokenKind;

typedef struct HttpRequestToken {
    HttpRequestTokenKind kind;
    struct HttpRequestToken *next;
    char *str;
    unsigned long len;
} HttpRequestToken;


HttpRequestToken *tokenize_http_request(const char *rawRequest, unsigned long len);

HttpRequestToken *new_token(HttpRequestTokenKind kind, HttpRequestToken *token, char *str, unsigned long len);

void dispose_token(HttpRequestToken *token);

HttpRequest *parse_http_request(const char *rawRequest) {
    HttpRequestToken *token = tokenize_http_request(rawRequest, 65535);
    struct HttpRequest *request = calloc(1, sizeof(struct HttpRequest));
    request->header = new_header();

    while (token) {
        if (token->kind == TK_Method) {
            if (strncmp(token->str, "GET", token->len) == 0) {
                request->method = Get;
            } else if (strncmp(token->str, "HEAD", token->len) == 0) {
                request->method = Head;
            } else if (strncmp(token->str, "POST", token->len) == 0) {
                request->method = Post;
            }
        }

        if (token->kind == TK_Path) {
            request->path = (char *) calloc(sizeof(char), token->len);
            strncpy(request->path, token->str, token->len);
        }

        if (token->kind == TK_Version) {
            request->version = V1_0;
        }

        if (token->kind == TK_Header) {
            char key[2048], value[2048];
            sscanf(token->str, "%s %s", key, value);
            key[strlen(key) - 1] = '\0';

            request->header->add(request->header, key, value);
        }

        if (token->kind == TK_Body) {
            char *contentLen = request->header->get(request->header, "Content-Length");

            if (contentLen) {
                int len = atoi(request->header->get(request->header, "Content-Length"));
                request->bodySize = len;
                request->body = calloc(sizeof(char), request->bodySize);
                memcpy(request->body, token->str, request->bodySize);

            }
        }

        token = token->next;
    }

    dispose_token(token);

    return request;
}

void dispose_request(HttpRequest *request) {
    request->header->destroy(request->header);
    free(request->path);
    free(request->body);
    free(request);
}

HttpRequestToken *tokenize_http_request(const char *rawRequest, unsigned long len) {
    HttpRequestToken token;
    memset(&token, 0, sizeof(HttpRequestToken));
    token.next = NULL;
    token.str = NULL;

    HttpRequestToken *cursor = &token;
    HttpRequestTokenKind mode = TK_Method;
    int tokenLen = 0;
    char *head = (char *)rawRequest;

    while (1) {
        if ((mode == TK_Method || mode == TK_Path) &&
            rawRequest[tokenLen] == ' '
                ) {
            cursor = new_token(mode, cursor, (char *) rawRequest, tokenLen);
            rawRequest += tokenLen + 1;
            mode++;
            tokenLen = 0;
            continue;
        }

        if (rawRequest[tokenLen] == '\r' && rawRequest[tokenLen + 1] == '\n') {
            cursor = new_token(mode, cursor, (char *) rawRequest, tokenLen);
            rawRequest += tokenLen + 2;
            tokenLen = 0;

            if (rawRequest[tokenLen] == '\r' && rawRequest[tokenLen + 1] == '\n') {
                mode = TK_Body;
                rawRequest += 2;
                cursor = new_token(mode, cursor, (char *) rawRequest, len - (long)(rawRequest - head));
                break;
            } else {
                if (mode != TK_Header) {
                    mode++;
                }
            }

            continue;
        }

        tokenLen += 1;
    }

    return token.next;
}

void dispose_token(HttpRequestToken *token) {
    HttpRequestToken *next;

    while (token) {
        next = token->next;
        free(token->str);
        free(token);

        token = next;
    }
}

HttpRequestToken *new_token(HttpRequestTokenKind kind, HttpRequestToken *token, char *str, unsigned long len) {
    HttpRequestToken *next = calloc(1, sizeof(HttpRequestToken));
    next->kind = kind;
    next->str = calloc(sizeof(char), len);
    memcpy(next->str, str, len);
    token->next = next;
    next->len = len;

    return next;
}