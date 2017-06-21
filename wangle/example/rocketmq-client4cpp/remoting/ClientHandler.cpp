#include "ClientHandler.h"

/**
 * Dispatch response to processors.
 * @param ctx
 * @param msg
 */
void rocketmq::ClientHandler::read(Context *ctx, std::shared_ptr<rocketmq::RemotingCommand> msg) {
    if (msg) {
        if (msg->isResponse()) {
            auto ret = client->responseTable.find(msg->getOpaque());
            if (ret == client->responseTable.end()) {
                // The request must have already been scanned and marked timeout
                //TODO: log and warn
            } else {
                std::shared_ptr<ResponseFuture> future = ret->second;
                future->setResult(msg);
                future->countDown();
                if(!future->getCallback()) {
                    //TODO: for now, execute the callback via IO executor pool.
                    future->getCallback()->onSuccess();
                }
            }
        }
    } else {
        //TODO: log and warn
    }
}
