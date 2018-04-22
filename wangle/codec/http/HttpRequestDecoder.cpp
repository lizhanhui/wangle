#include "HttpRequestDecoder.h"
#include <folly/io/Cursor.h>
#include <folly/String.h>
#include <iostream>

using namespace wangle;
using namespace folly;
using folly::io::Cursor;

HttpRequestDecoder::HttpRequestDecoder() : parseState(EXPECT_REQUEST_LINE) {


}

int64_t HttpRequestDecoder::findEndOfLine(folly::IOBufQueue &buf) {
    Cursor c(buf.front());
    for(uint32_t i = 0; i < buf.chainLength(); i++) {
        auto b = c.read<char>();
        if (b == '\r') {
            if (i + DELIMITER_LENGTH <= buf.chainLength()) {
                return i;
            } else {
                return -1;
            }
        }
    }
    return -1;
}

bool HttpRequestDecoder::decode(wangle::InboundHandler<folly::IOBufQueue &, wangle::HttpRequest>::Context *ctx,
                                folly::IOBufQueue &buf,
                                wangle::HttpRequest &result,
                                size_t &) {

    parseState = EXPECT_REQUEST_LINE;

    while (parseState != DONE) {
        switch(parseState) {
            case EXPECT_REQUEST_LINE:
            {
                // TODO: Consider maximum length of request line
                int64_t eol = findEndOfLine(buf);
                if (eol > 0) {
                    std::unique_ptr<IOBuf> frame = buf.split(eol + DELIMITER_LENGTH);
                    frame->trimEnd(DELIMITER_LENGTH);
                    frame->coalesce();
                    std::string requestLine((const char*)frame->data(), frame->length());
                    std::vector<std::string> pieces;
                    folly::split(" ", requestLine, pieces, true);
                    if (pieces.size() >= 3) {
                        result.setMethod(pieces[0]);
                        result.setUri(pieces[1]);
                        result.setVersion(pieces[2]);
                    }
                } else {
                    return false;
                }
                parseState = EXPECT_HEADERS;
                break;
            }
            case EXPECT_HEADERS:
            {
                while (true) {
                    int64_t eol = findEndOfLine(buf);
                    if (eol == 0) { // End of headers
                        buf.split(DELIMITER_LENGTH);
                        break;
                    }

                    if (eol > 0) {
                        std::unique_ptr<IOBuf> frame = buf.split(eol + DELIMITER_LENGTH);
                        frame->trimEnd(DELIMITER_LENGTH);
                        frame->coalesce();
                        std::string headerLine((const char*)frame->data(), frame->length());
                        std::string::size_type colonPos = headerLine.find(':');
                        result.put(headerLine.substr(0, colonPos), headerLine.substr(colonPos + 2));
                    }
                }
                parseState = EXPECT_BODY;
                break;
            }

            case EXPECT_BODY:
            {
                std::string contentLengthKey = "Content-Length";
                std::string contentLength = result.getHeader(contentLengthKey);
                if (!contentLength.empty()) {
                    int length = std::stoi(contentLength);
                    if (length > 0) {
                        std::unique_ptr<IOBuf> body = buf.split(length);
                        result.setBody(body);
                    }
                }
                parseState = DONE;
                break;
            }
            default:
                return false;
        }


    }

    return true;

}

const uint8_t HttpRequestDecoder::DELIMITER_LENGTH = 2;

