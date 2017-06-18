#ifndef PROJECT_CLIENTHANDLER_H
#define PROJECT_CLIENTHANDLER_H

#include <wangle/channel/Handler.h>
#include "RemotingCommand.h"

namespace rocketmq {
    class ClientHandler : public wangle::HandlerAdapter<RemotingCommand> {
    public:

    };
}


#endif //PROJECT_CLIENTHANDLER_H
