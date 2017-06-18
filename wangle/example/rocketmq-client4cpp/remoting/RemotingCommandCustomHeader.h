#ifndef PROJECT_COMMANDCUSTOMHEADER_H
#define PROJECT_COMMANDCUSTOMHEADER_H

#include <memory>
#include <unordered_map>
#include <folly/io/IOBufQueue.h>

namespace rocketmq {
    class RemotingCommandCustomHeader {
    public:
        RemotingCommandCustomHeader() : extFields(8) {}

        virtual void toNet()  = 0;

    private:
        std::unordered_map<std::string, std::string> extFields;
        friend class RemotingCommand;
    };
}

#endif //PROJECT_COMMANDCUSTOMHEADER_H
