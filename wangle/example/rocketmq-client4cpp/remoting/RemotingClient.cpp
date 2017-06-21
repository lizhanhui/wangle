#include "RemotingClient.h"


void rocketmq::RemotingClient::start() {
    client.group(std::make_shared<wangle::IOThreadPoolExecutor>(1));
    client.pipelineFactory(std::make_shared<RocketMQPipelineFactory>(this));
}

rocketmq::RocketMQPipeline::Ptr rocketmq::RocketMQPipelineFactory::newPipeline(std::shared_ptr<folly::AsyncTransportWrapper> sock) {
    auto pipeline = RocketMQPipeline::create();
    pipeline->addBack(wangle::AsyncSocketHandler(sock));
    pipeline->addBack(wangle::EventBaseHandler());
    pipeline->addBack(wangle::LengthFieldBasedFrameDecoder(4, UINT_MAX, 0, 0, 4, true));
    pipeline->addBack(rocketmq::RemotingCommandCodec());
    pipeline->addBack(rocketmq::ClientHandler(client));
    pipeline->finalize();
    return pipeline;
}



std::shared_ptr<rocketmq::RemotingCommand> rocketmq::RemotingClient::invoke(std::string address,
                                      std::shared_ptr<RemotingCommand> command,
                                      unsigned long timeout) {
    unsigned long long key = addressToLongLong(address);
    auto ret = channelTable.find(key);
    rocketmq::RocketMQPipeline* pipeline;
    if (ret == channelTable.end()) {
        pipeline = connect(address, timeout);
    } else {
        pipeline = ret->second;
    }
    return invokeImpl(pipeline, std::move(command), timeout);
}

void rocketmq::RemotingClient::invokeAsync(const std::string& address,
                                           std::shared_ptr<RemotingCommand> command,
                                           std::shared_ptr<InvokeCallback> callback,
                                           unsigned long timeout) {
    unsigned long long key = addressToLongLong(address);
    auto ret = channelTable.find(key);
    rocketmq::RocketMQPipeline* pipeline;
    if (ret == channelTable.end()) {
        pipeline = connect(address, timeout);
    } else {
        pipeline = ret->second;
    }

    invokeAsyncImpl(pipeline, std::move(command), callback, timeout);
}

std::shared_ptr<rocketmq::RemotingCommand> rocketmq::RemotingClient::invokeImpl(rocketmq::RocketMQPipeline* pipeline,
                                          std::shared_ptr<RemotingCommand> command,
                                          unsigned long timeout) {
    pipeline->write(command)
            .within(folly::Duration(timeout))
            .then([]{
                //TODO: Replace std::cout with glog.
                std::cout << "Sent OK" << std::endl;
            });
    std::shared_ptr<ResponseFuture> future = std::make_shared<ResponseFuture>(command->getOpaque(), timeout, nullptr);
    responseTable.insert(std::make_pair(command->getOpaque(), future));
    future->await();
    if (future->isSuccess()) {
        return future->getResult();
    } else {
        responseTable.erase(command->getOpaque());
        return nullptr;
    }
}

void rocketmq::RemotingClient::invokeAsyncImpl(rocketmq::RocketMQPipeline* pipeline,
                                               std::shared_ptr<RemotingCommand> command,
                                               std::shared_ptr<InvokeCallback> callback,
                                               unsigned long timeout) {

    unsigned long long timeoutMillis = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count() + timeout;
    std::shared_ptr<ResponseFuture> future = std::make_shared<ResponseFuture>(command->getOpaque(), timeoutMillis, callback);
    responseTable.insert(std::make_pair(command->getOpaque(), future));

    pipeline->write(command);
//            .then([&]{
//                callback->onSuccess();
//            })
//            .onError([&]{
//                responseTable.erase(command->getOpaque());
//                callback->onError();
//            })
//            .onTimeout(folly::Duration(timeout), [&] {
//                responseTable.erase(command->getOpaque());
//                callback->onError();
//            });
}

rocketmq::RocketMQPipeline* rocketmq::RemotingClient::connect(const std::string& address, unsigned long timeout) {
    unsigned long long key = addressToLongLong(address);
    auto ret = channelTable.find(key);
    if (ret != channelTable.end()) {
        return ret->second;
    }


    {
        std::lock_guard<std::mutex> guard(lock);
        ret = channelTable.find(key);
        if (ret == channelTable.end()) {
            std::string::size_type colonPosition = address.find(":");
            std::string ip = address.substr(0, colonPosition);
            uint16_t port = (uint16_t) std::stoul(address.substr(colonPosition + 1));
            folly::SocketAddress socketAddress = folly::SocketAddress(ip, port);
            folly::Future<RocketMQPipeline*> future = client.connect(socketAddress, std::chrono::milliseconds(timeout));
            RocketMQPipeline* pipeline = future.get();
            channelTable.insert(std::make_pair(key, pipeline));
            return pipeline;
        } else {
            return ret->second;
        }
    }
}
