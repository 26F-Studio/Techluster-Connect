//
// Created by Particle_G on 2021/8/19.
//

#include <controllers/Monitor.h>
#include <helpers/ResponseJson.h>
#include <magic_enum.hpp>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::api::v2;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;

Monitor::Monitor() :
        ResponseJsonHandler(
                [](const ResponseException &e, ResponseJson &response) {
                    response.setStatusCode(e.statusCode());
                    // TODO: Check if this causes too much copying
                    response(e.toJson());
                },
                [this](const orm::DrogonDbException &e, ResponseJson &response) {
                    LOG_ERROR << e.base().what();
                    response.setStatusCode(k500InternalServerError);
                    response.setResultCode(ResultCode::databaseError);
                    response.setMessage(i18n("databaseError"));
                },
                [this](const exception &e, ResponseJson &response) {
                    LOG_ERROR << e.what();
                    response.setStatusCode(k500InternalServerError);
                    response.setResultCode(ResultCode::internalError);
                    response.setMessage(i18n("internalError"));
                    response.setReason(e);
                }
        ),
        I18nHelper(CMAKE_PROJECT_NAME),
        _nodeManager(app().getPlugin<NodeManager>()),
        _perfmon(app().getPlugin<Perfmon>()) {}

void Monitor::getInfo(const HttpRequestPtr &req, function<void(const HttpResponsePtr &)> &&callback) {
    ResponseJson response;
    handleExceptions([&]() {
        auto nodeType = req->attributes()->get<NodeType>("nodeType");
        if (nodeType == NodeType::connect) {
            response.setData(_perfmon->parseInfo());
        } else if (nodeType == NodeType::all) {
            response.setData(_nodeManager->parseInfo());
        } else {
            response.setData(_nodeManager->parseInfo(nodeType));
        }
    }, response);
    response.httpCallback(callback, "*");

    // TODO: Move this into NodeManager
    // response.setStatusCode(k406NotAcceptable);
    // response.setResultCode(ResultCode::invalidArguments);
    // response.setMessage(i18n("notAvailable."s.append(enum_name(nodeType))));
    // response.setReason(e);
}
