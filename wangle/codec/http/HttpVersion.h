#ifndef WANGLE_HTTP_VERSION_H
#define WANGLE_HTTP_VERSION_H

namespace wangle {
    enum HttpVersion {
        UNKNOWN,
        HTTP_1_0,
        HTTP_1_1,
        HTTP_2_0
    };
}

#endif //WANGLE_HTTP_VERSION_H
