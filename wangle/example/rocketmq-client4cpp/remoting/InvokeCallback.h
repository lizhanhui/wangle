#ifndef PROJECT_INVOKECALLBACK_H
#define PROJECT_INVOKECALLBACK_H

#include <folly/futures/Future.h>

namespace rocketmq {

    class InvokeCallback {
        virtual void operationComplete(folly::Future<RemotingCommand>& future) = 0;
    };
}

#endif //PROJECT_INVOKECALLBACK_H
