#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "game_server.h"
#include "mocks/accept_task_mock.h"
#include "mocks/priv_accept_task_mock.h"
#include "mocks/priv_connection_mock.h"
#include "mocks/servers_mock.h"
#include "mocks/snake_game_mock.h"
#include "mocks/timeout_task_mock.h"
#include "mocks/timer_mock.h"


using namespace ::testing;
using AcceptTaskMock = NaggyMock<accept_task_mock>;
using PrivAcceptTaskMock = NaggyMock<priv_accept_task_mock>;
using PrivConnectionMock = NaggyMock<priv_connection_mock>;
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
        priv_accept_task_mock_ptr = std::make_unique<PrivAcceptTaskMock>();
        timeout_mock_ptr = std::make_unique<TimeoutMock>();
        timer_mock_ptr = std::make_unique<TimerMock>();
    }

    void SetUp() {
        EXPECT_CALL(*timeout_mock_ptr, attach_observer(_));
        EXPECT_CALL(*timer_mock_ptr, stop_timer());
        EXPECT_CALL(*accept_mock_ptr, attach_observer(_));
        EXPECT_CALL(*accept_mock_ptr, accept_connections());
        EXPECT_CALL(*priv_accept_task_mock_ptr, attach_observer(_));
        EXPECT_CALL(*priv_accept_task_mock_ptr, accept_connections());
        servers_mock_ptr = std::make_unique<ServersMock>(std::move(accept_mock_ptr), std::move(timeout_mock_ptr));
        sn_game_mock_ptr = std::make_unique<SnakeGameMock>(std::move(timer_mock_ptr));
        priv_connection_mock_ptr = std::make_unique<PrivConnectionMock>(std::move(priv_accept_task_mock_ptr));
    }

    void constructGameServer() {
        EXPECT_CALL(*servers_mock_ptr, attach_observer(_));
        EXPECT_CALL(*sn_game_mock_ptr, attach_observer(_));
        EXPECT_CALL(*priv_connection_mock_ptr, attach_observer(_));
        game_server_tested = std::make_unique<game_server>(std::move(servers_mock_ptr),
                                                           std::move(sn_game_mock_ptr),
                                                           std::move(priv_connection_mock_ptr));
    }

    std::unique_ptr<AcceptTaskMock> accept_mock_ptr;
    std::unique_ptr<PrivConnectionMock> priv_connection_mock_ptr;
    std::unique_ptr<PrivAcceptTaskMock> priv_accept_task_mock_ptr;
    std::unique_ptr<ServersMock> servers_mock_ptr;
    std::unique_ptr<SnakeGameMock> sn_game_mock_ptr;
    std::unique_ptr<TimeoutMock> timeout_mock_ptr;
    std::unique_ptr<TimerMock> timer_mock_ptr;

    std::unique_ptr<game_server> game_server_tested;
};

TEST_F(gameServerTest, gameServerConstructorShouldAttachObserversAndSetInitialDataInServers) {
    constructGameServer();
}

TEST_F(gameServerTest, restartGameFunctionShouldInvokeMethodFromSnakeGame) {
    EXPECT_CALL(*sn_game_mock_ptr, restart_game(interval_ms));
    constructGameServer();
    game_server_tested->restart_game(interval_ms);
}

TEST_F(gameServerTest, stopGameFunctionShouldInvokeMethodFromSnakeGame) {
    EXPECT_CALL(*sn_game_mock_ptr, stop_game());
    constructGameServer();
    game_server_tested->stop_game();
}

TEST_F(gameServerTest, changeSnakeMoveTimeShouldInvokeMethodFromSnakeGame) {
    EXPECT_CALL(*sn_game_mock_ptr, change_time_interval(interval_ms));
    constructGameServer();
    game_server_tested->change_snake_move_time(interval_ms);
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
    EXPECT_CALL(*priv_connection_mock_ptr, send_data(snake_data));
    constructGameServer();
    game_server_tested->update_snake_moved(snake_data);
}

TEST_F(gameServerTest, updateGameFinishedShouldInvokeMethodFromServers) {
    EXPECT_CALL(*servers_mock_ptr, change_receiving_server());
    constructGameServer();
    game_server_tested->update_game_finished();
}

TEST_F(gameServerTest, getClientsCountShouldGetConnectedClientsNumber) {
    constexpr size_t clients_connected = 0;
    constructGameServer();
    EXPECT_EQ(game_server_tested->get_clients_count(), clients_connected);
}

TEST_F(gameServerTest, updateBoardSizeShouldUpdateBoardSize) {
    EXPECT_CALL(*sn_game_mock_ptr, set_board_size(height, width));
    EXPECT_CALL(*servers_mock_ptr, update_initial_data(send_type{static_cast<int8_t>(height), static_cast<int8_t>(width)}));
    constructGameServer();
    game_server_tested->update_board_size(height, width);
}

TEST_F(gameServerTest, updateBoardSizeShouldDoNothingWhenBoardSizeIsNotProper) {
    constexpr size_t wrong_height = 0;
    constexpr size_t wrong_width = 0;
    EXPECT_CALL(*sn_game_mock_ptr, set_board_size(_, _)).Times(0);
    EXPECT_CALL(*servers_mock_ptr, update_initial_data(_)).Times(0);
    constructGameServer();
    game_server_tested->update_board_size(wrong_height, wrong_width);
}

TEST_F(gameServerTest, updateClientsCountChangedShouldSendCountToPrivilegedClient) {
    constexpr size_t clients_count = 10;
    EXPECT_CALL(*priv_connection_mock_ptr, send_clients_count(clients_count));
    constructGameServer();
    game_server_tested->update_clients_count_changed(clients_count);
}
