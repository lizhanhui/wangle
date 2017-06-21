#ifndef PROJECT_RESPONSEFUTURE_H
#define PROJECT_RESPONSEFUTURE_H

#include <cstdint>
#include <atomic>
#include "InvokeCallback.h"
#include "RemotingCommand.h"
#include "CountDownLatch.h"

namespace rocketmq {

    class ResponseFuture {
    public:
        ResponseFuture(int opaque, unsigned long timeoutMillis, std::shared_ptr<InvokeCallback> callback)
                : opaque(opaque), timeoutMillis(timeoutMillis), callback(callback), countDownLatch(1),
                  executeCallbackOnlyOnce(false), success(false) {
        }

        void setResult(std::shared_ptr<RemotingCommand> command) {
            result = command;
            success.store(true);
        }

        void await() {
            countDownLatch.await(timeoutMillis);
        }

        void countDown() {
            countDownLatch.countDown();
        }

        bool isSuccess() {
            return success.load();
        }

        std::shared_ptr<RemotingCommand> getResult() {
            return result;
        }

        std::shared_ptr<InvokeCallback> getCallback() {
            return callback;
        }

    private:
        int opaque;
        unsigned long timeoutMillis;
        std::shared_ptr<InvokeCallback> callback;
        std::shared_ptr<RemotingCommand> result;
        std::atomic_bool executeCallbackOnlyOnce;
        std::atomic_bool success;

        CountDownLatch countDownLatch;

    };
}

#endif //PROJECT_RESPONSEFUTURE_H
