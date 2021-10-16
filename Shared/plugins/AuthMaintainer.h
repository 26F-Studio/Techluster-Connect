//
// Created by Particle_G on 2021/9/10.
//

#pragma once

#include <drogon/HttpController.h>
#include <drogon/plugins/Plugin.h>

namespace tech::plugins {
    class AuthMaintainer : public drogon::Plugin<AuthMaintainer> {
    public:
        void initAndStart(const Json::Value &config) override;

        void shutdown() override;

        [[nodiscard]] std::string getReportAddress() const;

        drogon::HttpStatusCode checkAccessToken(const std::string &accessToken, int64_t &id);

    private:
        std::chrono::duration<double> _taskMinutes{};
        std::atomic<trantor::InetAddress> _connectAddress{}, _authAddress{};

        void _updateAuthAddress();
    };
}