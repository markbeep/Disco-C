# Disco.c, a Discord C bot library

### Installing
Use `git clone --recurse-submodules git@github.com:markbeep/Disco-C.git` to clone the submodules as well.

### File explanations

- libs/web/**websocket**: Contains the required methods to abstract the background websocket connection.
- libs/web/**gateway**: Handles the websocket connection to the Discord Gateway. Making sure the heartbeat is regularly sent and the incoming responses are all properly handled.
- libs/web/**request**: Helper file that abstracts the HTTP GET and POST requests.
- libs/utils/**cJSON**: External library linked below that is used to parse JSON.
- libs/discord/
  - **disco**: The main functions of Disco-C. It contains the highest level of abstraction for running the bot.
  - **event**: Handles received Discord events.
  - **structures/**: All of the Discord structure definitions
- **example**/: Example implementations of how the bot sytax looks like.

### Why are some things called `disco_...` while others are called `discord_...`?
All of the structures that are taken straight from the [Discord documentation](https://discord.com/developers/docs/resources) are all named with a leading `discord_`. At the structure definition there should also be a link leading to the exact place in the documentation that the definition was taken from.

On the other hand, everything starting with `disco_` is a public library method that can be used at the top level to control the bot.


### Required Libaries:
- https://libwebsockets.org/
- https://curl.se/libcurl/c/
- https://github.com/DaveGamble/cJSON/
