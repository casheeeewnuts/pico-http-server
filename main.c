#include <glib-2.0/glib.h>
#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include "http/http_server.h"
#include "http/util/path.h"

static char cwd[2048];
struct ServerOption parse_command_line_option(int argc, char **argv);
void init_process();

int main(int argc, char *argv[]) {
    init_process();

    struct ServerOption option = parse_command_line_option(argc, argv);
    HttpServer httpServer = new_http_server((struct ServerOption) option);

    httpServer.listen(&httpServer);

    return 0;
}

void init_process() {
    memset(cwd, 0, 2048);
    getcwd(cwd, 2048);
}

struct ServerOption parse_command_line_option(int argc, char **argv) {
    struct ServerOption option = {
            .port = 8000,
            .host = "127.0.0.1",
            .rootDir = cwd
    };
    struct option longopts[] = {
            { "port", required_argument, NULL, 'p'},
            { "host", required_argument, NULL, 'h'},
            { "root", required_argument, NULL, 'r'},
            { NULL, 0, NULL, 0},
    };
    char *resolved;
    int opt;
    int i;

    while ((opt = getopt_long(argc, argv, "p:h:r:", longopts, &i))) {
        switch (opt) {
            case 'p':
                option.port = strtol(optarg, NULL, 10);
                break;
            case 'h':
                option.host = optarg;
                break;
            case 'r':
                resolved = resolve_path(cwd, "/", optarg, NULL);

                if (resolved == NULL) {
                    perror("");
                    exit(EXIT_FAILURE);
                }

                option.rootDir = resolved;
                break;
            default:
                return option;
        }
    }

    return option;
}
