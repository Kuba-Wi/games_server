#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "snake_client.h"
#include "mocks/network_mock.h"

using namespace ::testing;
using NetworkMock = NaggyMock<network_mock>;

struct SnakeClientTest : public Test {
    void create_snake_client() {
        net_mock = std::make_unique<NetworkMock>();
        EXPECT_CALL(*net_mock, attach_observer(_));
        snake_client_ptr = std::make_unique<snake_client>(std::move(net_mock));
    }

    std::unique_ptr<NetworkMock> net_mock;
    std::unique_ptr<snake_client> snake_client_ptr;
};

TEST_F(SnakeClientTest, constructor) {
    create_snake_client();
}
