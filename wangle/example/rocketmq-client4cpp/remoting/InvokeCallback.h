#ifndef PROJECT_INVOKECALLBACK_H
#define PROJECT_INVOKECALLBACK_H

#include <folly/futures/Future.h>

namespace rocketmq {

    class InvokeCallback {
    public:
        virtual void onSuccess() = 0;
        virtual void onError() = 0;
    };
}

#endif //PROJECT_INVOKECALLBACK_H
