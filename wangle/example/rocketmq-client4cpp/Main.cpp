#include <iostream>
#include <string>
#include <folly/json.h>
#include <folly/dynamic.h>
#include <unordered_map>
#include "remoting/RemotingClient.h"

using namespace rocketmq;

int main(int argc, char** argv) {

    RemotingClient client;
    client.start();
    std::string address = "127.0.0.1:9876";
    std::string topic("TopicTest");
    std::shared_ptr<RemotingCommandCustomHeader> header = std::make_shared<GetRouteInfoRequestHeader>(topic);
    std::shared_ptr<RemotingCommand> command = RemotingCommand::createRequestCommand(105, header);
    client.invoke(address, command, 300000);

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