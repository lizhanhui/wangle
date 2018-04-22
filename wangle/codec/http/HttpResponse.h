#ifndef WANGLE_HTTP_RESPONSE_H
#define WANGLE_HTTP_RESPONSE_H

#include <wangle/codec/http/HttpVersion.h>
#include <string>
#include <unordered_map>

namespace wangle {
    class HttpResponse {

    public:
        HttpResponse();

        int status() {
            return statusCode;
        }

        HttpVersion version() {
            return version_;
        }

        void version(HttpVersion v) {
            version_ = v;
        }

        void putHeader(const std::string &key, const std::string &value) {
            headers_[key] = value;
        }

        std::unordered_map<std::string, std::string>& headers() {
            return headers_;
        }

        void setBody(const std::string &body) {
            putHeader("Content-Length", std::to_string(body.length()));
            body_ = body;
        }

        std::string &body() {
            return body_;
        }

        static const std::string DELIMITER;

    private:
        int statusCode;
        HttpVersion version_;
        std::unordered_map<std::string, std::string> headers_;
        std::string body_;

    };
}


#endif //WANGLE_HTTP_RESPONSE_H
