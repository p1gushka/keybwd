#include "handlers/quote_handler.hpp"
#include <Poco/JSON/Object.h>
#include <Poco/JSON/JSONWriter.h>
#include <iostream>

using namespace Poco::Net;
using namespace Poco::JSON;
using namespace server;

void QuoteHandler::handleRequest(HTTPServerRequest& req, HTTPServerResponse& resp) {
    resp.setContentType("application/json");
    std::string text = db_.get_random_text();

    Object result;
    result.set("text", text.empty() ? "" : text);

    resp.setStatus(HTTPResponse::HTTP_OK);
    std::ostream& out = resp.send();
    JSONWriter writer(out);
    result.stringify(writer);
}
