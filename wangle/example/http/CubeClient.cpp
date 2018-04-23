

#include <string>
#include <wangle/codec/LengthFieldBasedFrameDecoder.h>
#include <wangle/codec/StringCodec.h>

#include "CubeClient.h"

using namespace folly;
using namespace wangle;


void CubeClientHandler::read(Context* ctx, std::string msg) {

}

CubeClientPipeline::Ptr
CubeClientPipelineFactory::newPipeline(std::shared_ptr<folly::AsyncTransportWrapper> socket) {
    auto pipeline = CubeClientPipeline::create();
    pipeline->addBack(AsyncSocketHandler(socket));
    pipeline->addBack(EventBaseHandler());
    pipeline->addBack(LengthFieldBasedFrameDecoder());
    pipeline->addBack(StringCodec());
    pipeline->addBack(CubeClientHandler());
    pipeline->finalize();
    return pipeline;
}

CubeClient::CubeClient() : stopped(false) {

}

void CubeClient::start() {
    client.group(std::make_shared<IOThreadPoolExecutor>(1));
    client.pipelineFactory(std::make_shared<CubeClientPipelineFactory>());

    for (int i = 0; i < std::thread::hardware_concurrency(); ++i) {
        std::thread t(process);
        workers.push_back(std::move(t));
    }
}

void CubeClient::process() {

    while (!stopped) {

    }

}

void CubeClient::stop() {
    stopped = true;
}

CubeClientPipeline* CubeClient::getConnection(const std::string &address) {

    std::unordered_map<std::string, CubeClientPipeline*>::iterator result;
    {
        std::lock_guard<std::mutex> lk(connMutex);
        result = connMap.find(address);
    }

    if (result != connMap.end()) {
        CubeClientPipeline* conn = result->second;
        // TODO: check if the connection is alive or not.
        return conn;
    }

    std::string::size_type  pos = address.find(':');
    if (pos != std::string::npos) {
        CubeClientPipeline* conn = connect(address.substr(0, pos),
                                           std::stoul(address.substr(pos + 1)));
        if (nullptr != conn) {
            std::lock_guard<std::mutex> lk(connMutex);
            connMap[address] = conn;
        }
    }

    return nullptr;
}

CubeClientPipeline* CubeClient::connect(const std::string &host, uint16_t port, long timeout) {
    auto future = client.connect(SocketAddress(host, port));
    return future.get(Duration(timeout));
}

void CubeClient::submit(const std::string &param, wangle::HandlerContext* ctx) {
    std::lock_guard<std::mutex> lk(taskMutex);
    auto result = tasks.find(param);
    if (result == tasks.end()) {
        tasks[param] = ctx;
    } else {
        // Never goes here according to the competition protocol.
        throw "ParamDuplicated";
    }
}