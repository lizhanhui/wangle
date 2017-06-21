#include "RemotingCommandCodec.h"

void rocketmq::RemotingCommandCodec::read(Context *ctx, std::unique_ptr<folly::IOBuf> buf) {
    if (buf) {
        buf->coalesce();
        std::unique_ptr<rocketmq::RemotingCommand> command = rocketmq::RemotingCommand::decode(std::move(buf));
        ctx->fireRead(std::move(command));
    }
}

folly::Future<folly::Unit> rocketmq::RemotingCommandCodec::write(Context *ctx, std::shared_ptr<rocketmq::RemotingCommand> msg) {
    std::unique_ptr<folly::IOBuf> buf = msg->encode();
    return ctx->fireWrite(std::move(buf));
}
