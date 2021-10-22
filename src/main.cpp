#include "game_server.h"
#include "privileged_connection.h"
#include "program_options.h"
#include "servers.h"
#include "snake.h"
#include "snake_game.h"
#include "timeout_task.h"

int main(int argc, char **argv) {
    parse_argv(argc, argv);

    auto snake_game_ptr = std::make_unique<snake_game>(std::make_unique<timer<snake_game>>());
    auto servers_ptr = std::make_unique<servers>(std::make_unique<accept_task>(), std::make_unique<timeout_task>());
    auto priv_connection = std::make_unique<privileged_connection>(std::make_unique<privileged_accept_task>());
    game_server g_server(std::move(servers_ptr), std::move(snake_game_ptr), std::move(priv_connection));

    while (true) {}

    return 0;
}
