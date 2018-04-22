#ifndef WANGLE_HTTP_RESPONS_EDECODER_H
#define WANGLE_HTTP_RESPONS_EDECODER_H

#include <wangle/codec/http/HttpResponse.h>
#include <wangle/codec/ByteToMessageDecoder.h>

namespace wangle {
    class HttpResponseDecoder : public ByteToMessageDecoder<HttpResponse> {
    public:
        bool decode(Context* ctx, folly::IOBufQueue& buf, HttpResponse& result, size_t&) override;
    };
}


#endif //WANGLE_HTTP_RESPONS_EDECODER_H
