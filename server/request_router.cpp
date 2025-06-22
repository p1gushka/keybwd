#include "request_router.hpp"
#include "handlers/quote_handler.hpp"
#include <Poco/Net/HTTPServerRequest.h>
#include <string>

using namespace Poco::Net;
using namespace server;

HTTPRequestHandler* RequestRouter::createRequestHandler(const HTTPServerRequest& req) {
    const std::string path = req.getURI();
    if (req.getMethod() == HTTPRequest::HTTP_GET && path == "/quote") {
        return new QuoteHandler(db_);
    }
    // TODO: Добавить AuthHandler, StatsHandler и другие порты
    return nullptr;
}
