//
// Created by ParticleG on 2022/2/4.
//

#include <drogon/drogon.h>
#include <helpers/BasicJson.h>
#include <helpers/ResponseJson.h>
#include <magic_enum.hpp>
#include <plugins/BaseTester.h>
#include <structures/Exceptions.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;

void WebHelper::colorOut(const std::string &msg, const Color &color) {
    cout << "\x1b[" << enum_integer(color) << "m"
         << string(16, '-')
         << msg
         << string(48 - msg.length(), '-')
         << "\x1b[" << enum_integer(Color::none) << "m"
         << endl;
}

Json::Value WebHelper::_httpRequest(
        const HttpMethod &method,
        const string &path,
        const unordered_map<string, string> &parameters,
        const unordered_map<string, string> &headers,
        const Json::Value &body
) {
    auto client = HttpClient::newHttpClient("http://"s.append(_host));
    auto req = HttpRequest::newHttpRequest();
    if (!body.isNull()) {
        req = toRequest(body);
    }
    req->setMethod(method);
    req->setPath(_httpBase + path);
    if (!parameters.empty()) {
        for (const auto &[key, value]: parameters) {
            req->setParameter(key, value);
        }
    }
    if (!headers.empty()) {
        for (const auto &[key, value]: headers) {
            req->addHeader(key, value);
        }
    }
    auto[result, responsePtr] = client->sendRequest(req, 180);
    if (result != ReqResult::Ok) {
        throw NetworkException("Node is down", result);
    }
    try {
        ResponseJson response(responsePtr);
    } catch (const json_exception::InvalidFormat &e) {
        LOG_WARN << "Invalid response body (" << responsePtr->getStatusCode() << "): \n"
                 << e.what();
    }
    try {
        ResponseJson response(responsePtr);
        if (!response["code"].isUInt()) {
            throw NetworkException(
                    "Request failed: "s.append(response.stringify()),
                    ReqResult::BadResponse
            );
        }
        LOG_DEBUG << "Response Json: " << response.stringify("  ");
        return response.ref();
    } catch (const json_exception::InvalidFormat &e) {
        throw NetworkException("Invalid Json: "s.append(e.what()), ReqResult::BadResponse);
    }
}
