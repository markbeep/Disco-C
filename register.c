/**
 * @file register.c
 * @brief Example on how to register application commands on Discord.
 * For demonstration purposes the slash commands are split into multiple
 * files.
 * @version 0.1
 * @date 2022-07-04
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "examples/example_slash_commands/example_slash.h"
#include "src/discord/structures/command.h"
#include "src/utils/disco_logging.h"

int main() {
    d_set_log_level(D_LOG_ALL);
    register_hello_command();
    return 0;
}
