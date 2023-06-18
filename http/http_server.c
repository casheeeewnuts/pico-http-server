#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <glib-2.0/glib.h>
#include "http_server.h"
#include "util/path.h"

// private methods prototypes
void http_server_listen(HttpServer *);
int set_tcp_socket(HttpServer *server);
int wait_connection(const HttpServer *server);
int handle_connection(const HttpServer *server, int clientSocket, const struct sockaddr_in *clientAddr);
void handle_request(const HttpServer *server, const HttpRequest *request, HttpResponse *response);
int static_file_handler(const HttpServer *server, const HttpRequest *req, HttpResponse *res);
char *resolve_mime_type(MimeType, char *ext);

HttpServer new_http_server(struct ServerOption option) {
    HttpServer server;

    server.serverOption = option;
    server.listen = http_server_listen;
    server.socket = -1;
    server.mimeType.table = g_hash_table_new(g_str_hash, g_str_equal);
    server.mimeType.resolve = resolve_mime_type;

    g_hash_table_insert(server.mimeType.table, ".html", "text/html; charset=utf-8");
    g_hash_table_insert(server.mimeType.table, ".js", "text/javascript; charset=utf-8");
    g_hash_table_insert(server.mimeType.table, ".css", "text/css; charset=utf-8");
    g_hash_table_insert(server.mimeType.table, ".png", "image/png;");
    g_hash_table_insert(server.mimeType.table, ".jpg", "image/jpg;");

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

        handle_connection(server, clientSock, &clientAddr);
    }
}

int handle_connection(const HttpServer *server, int clientSocket, const struct sockaddr_in *clientAddr) {
    HttpRequest *request = accept_http_request(clientSocket, *clientAddr);

    if (!request) {
        close(clientSocket);
        return 1;
    }

    HttpResponse *response = new_response();

    handle_request(server, request, response);

    char *responseHeader = build_response_header(response);

    send(request->socket, responseHeader, strlen(responseHeader), 0);

    if (request->method != Head) {
        send(request->socket, response->body, response->contentLength, 0);
    }

    close(request->socket);

    dispose_request(request);
    dispose_response(response);
    free(responseHeader);

    return 0;
}

void handle_request(const HttpServer *server, const HttpRequest *request, HttpResponse *response) {
    static_file_handler(server, request, response);
}

char *resolve_path_from_server_root(const HttpServer *server, char *reqPath) {
    char *path = resolve_path(server->serverOption.rootDir, "/", reqPath, NULL);

    if (path[strlen(path) - 1] == '/') {
        strcat(path, "index.html");
    }

    return path;
}

int static_file_handler(const HttpServer *server, const HttpRequest *req, HttpResponse *res) {
    FILE *fp;
    int fd;
    struct stat st;
    char *path = resolve_path_from_server_root(server, req->path);
    char *ext, *mimeType;

    if (req->method == Head || req->method == Get) {
        fd = open(path, O_RDONLY);

        if (errno == EACCES) {
            forbidden(res);
            return 1;
        }

        if (errno == ENOENT) {
            not_found(res);
            return 1;
        }

        if (fstat(fd, &st) == -1) {
            internal_server_error(res);
            return 1;
        }

        if (S_ISDIR(st.st_mode)) {
            char *tmp = calloc(sizeof(char), 2049);
            strcat(tmp, req->path);

            if (tmp[strlen(tmp) - 1] != '/') {
                strcat(tmp, "/");
            }

            strcat(tmp, "index.html");

            redirect(res, Temporary, tmp);
            free(tmp);
            return 1;
        }

        res->contentLength = st.st_size;
        ext = strrchr(path, '.');
        mimeType = server->mimeType.resolve(server->mimeType, ext);

        if (mimeType != NULL) {
            res->contentType = mimeType;
        } else {
            res->contentType = "application/octet-stream";
        }

        if (req->method == Head) {
            res->status = 200;
            free(path);
            return 0;
        }

        fp = fdopen(fd, "rb");

        res->body = malloc(res->contentLength);
        fread(res->body, sizeof(char), res->contentLength, fp);

        res->status = 200;

        free(path);
        return 0;
    } else if (req->method == Post) {
        fp = fopen(path, "w");

        if (!fp) {
            perror("fopen");
            internal_server_error(res);
            return 1;
        }

        unsigned long s;
        s = fwrite(req->body, sizeof(char), req->bodySize, fp);

        fflush(fp);
        fclose(fp);
        printf("%lu %lu\n", req->bodySize, s);

        res->status = 201;
        res->contentLength = 0;
        return 0;
    } else {
        method_not_allowed(res);
        return 1;
    }
}

char *resolve_mime_type(MimeType mimeType, char *ext) {
    if (ext == NULL) {
        return NULL;
    }

    return g_hash_table_lookup(mimeType.table, ext);
}
