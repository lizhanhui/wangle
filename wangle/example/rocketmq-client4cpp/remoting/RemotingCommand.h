#ifndef PROJECT_REMOTING_H
#define PROJECT_REMOTING_H

#include <atomic>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <folly/dynamic.h>
#include <folly/json.h>
#include "RemotingCommandCustomHeader.h"
#include "Helper.h"

namespace rocketmq {

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
        RemotingCommand() : code(0), language(LanguageCode::CPP), version(0), opaque(0), flag(0), serializationType(SerializationType::JSON) {}

        RemotingCommand(int code, std::shared_ptr<RemotingCommandCustomHeader> header) : code(code), customHeader(header) {
            opaque = requestId.fetch_add(1);
            language = LanguageCode::CPP;
        }

        static RemotingCommand createRequestCommand(int code, std::shared_ptr<RemotingCommandCustomHeader> header) {
            return RemotingCommand(code, header);
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

        friend class RemotingEncoder;
        friend class RemotingDecoder;
    };
}


#endif //PROJECT_REMOTING_H
