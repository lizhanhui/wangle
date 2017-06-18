#include "RemotingCommandCodec.h"

void rocketmq::RemotingCommandCodec::read(Context *ctx, std::unique_ptr<folly::IOBuf> buf) {
    if (buf) {
        buf->coalesce();
        RemotingCommand command;
        //TODO: decode
        ctx->fireRead(command);
    }
}

folly::Future<folly::Unit> rocketmq::RemotingCommandCodec::write(Context *ctx, rocketmq::RemotingCommand msg) {
    std::string data = "abc";
    auto buf = folly::IOBuf::copyBuffer(data.data(), data.size());
    return ctx->fireWrite(std::move(buf));
}
