#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "snake_client.h"
#include "mocks/network_mock.h"

using namespace ::testing;
using NetworkMock = NaggyMock<network_mock>;

struct SnakeClientTest : public Test {
    void create_snake_client(std::unique_ptr<NetworkMock>& mock) {
        EXPECT_CALL(*mock, attach_observer(_));
        snake_client_ptr = std::make_unique<snake_client>(std::move(mock));
    }

    std::unique_ptr<snake_client> snake_client_ptr;
};

TEST_F(SnakeClientTest, constructorShouldInvokeNetworkAttachObserverMethod) {
    auto mock = std::make_unique<NetworkMock>();
    create_snake_client(mock);
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
