#include <iostream>
#include <string>
#include <folly/json.h>
#include <folly/dynamic.h>
#include <unordered_map>
#include "remoting/RemotingClient.h"
#include "remoting/CountDownLatch.h"

using namespace rocketmq;

int main(int argc, char** argv) {

    std::mutex m;

    CountDownLatch countDownLatch(1);
    std::thread t([&]() -> void {
        m.lock();
        std::cout << "Lambda is to await" << std::endl;
        m.unlock();

        countDownLatch.await();
        std::cout << "Lambda awaits OK" << std::endl;
    });

    std::thread t2([&]{
        m.lock();
        std::cout << "Lambda2 is to timed await" << std::endl;
        m.unlock();

        countDownLatch.await(500);
        std::cout << "Lambda2 timed await OK" << std::endl;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "Main is to count down" << std::endl;
    countDownLatch.countDown();

    t2.join();
    t.join();

//    RemotingClient client;
//    client.start();
//    std::string address = "127.0.0.1:9876";
//    std::string topic("TopicTest");
//    std::shared_ptr<RemotingCommandCustomHeader> header = std::make_shared<GetRouteInfoRequestHeader>(topic);
//    std::shared_ptr<RemotingCommand> command = RemotingCommand::createRequestCommand(105, header);
//    client.invoke(address, command, 300000);

//    std::unordered_map<std::string, std::string> m;
//
//    m["Hello"] = "World";
//
//    folly::dynamic map = folly::dynamic::object;
//
//    for (auto it = m.begin(); it != m.end(); it++) {
//        map.insert(it->first, it->second);
//    }
//
//    std::string s = folly::toJson(map);
//    std::cout << s << std::endl;

}