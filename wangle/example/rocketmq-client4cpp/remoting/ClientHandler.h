#ifndef PROJECT_CLIENTHANDLER_H
#define PROJECT_CLIENTHANDLER_H

#include <wangle/channel/Handler.h>
#include "RemotingCommand.h"

namespace rocketmq {
    class ClientHandler : public wangle::HandlerAdapter<std::shared_ptr<RemotingCommand> > {
    public:
        typedef wangle::HandlerAdapter<std::shared_ptr<RemotingCommand> >::Context Context;

        void read(Context *ctx, std::shared_ptr<RemotingCommand> msg) override;

    };
}


#endif //PROJECT_CLIENTHANDLER_H
