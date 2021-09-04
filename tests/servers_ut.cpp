#include <boost/asio.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "mocks/server_mock.h"
#include "mocks/accept_task_mock.h"
#include "mocks/timeout_task_mock.h"
#include "servers.h"

using namespace ::testing;
using ServerMock = NaggyMock<server_mock>;
using AcceptTaskMock = NaggyMock<accept_task_mock>;
using TimeoutMock = NaggyMock<timeout_task_mock>;

struct serversTest : public Test {
    serversTest() : fake_socket(io_context) {}

    std::unique_ptr<AcceptTaskMock> accept_mock = std::make_unique<AcceptTaskMock>();
    std::unique_ptr<TimeoutMock> timeout_mock = std::make_unique<TimeoutMock>();

    boost::asio::io_context io_context;
    tcp::socket fake_socket;
};


TEST_F(serversTest, setInitialDataShouldSetDataThatWillBeSentToNewAcceptedServer) {
    EXPECT_CALL(*timeout_mock, attach_observer(_));
    EXPECT_CALL(*timeout_mock, reset_deadline());
    EXPECT_CALL(*accept_mock, attach_observer(_));
    servers servers_tested(std::move(accept_mock), std::move(timeout_mock));

    const send_type init_data{1, 2, 3};
    const send_type init_data_sent{static_cast<int8_t>(client_signal::initial_data), 1, 2, 3};
    const send_type start_send_signal{static_cast<int8_t>(client_signal::start_sending)};

    auto serv_mock = std::make_shared<ServerMock>(std::move(fake_socket), &servers_tested);
    EXPECT_CALL(*serv_mock, send_data(init_data_sent));
    EXPECT_CALL(*serv_mock, send_data(start_send_signal));
    EXPECT_CALL(*serv_mock, receive_data());

    servers_tested.set_initial_data(init_data);
    servers_tested.update_server_accepted(serv_mock);
}
