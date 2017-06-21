#ifndef PROJECT_CLIENTHANDLER_H
#define PROJECT_CLIENTHANDLER_H

#include <wangle/channel/Handler.h>
#include "RemotingCommand.h"
#include "RemotingClient.h"

namespace rocketmq {

    class RemotingClient;

    class ClientHandler : public wangle::HandlerAdapter<std::shared_ptr<RemotingCommand> > {
    public:
        typedef wangle::HandlerAdapter<std::shared_ptr<RemotingCommand> >::Context Context;

        ClientHandler(RemotingClient *client) : client(client) {

        }

        void read(Context *ctx, std::shared_ptr<RemotingCommand> msg) override;

    private:
        RemotingClient *client;

    };
}


#endif //PROJECT_CLIENTHANDLER_H
