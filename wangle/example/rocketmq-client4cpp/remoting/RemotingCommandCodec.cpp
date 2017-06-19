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
    std::unique_ptr<folly::IOBuf> buf = msg.encode();
    return ctx->fireWrite(std::move(buf));
}
