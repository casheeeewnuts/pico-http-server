//
// Created by casheeeewnuts on 6/17/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "http_server.h"

void http_server_listen(HttpServer *);
int set_tcp_socket(HttpServer *server);
int wait_connection(const HttpServer *server);
int handle_connection(const HttpServer *server, int clientSocket, const struct sockaddr_in *clientAddr);
int handle_request(const HttpServer *server, const HttpRequest *request, HttpResponse *response);
void dispose_request(HttpRequest *request);
void dispose_response(HttpResponse *response);

HttpServer new_http_server(struct ServerOption option) {
    HttpServer server;

    server.serverOption = option;
    server.listen = http_server_listen;
    server.socket = -1;

    return server;
}

void http_server_listen(HttpServer *server) {
    if (set_tcp_socket(server)) {
        perror("set_tcp_socket");
        exit(EXIT_FAILURE);
    }

    if (listen(server->socket, SOMAXCONN)) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    wait_connection(server);
}

int set_tcp_socket(HttpServer *server) {
    int sockFd;
    struct sockaddr_in serverAddr;

    if ((sockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(server->serverOption.host);
    serverAddr.sin_port = htons(server->serverOption.port);

    if (bind(sockFd, (struct sockaddr *) &serverAddr, sizeof(serverAddr))) {
        return 1;
    }

    server->socket = sockFd;

    return 0;
}

int wait_connection(const HttpServer *server) {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSock;

    while (1) {
        clientSock = accept(server->socket, (struct sockaddr *) &clientAddr, &clientAddrLen);

        if (clientSock < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        if (handle_connection(server, clientSock, &clientAddr)) {
            return 1;
        }
    }
}

int handle_connection(const HttpServer *server, int clientSocket, const struct sockaddr_in *clientAddr) {
    HttpRequest *request = parse_http_request("GET / HTTP/1.0");
    request->socket = clientSocket;
    request->remoteAddr = *clientAddr;
    HttpResponse response;


    handle_request(server, request, &response);

    char *responseHeader = build_response_header(&response);

    send(request->socket, responseHeader, strlen(responseHeader), 0);
    send(request->socket, response.body, response.contentLength, 0);

    close(request->socket);

    dispose_request(request);
    dispose_response(&response);
    free(responseHeader);

    return 0;
}

int handle_request(const HttpServer *server, const HttpRequest *request, HttpResponse *response) {
    char *body = "hello!";

    response->status = 200;
    response->contentType = "text/plain; charset=utf-8";
    response->contentLength = strlen(body);
    response->body = body;
}

void dispose_request(HttpRequest *request) {
    free(request);
}

void dispose_response(HttpResponse *response) {}