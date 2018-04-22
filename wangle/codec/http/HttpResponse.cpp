#include "HttpResponse.h"

using namespace wangle;

HttpResponse::HttpResponse() : statusCode(200), version_(HTTP_1_1) {
}

const std::string HttpResponse::DELIMITER = "\r\n";