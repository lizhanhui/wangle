#ifndef PROJECT_REMOTINGCLIENT_H
#define PROJECT_REMOTINGCLIENT_H

#include <arpa/inet.h>
#include <wangle/bootstrap/ClientBootstrap.h>
#include <wangle/channel/AsyncSocketHandler.h>
#include <wangle/channel/EventBaseHandler.h>
#include <wangle/codec/LengthFieldBasedFrameDecoder.h>
#include <wangle/codec/StringCodec.h>
#include <folly/AtomicHashMap.h>
#include <folly/SocketAddress.h>
#include <ctime>
#include <mutex>
#include <iostream>

#include "RemotingCommand.h"
#include "RemotingCommandCodec.h"
#include "ClientHandler.h"
#include "ResponseFuture.h"

namespace rocketmq {

    typedef wangle::Pipeline<folly::IOBufQueue&, std::shared_ptr<RemotingCommand> > RocketMQPipeline;

    class RocketMQPipelineFactory : public wangle::PipelineFactory<RocketMQPipeline> {
    public:
        RocketMQPipeline::Ptr newPipeline(std::shared_ptr<folly::AsyncTransportWrapper> sock) override;
    };

    class RemotingClient {
    public:

        RemotingClient() : channelTable(16), responseTable(16) {

        }

        void start();

        RocketMQPipeline* connect(const std::string& address, unsigned long timeout);

        static unsigned long long addressToLongLong(const std::string& addr) {
            std::string::size_type colon_position = addr.find(":");
            if (colon_position == std::string::npos) {
                return 0;
            } else {
                std::string ip = addr.substr(0, colon_position);
                std::string port = addr.substr(colon_position + 1);

                unsigned long long result = 0;
                struct sockaddr_in sock;
                if(inet_pton(AF_INET, ip.c_str(), &(sock.sin_addr)) > 0) {
                    result += sock.sin_addr.s_addr;
                    result = (result << 16) + std::stoi(port);
                    return result;
                }

            }

            return 0;
        }

        void invoke(std::string address, std::shared_ptr<RemotingCommand> command, unsigned long timeout);

        void invokeAsync(const std::string& address,
                         std::shared_ptr<RemotingCommand> command,
                         std::shared_ptr<InvokeCallback> callback,
                         unsigned long timeout);

        void invokeImpl(RocketMQPipeline* pipeline,
                        std::shared_ptr<RemotingCommand> command,
                        unsigned long timeout);

        void invokeAsyncImpl(RocketMQPipeline* pipeline,
                             std::shared_ptr<RemotingCommand> command,
                             std::shared_ptr<InvokeCallback> callback,
                             unsigned long timeout);


        void shutdown() {
        }

    private:
        wangle::ClientBootstrap<RocketMQPipeline> client;
        folly::AtomicHashMap<unsigned long long, RocketMQPipeline*> channelTable;
        folly::AtomicHashMap<int, std::shared_ptr<ResponseFuture> > responseTable;
        std::mutex lock;

    };
}

#endif //PROJECT_REMOTINGCLIENT_H
