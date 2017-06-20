#include "RemotingCommand.h"

std::unique_ptr<folly::IOBuf> rocketmq::RemotingCommand::encode() {
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
    return Helper::prepend(std::move(headerBuf), length);
}

std::unique_ptr<rocketmq::RemotingCommand> rocketmq::RemotingCommand::decode(std::unique_ptr<folly::IOBuf> buf) {

}

std::unique_ptr<folly::IOBuf> rocketmq::RemotingCommand::encodeHeader() {
    {
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
}

std::unique_ptr<rocketmq::RemotingCommandCustomHeader> rocketmq::RemotingCommand::decodeHeader(
        std::unique_ptr<folly::IOBuf> buf) {

}