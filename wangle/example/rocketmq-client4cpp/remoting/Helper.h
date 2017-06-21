#ifndef PROJECT_HELPER_H
#define PROJECT_HELPER_H

#include <folly/io/IOBuf.h>

namespace rocketmq {
    class Helper {
    public:
        // Big endian is assumed.
        template <typename T>
        static std::unique_ptr<folly::IOBuf> prepend(std::unique_ptr<folly::IOBuf>&& buf, T t);
    };

    template <typename T>
    std::unique_ptr<folly::IOBuf> Helper::prepend(std::unique_ptr <folly::IOBuf> &&buf, T t) {
        int len = sizeof(T);
        unsigned char data[len];
        for (int i = 0; i < len; ++i) {
            data[i] = (unsigned char)((t >> (8 * (len - i - 1))) & 0xFF);
        }
        std::unique_ptr<folly::IOBuf> buffer = folly::IOBuf::copyBuffer(data, len);
        buffer->appendChain(std::move(buf));
        return buffer;
    }
}

#endif //PROJECT_HELPER_H
