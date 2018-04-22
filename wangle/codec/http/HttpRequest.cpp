#include "HttpRequest.h"

using namespace wangle;

HttpRequest::HttpRequest(): method_(INVALID), version_(UNKNOWN) {

}

void HttpRequest::setMethod(std::string &m) {
    if ("GET" == m) {
        method_ = GET;
        return;
    }

    if ("POST" == m) {
        method_ = POST;
        return;
    }

    if ("PUT" == m) {
        method_ = PUT;
        return;
    }

    if ("HEAD" == m) {
        method_ = HEAD;
        return;
    }

    if ("DELETE" == m) {
        method_ = DELETE;
        return;
    }

    method_ = INVALID;
}

void HttpRequest::setVersion(std::string &v) {
    if ("HTTP/1.0" == v) {
        version_ = HTTP_1_0;
        return;
    }

    if ("HTTP/1.1" == v) {
        version_ = HTTP_1_1;
        return;
    }

    if ("HTTP/2.0" == v) {
        version_ = HTTP_2_0;
        return;
    }
}