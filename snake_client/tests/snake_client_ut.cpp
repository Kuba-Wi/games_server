#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "snake_client.h"
#include "mocks/network_mock.h"
#include "mocks/ui_iface_mock.h"

using namespace ::testing;
using NetworkMock = NaggyMock<network_mock>;
using UiMock = NaggyMock<ui_iface_mock>;

using pair_str_bool = std::pair<std::string, bool>;

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

TEST_F(SnakeClientTest, setDisconnectedMethodShouldInvokeConnectionErrorFreeFunction) {
    const std::string message{"error"};
    auto net_mock = std::make_unique<NetworkMock>();
    create_snake_client(net_mock);

    EXPECT_CALL(*ui_mock_holder.mock_ptr, connection_error(message));
    snake_client_ptr->update_disconnected(message);
}

TEST_F(SnakeClientTest, setConnectedMethodShouldInvokeConnectionEstablishedFreeFunction) {
    auto net_mock = std::make_unique<NetworkMock>();
    create_snake_client(net_mock);

    EXPECT_CALL(*ui_mock_holder.mock_ptr, connection_established());
    snake_client_ptr->set_connected();
}

struct SnakeClientParametrizedTest : SnakeClientTest, 
                                     WithParamInterface<pair_str_bool> {};

INSTANTIATE_TEST_CASE_P(TestsWithTrueResult, 
                        SnakeClientParametrizedTest,
                        Values(pair_str_bool{"0.0.0.0", true}, 
                               pair_str_bool{"1.1.1.1", true}, 
                               pair_str_bool{"255.255.255.255", true}, 
                               pair_str_bool{"123.65.34.90", true}));

INSTANTIATE_TEST_CASE_P(TestsWithFalseResult, 
                        SnakeClientParametrizedTest,
                        Values(pair_str_bool{"256.0.0.0", false},
                               pair_str_bool{"0.0.0", false},
                               pair_str_bool{"-1.0.0.0", false},
                               pair_str_bool{"123456789", false},
                               pair_str_bool{"some text", false}));

TEST_P(SnakeClientParametrizedTest, setServerAddressShouldReturnTrueIfIpAddressIsCorrectAndFalseOtherwise) {
    auto mock = std::make_unique<NetworkMock>();
    create_snake_client(mock);
    const auto& [input, result] = GetParam();
    ASSERT_EQ(snake_client_ptr->set_server_address(input), result);
}

TEST_F(SnakeClientTest, connectNetworkShouldInvokeConnectMethodFromNetwork) {
    auto mock = std::make_unique<NetworkMock>();
    EXPECT_CALL(*mock, connect);
    create_snake_client(mock);
    snake_client_ptr->connect_network();
}

TEST_F(SnakeClientTest, updateConnectionFailedShouldInvokeConnectionErrorFunction) {
    const std::string message{"error"};
    auto mock = std::make_unique<NetworkMock>();
    EXPECT_CALL(*ui_mock_holder.mock_ptr, connection_error(message));
    create_snake_client(mock);
    snake_client_ptr->update_connection_failed(message);
}

TEST_F(SnakeClientTest, sendDataShouldDoNothinkWhenSendingIsNotEnabled) {
    constexpr uint8_t data = 1;
    auto mock = std::make_unique<NetworkMock>();
    EXPECT_CALL(*mock, send_data(_)).Times(0);
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
    EXPECT_CALL(*ui_mock_holder.mock_ptr, refresh_model());
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
    const std::string message{"error"};
    const std::vector<int8_t> data_start{static_cast<int8_t>(client_signal::start_sending)};
    constexpr uint8_t data_not_sent = 1;
    auto mock = std::make_unique<NetworkMock>();
    EXPECT_CALL(*mock, send_data(_)).Times(0);
    create_snake_client(mock);

    EXPECT_CALL(*ui_mock_holder.mock_ptr, enable_sending());
    EXPECT_CALL(*ui_mock_holder.mock_ptr, connection_error(message));
    snake_client_ptr->update_snake(data_start);
    snake_client_ptr->update_disconnected(message);

    snake_client_ptr->send_data(data_not_sent);
}

TEST_F(SnakeClientTest, checkIndexPresentOnEmptyBoardShouldReturnFalse) {
    constexpr uint8_t x = 1;
    constexpr int8_t y = 2;
    auto mock = std::make_unique<NetworkMock>();
    create_snake_client(mock);

    ASSERT_FALSE(snake_client_ptr->check_index_present(x, y));
}

TEST_F(SnakeClientTest, checkIndexPresentShouldReturnTrueIfReceivedDataPointsWithThisIndex) {
    constexpr int8_t height = 2;
    constexpr int8_t width = 2;
    const std::vector<int8_t> data_initial{static_cast<int8_t>(client_signal::initial_data), height, width};
    const std::vector<int8_t> data_with_indexes{0, 0, 0, 1};
    auto mock = std::make_unique<NetworkMock>();
    create_snake_client(mock);

    EXPECT_CALL(*ui_mock_holder.mock_ptr, set_board_dimensions());
    EXPECT_CALL(*ui_mock_holder.mock_ptr, refresh_model()).Times(2);
    snake_client_ptr->update_snake(data_initial);
    snake_client_ptr->update_snake(data_with_indexes);

    ASSERT_TRUE(snake_client_ptr->check_index_present(0, 0));
    ASSERT_TRUE(snake_client_ptr->check_index_present(0, 1));
    ASSERT_FALSE(snake_client_ptr->check_index_present(1, 0));
    ASSERT_FALSE(snake_client_ptr->check_index_present(1, 1));
}
