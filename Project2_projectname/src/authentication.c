#include "authentication.h"
#include <string.h>

int authenticate_request(HttpRequest *request) {
    if (strstr(request->path, "/secure")) {
        // Hardcoded username/password
        return strcmp(request->auth, "Basic dXNlcm5hbWU6cGFzc3dvcmQ=") == 0; // base64 for "username:password"
    }
    return 1; // allow access to non-secure paths
}
