#pragma once
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include "../database/text_database.hpp"

namespace server {

class RequestRouter : public Poco::Net::HTTPRequestHandlerFactory {
public:
    explicit RequestRouter(TextDatabase& db) : db_(db) {}
    Poco::Net::HTTPRequestHandler* createRequestHandler(
        const Poco::Net::HTTPServerRequest& req) override;

private:
    TextDatabase& db_;
};

}
