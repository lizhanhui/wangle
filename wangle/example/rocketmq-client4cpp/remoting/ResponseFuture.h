#ifndef PROJECT_RESPONSEFUTURE_H
#define PROJECT_RESPONSEFUTURE_H

#include <cstdint>
#include <atomic>
#include "InvokeCallback.h"

namespace rocketmq {

    class ResponseFuture {
    public:
        ResponseFuture(int opaque, int64_t timeoutMillis, const InvokeCallback& callback)
                : opaque(opaque), timeoutMillis(timeoutMillis), callback(callback) {
            executeCallbackOnlyOnce = false;
        }

    private:
        int opaque;
        int64_t timeoutMillis;
        const InvokeCallback& callback;

        std::atomic_bool executeCallbackOnlyOnce;

    };
}

#endif //PROJECT_RESPONSEFUTURE_H
