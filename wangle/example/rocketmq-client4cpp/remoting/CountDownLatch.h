#ifndef PROJECT_COUNTDOWNLATCH_H
#define PROJECT_COUNTDOWNLATCH_H

#include <atomic>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

namespace rocketmq {
    class CountDownLatch {
    public:
        CountDownLatch(int count) : count(count) {

        }

        void await() {
            if (count.load() <= 0) {
                return;
            } else {
                std::unique_lock<std::mutex> lk(m);
                cv.wait(lk, [&](){return count.load() <= 0;});
                lk.unlock();
                cv.notify_one();
            }
        }

        void await(unsigned long timeout) {
            if (count.load() <= 0) {
                return;
            } else {
                std::unique_lock<std::mutex> lk(m);
                cv.wait_for(lk, std::chrono::milliseconds(timeout), [&](){
                    return count.load() <= 0;
                });

                lk.unlock();
                cv.notify_one();
            }
        }

        void countDown() {
            count--;
            if (count.load() <= 0) {
                cv.notify_all();
            }
        }

    private:
        std::atomic_int count;
        std::mutex m;
        std::condition_variable cv;
    };
}

#endif //PROJECT_COUNTDOWNLATCH_H
