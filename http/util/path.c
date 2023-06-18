//
// Created by casheeeewnuts on 6/18/23.
//

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

char *resolve_path(char *base, ...) {
    va_list e;
    va_start(e, base);
    char path[65535];
    char *following, *resolved;
    memset(path, 0, 65535);

    strcat(path, base);
    while ((following = va_arg(e, char *))) {
        strcat(path, following);
    }

    resolved = malloc(sizeof(char) * 65535);

    realpath(path, resolved);

    return resolved;
}