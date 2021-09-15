#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "snake_client.h"
#include "mocks/network_mock.h"
#include "mocks/ui_iface_mock.h"

using namespace ::testing;
using NetworkMock = NaggyMock<network_mock>;
using UiMock = NaggyMock<ui_iface_mock>;

struct SnakeClientTest : public Test {
    void create_snake_client(std::unique_ptr<NetworkMock>& mock) {
        EXPECT_CALL(*mock, attach_observer(_));
        snake_client_ptr = std::make_unique<snake_client>(std::move(mock));
    }

    void SetUp() {
        ui_mock_holder.mock_ptr = std::make_unique<UiMock>();
    }

    void TearDown() {
        ui_mock_holder.mock_ptr.reset();
    }

    std::unique_ptr<snake_client> snake_client_ptr;
};

TEST_F(SnakeClientTest, constructorShouldInvokeNetworkAttachObserverMethod) {
    auto mock = std::make_unique<NetworkMock>();
    create_snake_client(mock);
}

TEST_F(SnakeClientTest, setDisconnectedMethodShouldInvokeWaitForConnectionFreeFunction) {
    auto net_mock = std::make_unique<NetworkMock>();
    create_snake_client(net_mock);

    EXPECT_CALL(*ui_mock_holder.mock_ptr, wait_for_connection());
    snake_client_ptr->set_disconnected();
}

TEST_F(SnakeClientTest, setConnectedMethodShouldInvokeConnectionEstablishedFreeFunction) {
    auto net_mock = std::make_unique<NetworkMock>();
    create_snake_client(net_mock);

    EXPECT_CALL(*ui_mock_holder.mock_ptr, connection_established());
    snake_client_ptr->set_connected();
}

// TODO: add TEST_P with more examples
TEST_F(SnakeClientTest, setServerAddressShouldReturnTrueIfIpAddressIsCorrect) {
    const std::string server_ip{"1.1.1.1"};
    auto mock = std::make_unique<NetworkMock>();
    create_snake_client(mock);
    ASSERT_TRUE(snake_client_ptr->set_server_address(server_ip));
}

TEST_F(SnakeClientTest, setServerAddressShouldReturnFalseIfIpAddressIsInCorrect) {
    const std::string server_ip{"256.1.1.1"};
    auto mock = std::make_unique<NetworkMock>();
    create_snake_client(mock);
    ASSERT_FALSE(snake_client_ptr->set_server_address(server_ip));
}

TEST_F(SnakeClientTest, connectNetworkShouldInvokeConnectMethodFromNetwork) {
    auto mock = std::make_unique<NetworkMock>();
    EXPECT_CALL(*mock, connect);
    create_snake_client(mock);
    snake_client_ptr->connect_network();
}

TEST_F(SnakeClientTest, sendDataShouldDoNothinkWhenSendingIsNotEnabled) {
    constexpr uint8_t data = 1;
    auto mock = std::make_unique<NetworkMock>();
    create_snake_client(mock);
    snake_client_ptr->send_data(data);
}

TEST_F(SnakeClientTest, sendDataShouldInvokeMethodFromNetworkWhenUpdateSnakeMethodEnablesIt) {
    const std::vector<int8_t> data_received{static_cast<int8_t>(client_signal::start_sending)};
    constexpr uint8_t data_sent = 1;
    auto mock = std::make_unique<NetworkMock>();
    EXPECT_CALL(*mock, send_data(data_sent));
    create_snake_client(mock);

    EXPECT_CALL(*ui_mock_holder.mock_ptr, enable_sending());
    snake_client_ptr->update_snake(data_received);
    snake_client_ptr->send_data(data_sent);
}

TEST_F(SnakeClientTest, updateSnakeWithDataStartingWithPositiveValueShouldRefreshModel) {
    const std::vector<int8_t> data_received{1, 2, 3};
    auto mock = std::make_unique<NetworkMock>();
    create_snake_client(mock);
    
    EXPECT_CALL(*ui_mock_holder.mock_ptr, refresh_model());
    snake_client_ptr->update_snake(data_received);
}

TEST_F(SnakeClientTest, updateSnakeWithEmptyDataShouldDoNothing) {
    const std::vector<int8_t> data_received{};
    auto mock = std::make_unique<NetworkMock>();
    create_snake_client(mock);

    snake_client_ptr->update_snake(data_received);
}

