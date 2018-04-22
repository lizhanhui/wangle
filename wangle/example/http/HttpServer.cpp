#include <gflags/gflags.h>

#include <folly/init/Init.h>
#include <wangle/bootstrap/ServerBootstrap.h>
#include <wangle/codec/http/HttpRequestDecoder.h>
#include <wangle/codec/http/HttpResponseEncoder.h>
#include <wangle/channel/AsyncSocketHandler.h>

using namespace folly;
using namespace wangle;

DEFINE_int32(port, 8080, "HTTP Server listen port");

typedef Pipeline<IOBufQueue&, HttpRequest> HttpPipeline;

class HttpHandler : public HandlerAdapter<HttpRequest, HttpResponse> {
public:
    void read(Context* ctx, HttpRequest request) override {
        HttpResponse response;
        std::string body = "Hello World!";
        response.setBody(body);
        response.putHeader("Content-Type", "text/plain");
        write(ctx, response).then([this, ctx]() {
            close(ctx);
        });
    }

    void readException(Context *ctx, exception_wrapper e) override {
        close(ctx);
        std::cout << e.what() << std::endl;
    }

    Future<Unit> write(Context *ctx, HttpResponse response) override {
        return ctx->fireWrite(response);
    }
};

class HttpPipelineFactory : public PipelineFactory<HttpPipeline> {
public:
    HttpPipeline::Ptr newPipeline(std::shared_ptr<AsyncTransportWrapper> socket) override {
        auto pipeline = HttpPipeline::create();
        pipeline->addBack(AsyncSocketHandler(socket));
        pipeline->addBack(HttpRequestDecoder());
        pipeline->addBack(HttpResponseEncoder());
        pipeline->addBack(HttpHandler());
        pipeline->finalize();
        return pipeline;
    }
};

int main(int argc, char* argv[]) {
    folly::Init init(&argc, &argv);

    ServerBootstrap<HttpPipeline> server;
    server.childPipeline(std::make_shared<HttpPipelineFactory>());
    server.bind(FLAGS_port);
    server.waitForStop();
    return 0;
}
