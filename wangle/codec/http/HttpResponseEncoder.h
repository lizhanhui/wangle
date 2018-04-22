#ifndef WANGLE_HTTP_RESPONSE_ENCODER_H
#define WANGLE_HTTP_RESPONSE_ENCODER_H

#include <wangle/codec/http/HttpResponse.h>
#include <wangle/codec/MessageToByteEncoder.h>

namespace wangle {
    class HttpResponseEncoder : public MessageToByteEncoder<HttpResponse> {

    public:
        std::unique_ptr<folly::IOBuf> encode(HttpResponse& msg) override;
    };
}


#endif //WANGLE_HTTP_RESPONSE_ENCODER_H
