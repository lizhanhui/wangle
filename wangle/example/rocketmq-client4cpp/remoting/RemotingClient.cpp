#include "RemotingClient.h"


void rocketmq::RemotingClient::start() {
    client.group(std::make_shared<wangle::IOThreadPoolExecutor>(1));
    client.pipelineFactory(std::make_shared<RocketMQPipelineFactory>());
}

rocketmq::RocketMQPipeline::Ptr rocketmq::RocketMQPipelineFactory::newPipeline(std::shared_ptr<folly::AsyncTransportWrapper> sock) {
    auto pipeline = RocketMQPipeline::create();
    pipeline->addBack(wangle::AsyncSocketHandler(sock));
    pipeline->addBack(wangle::EventBaseHandler());
    pipeline->addBack(wangle::LengthFieldBasedFrameDecoder(4, UINT_MAX, 0, 0, 4, true));
    pipeline->addBack(rocketmq::RemotingCommandCodec());
    pipeline->addBack(rocketmq::ClientHandler());
    pipeline->finalize();
    return pipeline;
}



void rocketmq::RemotingClient::invoke(std::string address, RemotingCommand &command, unsigned long timeout) {
    unsigned long long key = addressToLongLong(address);
    auto ret = channelTable.find(key);

    if (ret == channelTable.end()) {
        std::shared_ptr<ChannelWrapper> channelWrapper = connect(address, timeout);
        invokeImpl(channelWrapper, command, timeout);
    } else {
        std::shared_ptr<ChannelWrapper> channelWrapper = ret->second;
        invokeImpl(channelWrapper, command, timeout);
    }
}

void rocketmq::RemotingClient::invokeAsync(const std::string& address, RemotingCommand& command,
                                           const InvokeCallback& callback,
                                           unsigned long timeout) {
    unsigned long long key = addressToLongLong(address);
    auto ret = channelTable.find(key);
    std::shared_ptr<ChannelWrapper> channelWrapper;
    if (ret == channelTable.end()) {
        channelWrapper = connect(address, timeout);
        if (nullptr != channelWrapper) {
            channelTable.insert(std::make_pair(key, channelWrapper));
        }
    } else {
        channelWrapper = ret->second;
    }

    invokeAsyncImpl(channelWrapper, command, callback, timeout);

}

void rocketmq::RemotingClient::invokeImpl(std::shared_ptr<ChannelWrapper> channelWrapper,
                                          RemotingCommand &command,
                                          unsigned long timeout) {
    folly::Future<RocketMQPipeline*> future = channelWrapper->write(command);
    future.wait(folly::Duration(timeout));
}

void rocketmq::RemotingClient::invokeAsyncImpl(std::shared_ptr<ChannelWrapper> channelWrapper,
                                               const RemotingCommand &command,
                                               const InvokeCallback& callback,
                                               unsigned long timeout) {
    channelWrapper->write(command);
    unsigned long long timeoutMillis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + timeout;
    std::shared_ptr<ResponseFuture> future = std::make_shared<ResponseFuture>(command.getOpaque(), timeoutMillis, callback);
    responseTable.insert(std::make_pair(command.getOpaque(), future));
}

std::shared_ptr<rocketmq::ChannelWrapper> rocketmq::RemotingClient::connect(const std::string& address, unsigned long timeout) {
    unsigned long long key = addressToLongLong(address);
    auto ret = channelTable.find(key);
    if (ret != channelTable.end()) {
        return ret->second;
    }

    {
        std::lock_guard<std::mutex> guard(lock);
        ret = channelTable.find(key);
        if (ret != channelTable.end()) {
            return ret->second;
        } else {
            std::string::size_type colonPosition = address.find(":");
            std::string ip = address.substr(0, colonPosition);
            uint16_t port = (uint16_t) std::stoul(address.substr(colonPosition + 1));
            folly::SocketAddress socketAddress = folly::SocketAddress(ip, port);
            folly::Future<RocketMQPipeline*> future = client.connect(socketAddress, std::chrono::milliseconds(timeout));
            std::shared_ptr<rocketmq::ChannelWrapper> channelWrapper = std::make_shared<rocketmq::ChannelWrapper>(future.get(std::chrono::milliseconds(timeout)));
            channelTable.insert(std::make_pair(key, channelWrapper));
            return channelWrapper;
        }
    }

}
