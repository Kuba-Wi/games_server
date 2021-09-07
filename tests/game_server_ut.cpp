#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "game_server.h"
#include "mocks/accept_task_mock.h"
#include "mocks/servers_mock.h"
#include "mocks/snake_game_mock.h"
#include "mocks/timeout_task_mock.h"
#include "mocks/timer_mock.h"


using namespace ::testing;
using AcceptTaskMock = NaggyMock<accept_task_mock>;
using ServersMock = NaggyMock<servers_mock>;
using SnakeGameMock = NaggyMock<snake_game_mock>;
using TimeoutMock = NaggyMock<timeout_task_mock>;
using TimerMock = NaggyMock<timer_mock<snake_game>>;

constexpr size_t interval_ms = 100;
constexpr size_t height = 5;
constexpr size_t width = 7;

struct gameServerTest : public Test {
    gameServerTest() {
        accept_mock_ptr = std::make_unique<AcceptTaskMock>();
        timeout_mock_ptr = std::make_unique<TimeoutMock>();
        timer_mock_ptr = std::make_unique<TimerMock>();
    }

    void SetUp() {
        EXPECT_CALL(*accept_mock_ptr, attach_observer(_));
        EXPECT_CALL(*timeout_mock_ptr, attach_observer(_));
        EXPECT_CALL(*timer_mock_ptr, stop_timer());
        servers_mock_ptr = std::make_unique<ServersMock>(std::move(accept_mock_ptr), std::move(timeout_mock_ptr));
        sn_game_mock_ptr = std::make_unique<SnakeGameMock>(std::move(timer_mock_ptr), interval_ms, height, width);
    }

    void constructGameServer() {
        const send_type initial_data{height, width};
        EXPECT_CALL(*servers_mock_ptr, attach_observer(_));
        EXPECT_CALL(*sn_game_mock_ptr, attach_observer(_));
        EXPECT_CALL(*servers_mock_ptr, set_initial_data(initial_data));
        game_server_tested = std::make_unique<game_server>(std::move(servers_mock_ptr), std::move(sn_game_mock_ptr));
    }

    std::unique_ptr<ServersMock> servers_mock_ptr;
    std::unique_ptr<SnakeGameMock> sn_game_mock_ptr;
    std::unique_ptr<AcceptTaskMock> accept_mock_ptr;
    std::unique_ptr<TimeoutMock> timeout_mock_ptr;
    std::unique_ptr<TimerMock> timer_mock_ptr;

    std::unique_ptr<game_server> game_server_tested;
};

TEST_F(gameServerTest, gameServerConstructorShouldAttachObserversAndSetInitialDataInServers) {
    constructGameServer();
}

TEST_F(gameServerTest, startGameFunctionShouldInvokeMethodFromSnakeGame) {
    EXPECT_CALL(*sn_game_mock_ptr, start_new_game());
    constructGameServer();
    game_server_tested->start_game();
}

TEST_F(gameServerTest, updateGameShouldInvokeMethodFromSnakeGame) {
    constexpr uint8_t direction = 1;
    EXPECT_CALL(*sn_game_mock_ptr, set_snake_direction(direction));
    constructGameServer();
    game_server_tested->update_game(direction);
}

TEST_F(gameServerTest, updateSnakeMovedShouldInvokeMethodFromServers) {
    const std::vector<int8_t> snake_data{1, 2, 3, 4};
    EXPECT_CALL(*servers_mock_ptr, send_data(snake_data));
    constructGameServer();
    game_server_tested->update_snake_moved(snake_data);
}

TEST_F(gameServerTest, updateGameFinishedShouldInvokeMethodFromServers) {
    EXPECT_CALL(*servers_mock_ptr, change_receiving_server());
    constructGameServer();
    game_server_tested->update_game_finished();
}
