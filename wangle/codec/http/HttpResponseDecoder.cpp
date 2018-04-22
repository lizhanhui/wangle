#include "HttpResponseDecoder.h"
using namespace wangle;

bool HttpResponseDecoder::decode(wangle::InboundHandler<folly::IOBufQueue &, wangle::HttpResponse>::Context *ctx,
                                 folly::IOBufQueue &buf, wangle::HttpResponse &result, size_t &) {
    return true;
}