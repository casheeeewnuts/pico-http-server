#include "http/http_server.h"

int main() {
    struct ServerOption option = {
            .port = 8000,
            .host = "127.0.0.1"
    };

    HttpServer httpServer = new_http_server(option);

    httpServer.listen(&httpServer);

    return 0;
}
