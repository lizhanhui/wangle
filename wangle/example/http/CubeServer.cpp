#include <gflags/gflags.h>

#include <wangle/bootstrap/ServerBootstrap.h>
#include <wangle/codec/LengthFieldBasedFrameDecoder.h>
#include <wangle/channel/AsyncSocketHandler.h>
#include <wangle/codec/StringCodec.h>

using namespace folly;
using namespace wangle;

DEFINE_int32("port", 1234, "CubeServer Port");
typedef Pipeline<IOBufQueue&, std::string> CubePipeline;

class DubboHandler : public HandlerAdapter<std::string> {
public:
    void read(Context* ctx, std::string msg) override {
        // invoke Dubbo here.
    }
};

class CubePipelineFactory : public PipelineFactory<CubePipeline> {
public:
    CubePipeline::Ptr newPipeline(std::shared_ptr<AsyncTransportWrapper> socket) {
        auto pipeline = CubePipeline::create();
        pipeline->addBack(AsyncSocketHandler(socket));
        pipeline->addBack(LengthFieldBasedFrameDecoder());
        pipeline->addBack(StringCodec());
        pipeline->addBack(DubboHandler());
        pipeline->finalize();
        return pipeline;
    }
};

class CubeServer {
public:
    void launch() {
        server.group(std::make_shared<IOThreadPoolExecutor>(1),
                     std::make_shared<IOThreadPoolExecutor>(std::thread::hardware_concurrency()));
        server.childPipeline(std::make_shared<CubePipelineFactory>());
        server.bind(FLAGS_port);
        server.waitForStop();
    }

    void stop() {
        server.stop();
    }

private:
    ServerBootstrap<CubePipeline> server;
};