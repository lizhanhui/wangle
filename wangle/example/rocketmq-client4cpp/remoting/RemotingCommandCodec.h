//
// Created by Li Zhanhui on 2017/6/18.
//

#ifndef PROJECT_REMOTINGCOMMANDCODEC_H
#define PROJECT_REMOTINGCOMMANDCODEC_H

#include <wangle/channel/Handler.h>
#include "RemotingCommand.h"

namespace rocketmq {
    class RemotingCommandCodec : public wangle::Handler<std::unique_ptr<folly::IOBuf>, RemotingCommand, RemotingCommand, std::unique_ptr<folly::IOBuf> > {

    public:
        typedef typename wangle::Handler<std::unique_ptr<folly::IOBuf>, RemotingCommand,
                                         RemotingCommand, std::unique_ptr<folly::IOBuf>>::Context Context;

        void read(Context *ctx, std::unique_ptr<folly::IOBuf> msg) override;

        folly::Future<folly::Unit> write(Context *ctx, RemotingCommand msg) override;
    };
}


#endif //PROJECT_REMOTINGCOMMANDCODEC_H
