#ifndef WANGLE_CUBE_CLIENT_H
#define WANGLE_CUBE_CLIENT_H

#include <wangle/bootstrap/ClientBootstrap.h>
#include <wangle/channel/AsyncSocketHandler.h>
#include <wangle/channel/EventBaseHandler.h>
#include <wangle/channel/Handler.h>

#include <unordered_map>
#include <mutex>
#include <thread>


typedef wangle::Pipeline<folly::IOBufQueue&, std::string> CubeClientPipeline;

class CubeClient {
public:
    CubeClient();

    void start();

    void process();

    void stop();

    CubeClientPipeline* connect(const std::string& host, uint16_t port, long timeout = 3000);

    CubeClientPipeline* getConnection(const std::string& address);

    void submit(const std::string &param, wangle::HandlerContext* ctx);

private:
    wangle::ClientBootstrap<CubeClientPipeline> client;
    std::unordered_map<std::string, CubeClientPipeline*> connMap;
    std::mutex connMutex;

    std::unordered_map<std::string, wangle::HandlerContext*> tasks;
    std::mutex taskMutex;

    bool stopped;

    std::vector<std::thread> workers;
};

class CubeClientHandler : public wangle::HandlerAdapter<std::string> {
public:
    void read(Context* ctx, std::string msg) override;
};

class CubeClientPipelineFactory : wangle::PipelineFactory<CubeClientPipeline> {
public:
    CubeClientPipeline::Ptr
    newPipeline(std::shared_ptr<folly::AsyncTransportWrapper> socket) override;
};

#endif //WANGLE_CUBE_CLIENT_H
