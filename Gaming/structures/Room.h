//
// Created by particleg on 2021/10/8.
//

#pragma once

#include <drogon/drogon.h>
#include <drogon/WebSocketConnection.h>
#include <shared_mutex>

namespace tech::structures {
    class Room {
    public:
        enum class State {
            pending,
            starting,
            started,
        };

        enum class EndCondition {
            playingLeft = 0,
            finishReach,
        };

        explicit Room(
                const std::string &password,
                const uint64_t &capacity,
                Json::Value info,
                Json::Value data
        );

        Room(Room &&room) noexcept;

        [[nodiscard]] const std::string &roomId() const;

        [[nodiscard]] bool checkPassword(const std::string &password) const;

        [[nodiscard]] Json::Value getInfo(const Json::Value &list = _defaultList());

        void setInfo(const Json::Value &list);

        [[nodiscard]] Json::Value getData(const Json::Value &list = _defaultList());

        void setData(const Json::Value &list);

        void subscribe(const drogon::WebSocketConnectionPtr &connection);

        void unsubscribe(const drogon::WebSocketConnectionPtr &connection);

        void unsubscribe(const int64_t &userId);

        [[nodiscard]] bool empty() const;

        [[nodiscard]] Json::Value parse(const bool &inner = false) const;

        void publish(
                std::string &&message,
                const int64_t &excludedId = -1
        );

        void tell(
                std::string &&message,
                const int64_t &target
        );

        void changeAdmin(
                const drogon::WebSocketConnectionPtr &connection,
                const int64_t &userId
        );

        void checkReady();

        void cancelStarting();

        void checkFinished();

        ~Room();

    private:
        mutable std::shared_mutex _sharedMutex;
        const std::string _roomId, _passwordHash;
        std::atomic<uint64_t> _capacity, _endCount, _timerId;
        Json::Value _info, _data;
        std::atomic<State> _state;
        std::atomic<EndCondition> _endCondition;
        std::unordered_map<int64_t, drogon::WebSocketConnectionPtr> _connectionsMap;
        std::atomic<trantor::InetAddress> _transferNode;

        static Json::Value _defaultList() {
            Json::Value result(Json::arrayValue);
            result.append(Json::arrayValue);
            return result;
        }

        [[nodiscard]] bool _full() const;

        [[nodiscard]] uint64_t _size() const;

        [[nodiscard]] uint64_t _count() const;

        void _insert(const drogon::WebSocketConnectionPtr &connection);

        void _remove(const drogon::WebSocketConnectionPtr &connection);

        void _startingGame();

        std::string _getTransferNode();
    };
}
