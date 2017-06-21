#ifndef PROJECT_REMOTING_H
#define PROJECT_REMOTING_H

#include <atomic>
#include <cstdint>
#include <string>
#include <memory>
#include <unordered_map>
#include <folly/dynamic.h>
#include <folly/json.h>
#include "Helper.h"

namespace rocketmq {

    class RemotingCommand;

    class RemotingCommandCustomHeader {
    public:
        RemotingCommandCustomHeader() {}

        virtual void toNet(RemotingCommand* command) = 0;
    };

    class GetRouteInfoRequestHeader : public RemotingCommandCustomHeader {
    public:
        GetRouteInfoRequestHeader(std::string& topic) : topic(topic) {
        }

        void toNet(RemotingCommand* command);

    private:
        std::string topic;
    };

    enum LanguageCode : unsigned char {
        JAVA = 0,
        CPP = 1,
        DOTNOT = 2,
        PYTHON = 3,
        DELPHI = 4,
        ERLANG = 5,
        RUBY = 6,
        OTHER = 7,
        HTTP = 8,
        GO = 9,
        PHP = 10
    };

    enum SerializationType : unsigned char {
        JSON = 0,
        ROCKETMQ = 1
    };

    class RemotingCommand {

    public:
        RemotingCommand() : code(0), language(LanguageCode::CPP), version(0), opaque(0), flag(0),
                            serializationType(SerializationType::JSON), extFields(16) {

        }

        RemotingCommand(int code, std::shared_ptr<RemotingCommandCustomHeader> header) : code(code),
                                                                                         customHeader(header),
                                                                                         extFields(16) {
            opaque = requestId.fetch_add(1);
            language = LanguageCode::CPP;
        }

        static std::shared_ptr<RemotingCommand> createRequestCommand(int code, std::shared_ptr<RemotingCommandCustomHeader> header) {
            return std::make_shared<RemotingCommand>(code, header);
        }

        void setCode(int code) {
            this->code = code;
        }

        std::shared_ptr<RemotingCommandCustomHeader> getCustomHeader() {
            return customHeader;
        }

        void setCustomHeader(std::shared_ptr<RemotingCommandCustomHeader> header) {
            customHeader = header;
        }

        int getOpaque() const {
            return opaque;
        }

        std::unique_ptr<folly::IOBuf> encode();

        static std::unique_ptr<RemotingCommand> decode(std::unique_ptr<folly::IOBuf> buf);

        std::unique_ptr<folly::IOBuf> encodeHeader();

        static std::unique_ptr<RemotingCommandCustomHeader> decodeHeader(std::unique_ptr<folly::IOBuf> buf);

        void addExtField(std::string&& key, std::string value) {
            extFields[key] = value;
        }

        bool isResponse() {
            int bits = 1;
            return (bits & flag) == bits;
        }

    private:
        static std::atomic_int requestId;
        int code;
        LanguageCode language;
        int version;
        int opaque;
        int flag;
        std::string remark;
        std::shared_ptr<RemotingCommandCustomHeader> customHeader;
        SerializationType serializationType;
        std::unique_ptr<folly::IOBuf> body;
        std::unordered_map<std::string, std::string> extFields;
        friend class RemotingEncoder;
        friend class RemotingDecoder;
    };
}


#endif //PROJECT_REMOTING_H
