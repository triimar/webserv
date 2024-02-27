#include "../../include/Server.hpp"

const char *Server::getStatusMessage(int status) {
    switch (status) {
    // info
    case 100: return (STATUS_100);
    case 101: return (STATUS_101);
    case 102: return (STATUS_102);
    case 103: return (STATUS_103);
    // success
    case 200: return (STATUS_200);
    case 201: return (STATUS_201);
    case 202: return (STATUS_202);
    case 203: return (STATUS_203);
    case 204: return (STATUS_204);
    case 205: return (STATUS_205);
    case 206: return (STATUS_206);
    case 207: return (STATUS_207);
    case 208: return (STATUS_208);
    case 226: return (STATUS_226);
    // redirection
    case 300: return (STATUS_300);
    case 301: return (STATUS_301);
    case 302: return (STATUS_302);
    case 303: return (STATUS_303);
    case 304: return (STATUS_304);
    case 307: return (STATUS_307);
    case 308: return (STATUS_308);
    // client error
    case 400: return (STATUS_400);
    case 401: return (STATUS_401);
    case 402: return (STATUS_402);
    case 403: return (STATUS_403);
    case 404: return (STATUS_404);
    case 405: return (STATUS_405);
    case 406: return (STATUS_406);
    case 407: return (STATUS_407);
    case 408: return (STATUS_408);
    case 409: return (STATUS_409);
    case 410: return (STATUS_410);
    case 411: return (STATUS_411);
    case 412: return (STATUS_412);
    case 413: return (STATUS_413);
    case 414: return (STATUS_414);
    case 415: return (STATUS_415);
    case 416: return (STATUS_416);
    case 417: return (STATUS_417);
    case 418: return (STATUS_418);
    case 421: return (STATUS_421);
    case 422: return (STATUS_422);
    case 423: return (STATUS_423);
    case 424: return (STATUS_424);
    case 425: return (STATUS_425);
    case 426: return (STATUS_426);
    case 428: return (STATUS_428);
    case 429: return (STATUS_429);
    case 431: return (STATUS_431);
    case 451: return (STATUS_451);
    // server error
    case 500: return (STATUS_500);
    case 501: return (STATUS_501);
    case 502: return (STATUS_502);
    case 503: return (STATUS_503);
    case 504: return (STATUS_504);
    case 505: return (STATUS_505);
    case 506: return (STATUS_506);
    case 507: return (STATUS_507);
    case 508: return (STATUS_508);
    case 510: return (STATUS_510);
    case 511: return (STATUS_511);
    default: return (NULL);
    }
}
