#ifndef WANGLE_HTTP_REQUEST_ENCODER_H
#define WANGLE_HTTP_REQUEST_ENCODER_H

#include <wangle/codec/http/HttpRequest.h>
#include <wangle/codec/MessageToByteEncoder.h>

namespace wangle {
    class HttpRequestEncoder : public MessageToByteEncoder<HttpRequest> {
    public:
        std::unique_ptr<folly::IOBuf> encode(HttpRequest& msg) override;
    };
}


#endif //WANGLE_HTTP_REQUEST_ENCODER_H
