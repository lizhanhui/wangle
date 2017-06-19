#ifndef PROJECT_REMOTING_H
#define PROJECT_REMOTING_H

#include <atomic>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <folly/dynamic.h>
#include <folly/json.h>
#include "RemotingCommandCustomHeader.h"

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

        template <typename T>
        std::unique_ptr<folly::IOBuf> prepend(std::unique_ptr<folly::IOBuf>&& buf, T t) {
            int len = sizeof(T);
            unsigned char data[len];
            for (int i = 0; i < len; ++i) {
                data[i] = (unsigned char)((t >> 8 *(len - i - 1)) & 0xFF);
            }
            std::unique_ptr<folly::IOBuf> buffer = folly::IOBuf::wrapBuffer(data, len);
            buffer->appendChain(std::move(buf));
            return buffer;
        }

        std::unique_ptr<folly::IOBuf> encode() {
            int length = 4;
            std::unique_ptr<folly::IOBuf> header = encodeHeader();
            int headerLength = (int) header->length();
            length += headerLength;
            if (nullptr != body) {
                length += body->length();
                header->appendChain(std::move(body));
            }

            unsigned char headerMeta[4];
            headerMeta[0] = serializationType;
            headerMeta[1] = (unsigned char) ((headerLength >> 16) & 0xFF);
            headerMeta[2] = (unsigned char) ((headerLength >> 8) & 0xFF);
            headerMeta[3] = (unsigned char) ((headerLength >> 0) & 0xFF);

            std::unique_ptr<folly::IOBuf> headerBuf = folly::IOBuf::wrapBuffer(headerMeta, 4);
            headerBuf->appendChain(std::move(header));
            return prepend(std::move(headerBuf), length);
        }

        std::unique_ptr<folly::IOBuf> encodeHeader() {
            if (nullptr != customHeader) {
                customHeader->toNet();
                switch(serializationType) {
                    case JSON:
                        folly::dynamic map = folly::dynamic::object();
                        for (auto it = customHeader->extFields.begin(); it != customHeader->extFields.end(); it++) {
                            map[it->first] = it->second;
                        }
                        std::string json = folly::toJson(map);
                        return folly::IOBuf::copyBuffer(json.data(), json.length());
                    case ROCKETMQ:
                        // TODO: Manually serialize header map.
                        break;
                }
            }

            return nullptr;
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

        friend class RemotingEncoder;
        friend class RemotingDecoder;
    };
}


#endif //PROJECT_REMOTING_H
