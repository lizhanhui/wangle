#include "RemotingCommand.h"

std::unique_ptr<folly::IOBuf> rocketmq::RemotingCommand::encode() {
    int length = 4;
    std::unique_ptr<folly::IOBuf> header = encodeHeader();
    int headerLength = (nullptr == header) ? 0 : (int) header->length();
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

    std::unique_ptr<folly::IOBuf> headerBuf = folly::IOBuf::copyBuffer(headerMeta, 4);
    headerBuf->appendChain(std::move(header));
    return Helper::prepend(std::move(headerBuf), length);
}

std::unique_ptr<rocketmq::RemotingCommand> rocketmq::RemotingCommand::decode(std::unique_ptr<folly::IOBuf> buf) {
    return nullptr;
}

std::unique_ptr<folly::IOBuf> rocketmq::RemotingCommand::encodeHeader() {
    if (nullptr != customHeader) {
        customHeader->toNet(this);
    }

    switch(serializationType) {
        case JSON:
        {
            folly::dynamic map = folly::dynamic::object();
            for (auto it = extFields.begin(); it != extFields.end(); it++) {
                map[it->first] = it->second;
            }

            map["code"] = code;
            map["language"] = 1;
            map["version"] = 0;
            map["opaque"] = opaque;
            map["flag"] = flag;
            map["remark"] = remark;

            std::string json = folly::toJson(map);
            return folly::IOBuf::copyBuffer(json.data(), json.length());
        }

        case ROCKETMQ:
        {
            break;
        }
    }

    return nullptr;
}

std::unique_ptr<rocketmq::RemotingCommandCustomHeader> rocketmq::RemotingCommand::decodeHeader(
        std::unique_ptr<folly::IOBuf> buf) {
    return nullptr;
}

void rocketmq::GetRouteInfoRequestHeader::toNet(RemotingCommand *command) {
    command->addExtField("topic", topic);
}

std::atomic_int rocketmq::RemotingCommand::requestId = ATOMIC_VAR_INIT(0);