#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include <boost/asio.hpp>

#include "mocks/server_mock.h"
#include "mocks/accept_task_mock.h"
#include "mocks/timeout_task_mock.h"
#include "servers.h"

using namespace ::testing;
using ServerMock = NaggyMock<server_mock>;
using AcceptTaskMock = NaggyMock<accept_task_mock>;
using TimeoutMock = NaggyMock<timeout_task_mock>;

struct serversTest : public Test {
    serversTest() : start_send_signal{static_cast<int8_t>(client_signal::start_sending)},
                    fake_socket(io_context) {}

    void create_servers_tested(size_t reset_deadline_times) {
        EXPECT_CALL(*timeout_mock, attach_observer(_));
        EXPECT_CALL(*timeout_mock, reset_deadline()).Times(reset_deadline_times);
        EXPECT_CALL(*accept_mock, attach_observer(_));
        servers_tested = std::make_unique<servers>(std::move(accept_mock), std::move(timeout_mock));
    }

    void update_first_server_accepted(std::shared_ptr<ServerMock>& s_mock) {
        EXPECT_CALL(*s_mock, send_data(start_send_signal));
        EXPECT_CALL(*s_mock, receive_data());
        servers_tested->update_server_accepted(s_mock);
    }

    void accept_two_server_mocks(std::shared_ptr<ServerMock>& first, 
                                 std::shared_ptr<ServerMock>& second) {

        first = std::make_shared<ServerMock>(std::move(fake_socket), servers_tested.get());
        second = std::make_shared<ServerMock>(std::move(fake_socket), servers_tested.get());

        update_first_server_accepted(first);

        EXPECT_CALL(*second, receive_data());
        servers_tested->update_server_accepted(second);
    }

    std::unique_ptr<AcceptTaskMock> accept_mock = std::make_unique<AcceptTaskMock>();
    std::unique_ptr<TimeoutMock> timeout_mock = std::make_unique<TimeoutMock>();
    std::unique_ptr<servers> servers_tested;
    const send_type start_send_signal;

    boost::asio::io_context io_context;
    tcp::socket fake_socket;
};

TEST_F(serversTest, updateServerAcceptedShouldInvokeSendAndReceiveFunctionsFromServer) {
    constexpr size_t reset_deadline_times = 1;
    create_servers_tested(reset_deadline_times);

    auto serv_mock = std::make_shared<ServerMock>(std::move(fake_socket), servers_tested.get());
    update_first_server_accepted(serv_mock);
}

TEST_F(serversTest, setInitialDataShouldSetDataThatWillBeSentToNewAcceptedServer) {
    const send_type init_data{1, 2};
    const send_type init_data_sent{static_cast<int8_t>(client_signal::initial_data), 1, 2};
    constexpr size_t reset_deadline_times = 1;
    create_servers_tested(reset_deadline_times);

    auto serv_mock = std::make_shared<ServerMock>(std::move(fake_socket), servers_tested.get());

    EXPECT_CALL(*serv_mock, send_data(init_data_sent));
    servers_tested->set_initial_data(init_data);
    update_first_server_accepted(serv_mock);
}

TEST_F(serversTest, sendDataShouldInvokeSendMethodFromServer) {
    const send_type data{1, 2, 3, 4};
    constexpr size_t reset_deadline_times = 1;
    create_servers_tested(reset_deadline_times);

    auto serv_mock = std::make_shared<ServerMock>(std::move(fake_socket), servers_tested.get());

    update_first_server_accepted(serv_mock);

    EXPECT_CALL(*serv_mock, send_data(data));
    servers_tested->send_data(data);
}

TEST_F(serversTest, changeReceivingServerShouldSendStopSignalToFirstServerOnListAndStartSignalToSecondServer) {
    const send_type stop_send_signal{static_cast<int8_t>(client_signal::stop_sending)};
    constexpr size_t reset_deadline_times = 2;
    std::shared_ptr<ServerMock> serv_mock;
    std::shared_ptr<ServerMock> serv_mock_second;
    create_servers_tested(reset_deadline_times);

    accept_two_server_mocks(serv_mock, serv_mock_second);

    EXPECT_CALL(*serv_mock, send_data(stop_send_signal));
    EXPECT_CALL(*serv_mock_second, send_data(start_send_signal));
    servers_tested->change_receiving_server();
}

TEST_F(serversTest, updateDataReceivedShouldResetDeadline) {
    constexpr uint8_t data_received = 1;
    constexpr size_t reset_deadline_times = 2;
    create_servers_tested(reset_deadline_times);
    auto serv_mock = std::make_shared<ServerMock>(std::move(fake_socket), servers_tested.get());
    update_first_server_accepted(serv_mock);

    servers_tested->update_data_received(data_received);
}

TEST_F(serversTest, updateDisconnectedWithNotReceivingServerShouldNotResetDeadlineSecondTime) {
    constexpr size_t reset_deadline_times = 1;
    std::shared_ptr<ServerMock> serv_mock;
    std::shared_ptr<ServerMock> serv_mock_second;
    create_servers_tested(reset_deadline_times);

    accept_two_server_mocks(serv_mock, serv_mock_second);

    servers_tested->update_disconnected(serv_mock_second);
}

TEST_F(serversTest, updateDisconnectedWithReceivingServerShouldUpdateReceivingServer) {
    constexpr size_t reset_deadline_times = 2;
    std::shared_ptr<ServerMock> serv_mock;
    std::shared_ptr<ServerMock> serv_mock_second;
    create_servers_tested(reset_deadline_times);

    accept_two_server_mocks(serv_mock, serv_mock_second);

    EXPECT_CALL(*serv_mock_second, send_data(start_send_signal));
    servers_tested->update_disconnected(serv_mock);
}

TEST_F(serversTest, updateTimeoutShouldChangeReceivingServer) {
    const send_type stop_send_signal{static_cast<int8_t>(client_signal::stop_sending)};
    constexpr size_t reset_deadline_times = 2;
    std::shared_ptr<ServerMock> serv_mock;
    std::shared_ptr<ServerMock> serv_mock_second;
    create_servers_tested(reset_deadline_times);

    accept_two_server_mocks(serv_mock, serv_mock_second);

    EXPECT_CALL(*serv_mock, send_data(stop_send_signal));
    EXPECT_CALL(*serv_mock_second, send_data(start_send_signal));
    servers_tested->update_timeout();
}
