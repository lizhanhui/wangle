#include <gflags/gflags.h>

#include <folly/init/Init.h>
#include <wangle/bootstrap/ClientBootstrap.h>
#include <wangle/codec/http/HttpRequestEncoder.h>
#include <wangle/codec/http/HttpResponseDecoder.h>
#include <wangle/channel/AsyncSocketHandler.h>
#include <wangle/channel/EventBaseHandler.h>


DEFINE_int32(port, 8080, "HTTP Server Listen Port");
DEFINE_string(host, "localhost", "HTTP Server Host");


using namespace folly;
using namespace wangle;

typedef Pipeline<IOBufQueue, HttpResponse> HttpPipeline;

class HttpHandler : public HandlerAdapter<HttpResponse> {
public:

    void read(Context* context, HttpResponse response) override {

    }

};

class HttpPipelineFactory : public PipelineFactory<HttpPipeline> {
    HttpPipeline::Ptr newPipeline(std::shared_ptr<AsyncTransportWrapper> socket) {
        auto pipeline = HttpPipeline::create();
        pipeline->addBack(AsyncSocketHandler(socket));
        pipeline->addBack(EventBaseHandler());
        pipeline->addBack(HttpResponseDecoder());
        pipeline->addBack(HttpRequestEncoder());
        pipeline->addBack(HttpHandler());
        pipeline->finalize();
        return pipeline;
    }
};

int main(int argc, char* argv[]) {

    folly::Init init(&argc, &argv);
    ClientBootstrap<HttpPipeline> client;
    client.group(std::make_shared<IOThreadPoolExecutor>(1));
    client.pipelineFactory(std::make_shared<HttpPipelineFactory>());
    auto pipeline = client.connect(SocketAddress(FLAGS_host, FLAGS_port));

    // Begin to send request

}