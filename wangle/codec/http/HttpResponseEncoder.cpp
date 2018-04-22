#include <folly/io/IOBuf.h>
#include "HttpResponseEncoder.h"

using namespace wangle;

std::unique_ptr<folly::IOBuf> HttpResponseEncoder::encode(wangle::HttpResponse &msg) {

    std::string responseLine("HTTP/");
    switch (msg.version()) {
        case HTTP_1_0:
            responseLine += "1.0";
            break;
        case HTTP_1_1:
            responseLine += "1.1";
            break;
        case HTTP_2_0:
            responseLine += "2.0";

        default:
            responseLine += "Unknown";
            break;
    }

    responseLine += " " + std::to_string(msg.status()) + " OK" + HttpResponse::DELIMITER;


    std::string headerLines;
    std::unordered_map<std::string, std::string> headers = msg.headers();
    for(auto it = headers.begin(); it != headers.end(); it++) {
        headerLines += it->first;
        headerLines += ": ";
        headerLines += it->second;
        headerLines += HttpResponse::DELIMITER;
    }

    std::string response(responseLine);
    response += headerLines;
    response += HttpResponse::DELIMITER;
    response += msg.body();

    std::unique_ptr<folly::IOBuf> buf = folly::IOBuf::copyBuffer(response.data(), response.length());
    return std::move(buf);
}
