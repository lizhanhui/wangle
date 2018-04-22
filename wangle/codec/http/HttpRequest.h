#ifndef WANGLE_HTTP_REQUEST_H
#define WANGLE_HTTP_REQUEST_H

#include <string>
#include <unordered_map>
#include <folly/io/IOBuf.h>
#include <wangle/codec/http/HttpVersion.h>

namespace wangle {
    class HttpRequest {
    public:

        enum Method {
            INVALID, GET, POST, HEAD, PUT, DELETE
        };

        HttpRequest();

        Method method() {
            return method_;
        }

        void setMethod(std::string& m);

        HttpVersion version() {
            return version_;
        }

        void setVersion(std::string& v);

        std::string uri() {
            return uri_;
        }

        void setUri(std::string& uri) {
            uri_ = uri;
        }

        void put(const std::string &key, const std::string &value) {
            headers[key] = value;
        }

        std::string getHeader(const std::string &key) {
            auto result = headers.find(key);
            if (result != headers.end()) {
                return result->second;
            }
            return "";
        }

        void setBody(std::unique_ptr<folly::IOBuf> &body) {
            body_ = std::move(body);
        }

        std::unique_ptr<folly::IOBuf>& body() {
            return body_;
        }

    private:
        Method method_;
        HttpVersion version_;
        std::string uri_;
        std::unordered_map<std::string, std::string> headers;
        std::unique_ptr<folly::IOBuf> body_;
    };
}


#endif //WANGLE_HTTP_REQUEST_H
