#include "game_server.h"
#include "servers.h"
#include "snake.h"
#include "snake_game.h"
#include "timeout_task.h"

int main() {
    auto snake_game_ptr = std::make_unique<snake_game>(std::make_unique<timer<snake_game>>(), 200, 10, 12);
    auto servers_ptr = std::make_unique<servers>(std::make_unique<accept_task>(), std::make_unique<timeout_task>());
    game_server g_server(std::move(servers_ptr), std::move(snake_game_ptr));
    g_server.start_game();

    while (true) {}

    return 0;
}
