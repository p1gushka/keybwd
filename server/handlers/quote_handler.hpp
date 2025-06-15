#pragma once
#include <Poco/Net/HTTPRequestHandler.h>
#include "../../database/text_database.hpp"

namespace server {
class QuoteHandler : public Poco::Net::HTTPRequestHandler {
public:
    explicit QuoteHandler(TextDatabase& db) : db_(db) {}
    void handleRequest(Poco::Net::HTTPServerRequest& req,
                       Poco::Net::HTTPServerResponse& resp) override;

private:
    TextDatabase& db_;
};
}
