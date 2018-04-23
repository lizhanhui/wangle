#include <vector>
#include <gflags/gflags.h>

#include <folly/init/Init.h>
#include <folly/String.h>
#include <wangle/bootstrap/ServerBootstrap.h>
#include <wangle/codec/http/HttpRequestDecoder.h>
#include <wangle/codec/http/HttpResponseEncoder.h>
#include <wangle/channel/AsyncSocketHandler.h>

#include "CubeClient.h"

using namespace folly;
using namespace wangle;

DEFINE_int32(port, 8080, "HTTP Server listen port");

typedef Pipeline<IOBufQueue&, HttpRequest> HttpPipeline;



class HttpHandler : public HandlerAdapter<HttpRequest, HttpResponse> {
public:

    HttpHandler(std::shared_ptr<CubeClient> cubeClient) : cubeClient_(cubeClient) {

    }

    void read(Context* ctx, HttpRequest request) override {

        std::string contentType = request.getHeader("Content-Type");
        if (contentType == "application/x-www-form-urlencoded") {
            request.body()->coalesce();
            std::string body((const char*)request.body()->data(), request.body()->length());
            std::vector<std::string> params;
            folly::split("&", body, params, true);
            for(std::string &item : params) {
                auto separator = item.find('=');
                if (separator == std::string::npos) {
                    continue;
                }

                if ("parameter" == item.substr(0, separator)) {
                    cubeClient_->submit(item.substr(separator + 1), ctx);
                    break;
                }
            }
        }
    }

    void readException(Context *ctx, exception_wrapper e) override {
        close(ctx);
    }
private:
    std::shared_ptr<CubeClient> cubeClient_;
};

class HttpPipelineFactory : public PipelineFactory<HttpPipeline> {
public:

    HttpPipelineFactory(std::shared_ptr<CubeClient> cubeClient) : cubeClient_(cubeClient) {

    }

    HttpPipeline::Ptr newPipeline(std::shared_ptr<AsyncTransportWrapper> socket) override {
        auto pipeline = HttpPipeline::create();
        pipeline->addBack(AsyncSocketHandler(socket));
        pipeline->addBack(HttpRequestDecoder());
        pipeline->addBack(HttpResponseEncoder());
        pipeline->addBack(HttpHandler(cubeClient_));
        pipeline->finalize();
        return pipeline;
    }

private:
    std::shared_ptr<CubeClient> cubeClient_;
};


int main(int argc, char* argv[]) {
    folly::Init init(&argc, &argv);

    ServerBootstrap<HttpPipeline> server;

    std::shared_ptr<CubeClient> cubeClient;
    cubeClient->start();

    server.childPipeline(std::make_shared<HttpPipelineFactory>(cubeClient));
    server.bind(FLAGS_port);
    server.waitForStop();
    return 0;
}