TEST_F(SnakeClientTest, updateSnakeWithInitialDataShouldSetBoardHeightAndWidth) {
    constexpr int8_t height = 1;
    constexpr int8_t width = 2;
    const std::vector<int8_t> data_received{static_cast<int8_t>(client_signal::initial_data), height, width};
    auto mock = std::make_unique<NetworkMock>();
    create_snake_client(mock);
    
    EXPECT_CALL(*ui_mock_holder.mock_ptr, set_board_dimensions());
    snake_client_ptr->update_snake(data_received);

    ASSERT_EQ(snake_client_ptr->get_board_height(), height);
    ASSERT_EQ(snake_client_ptr->get_board_width(), width);
}

TEST_F(SnakeClientTest, updateSnakeWithWrongSizeOfInitialDataShouldDoNothing) {
    const std::vector<int8_t> data_received{static_cast<int8_t>(client_signal::initial_data), 1};
    auto mock = std::make_unique<NetworkMock>();
    create_snake_client(mock);

    snake_client_ptr->update_snake(data_received);
}

TEST_F(SnakeClientTest, updateSnakeWithStopSendingSignalShouldInvokeStopSendingFreeFunction) {
    const std::vector<int8_t> data_received{static_cast<int8_t>(client_signal::stop_sending)};
    auto mock = std::make_unique<NetworkMock>();
    create_snake_client(mock);

    EXPECT_CALL(*ui_mock_holder.mock_ptr, stop_sending());
    snake_client_ptr->update_snake(data_received);
}

TEST_F(SnakeClientTest, updateSnakeWithStopSendingSignalShouldNotLetSendDataToBeInvoked) {
    const std::vector<int8_t> data_start{static_cast<int8_t>(client_signal::start_sending)};
    const std::vector<int8_t> data_stop{static_cast<int8_t>(client_signal::stop_sending)};
    constexpr uint8_t data_not_sent = 1;
    auto mock = std::make_unique<NetworkMock>();
    create_snake_client(mock);

    EXPECT_CALL(*ui_mock_holder.mock_ptr, enable_sending());
    EXPECT_CALL(*ui_mock_holder.mock_ptr, stop_sending());
    snake_client_ptr->update_snake(data_start);
    snake_client_ptr->update_snake(data_stop);

    snake_client_ptr->send_data(data_not_sent);
}

TEST_F(SnakeClientTest, setDisconnectedShouldNotLetSendDataToBeInvoked) {
    const std::vector<int8_t> data_start{static_cast<int8_t>(client_signal::start_sending)};
    constexpr uint8_t data_not_sent = 1;
    auto mock = std::make_unique<NetworkMock>();
    create_snake_client(mock);

    EXPECT_CALL(*ui_mock_holder.mock_ptr, enable_sending());
    EXPECT_CALL(*ui_mock_holder.mock_ptr, wait_for_connection());
    snake_client_ptr->update_snake(data_start);
    snake_client_ptr->set_disconnected();

    snake_client_ptr->send_data(data_not_sent);
}

TEST_F(SnakeClientTest, checkIndexPresentOnEmptyBoardShouldReturnFalse) {
    constexpr uint8_t x = 1;
    constexpr int8_t y = 2;
    auto mock = std::make_unique<NetworkMock>();
    create_snake_client(mock);

    ASSERT_FALSE(snake_client_ptr->check_index_present(x, y));
}

TEST_F(SnakeClientTest, checkIndexPresentShouldReturnTrueIfReceivedDataPointsToThisIndex) {
    constexpr int8_t height = 2;
    constexpr int8_t width = 2;
    const std::vector<int8_t> data_initial{static_cast<int8_t>(client_signal::initial_data), height, width};
    const std::vector<int8_t> data_with_indexes{0, 0, 0, 1};
    auto mock = std::make_unique<NetworkMock>();
    create_snake_client(mock);

    EXPECT_CALL(*ui_mock_holder.mock_ptr, set_board_dimensions());
    EXPECT_CALL(*ui_mock_holder.mock_ptr, refresh_model());
    snake_client_ptr->update_snake(data_initial);
    snake_client_ptr->update_snake(data_with_indexes);

    ASSERT_TRUE(snake_client_ptr->check_index_present(0, 0));
    ASSERT_TRUE(snake_client_ptr->check_index_present(0, 1));
    ASSERT_FALSE(snake_client_ptr->check_index_present(1, 0));
    ASSERT_FALSE(snake_client_ptr->check_index_present(1, 1));
}
