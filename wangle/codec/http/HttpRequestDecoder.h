#ifndef WANGLE_HTTP_REQUEST_DECODER_H
#define WANGLE_HTTP_REQUEST_DECODER_H

#include <wangle/codec/http/HttpRequest.h>
#include <wangle/codec/ByteToMessageDecoder.h>

namespace wangle {
    class HttpRequestDecoder : public ByteToMessageDecoder<HttpRequest> {
    public:
        enum HttpRequestParseState {
            EXPECT_REQUEST_LINE,
            EXPECT_HEADERS,
            EXPECT_BODY,
            DONE
        };

        HttpRequestDecoder();

        int64_t findEndOfLine(folly::IOBufQueue& buf);

        bool decode(Context* ctx, folly::IOBufQueue& buf, HttpRequest& result, size_t&) override;

    private:
        HttpRequestParseState parseState;
        static const uint8_t DELIMITER_LENGTH;
    };
}


#endif //WANGLE_HTTP_REQUEST_DECODER_H
