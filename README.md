# Disco.c, a Discord C bot library

### Installing
Use `git clone --recurse-submodules git@github.com:markbeep/Disco-C.git` to clone the submodules as well.

### File explanations

- src/web/**websocket**: Contains the required methods to abstract the background websocket connection.
- src/web/**gateway**: Handles the websocket connection to the Discord Gateway. Making sure the heartbeat is regularly sent and the incoming responses are all properly handled.
- src/web/**request**: Helper file that abstracts the HTTP GET/POST/PATCH/etc. requests.
- src/discord/
  - **disco**: The main functions of Disco-C. It contains the highest level of abstraction for running the bot.
  - **event**: Handles received Discord events.
  - **structures/**: All of the Discord structure definitions.
  - Structure specific functions are all defined in their corresponding source files.
- **example**/: Example implementations of how the bot sytax looks like.
- **lib**/: External libraries not downloadable with the APT package manager (basically just Github libraries). Will be renamed to `include` in the future to maintain the Pitchfork standard.

### Why are some things called `disco_...` while others are called `discord_...`?
All of the structures that are taken straight from the [Discord documentation](https://discord.com/developers/docs/) are all named with a leading `discord_`. At the structure definition there should also be a link leading to the exact place in the documentation that the definition was taken from.

On the other hand, everything starting with `disco_` is a public library method that can be used at the top level to control the bot.


### Required Libaries:
- https://libwebsockets.org/
  - `sudo apt install libwebsockets-dev`
- https://curl.se/libcurl/c/
  - `sudo apt install libcurl4-gnutls-dev`
- https://github.com/DaveGamble/cJSON/
- https://github.com/sheredom/hashmap.h/

### Progress
- Events ![](https://progress-bar.dev/14/?title=8/57)
  - [x] READY
  - [ ] RESUMED
  - [ ] RECONNECT
  - [ ] INVALID_SESSION
  - [ ] APPLICATION_COMMAND_PERMISSIONS_UPDATE
  - [x] CHANNEL_CREATE
  - [x] CHANNEL_UPDATE
  - [x] CHANNEL_DELETE
  - [ ] CHANNEL_PINS_UPDATE
  - [ ] THREAD_CREATE
  - [ ] THREAD_UPDATE
  - [ ] THREAD_DELETE
  - [ ] THREAD_LIST_SYNC
  - [ ] THREAD_MEMBER_UPDATE
  - [ ] THREAD_MEMBERS_UPDATE
  - [ ] GUILD_CREATE
  - [ ] GUILD_UPDATE
  - [ ] GUILD_DELETE
  - [ ] GUILD_BAN_ADD
  - [ ] GUILD_BAN_REMOVE
  - [ ] GUILD_EMOJIS_UPDATE
  - [ ] GUILD_STICKERS_UPDATE
  - [ ] GUILD_INTEGRATIONS_UPDATE
  - [ ] GUILD_MEMBER_ADD
  - [ ] GUILD_MEMBER_REMOVE
  - [ ] GUILD_MEMBER_UPDATE
  - [ ] GUILD_MEMBERS_CHUNK
  - [ ] GUILD_ROLE_CREATE
  - [ ] GUILD_ROLE_UPDATE
  - [ ] GUILD_ROLE_DELETE
  - [ ] GUILD_SCHEDULED_EVENT_CREATE
  - [ ] GUILD_SCHEDULED_EVENT_UPDATE
  - [ ] GUILD_SCHEDULED_EVENT_DELETE
  - [ ] GUILD_SCHEDULED_EVENT_USER_ADD
  - [ ] GUILD_SCHEDULED_EVENT_USER_REMOVE
  - [ ] INTEGRATION_CREATE
  - [ ] INTEGRATION_UPDATE
  - [ ] INTEGRATION_CREATE
  - [ ] INVITE_CREATE
  - [ ] INVITE_DELETE
  - [x] MESSAGE_CREATE
  - [x] MESSAGE_UPDATE
  - [x] MESSAGE_DELETE
  - [ ] MESSAGE_DELETE_BULK
  - [ ] MESSAGE_REACTION_ADD
  - [ ] MESSAGE_REACTION_REMOVE
  - [ ] MESSAGE_REACTION_REMOVE_ALL
  - [ ] MESSAGE_REACTION_REMOVE_EMOJI
  - [ ] PRESENCE_UPDATE
  - [ ] STAGE_INSTANCE_CREATE
  - [ ] STAGE_INSTANCE_DELETE
  - [ ] STAGE_INSTANCE_UPDATE
  - [ ] TYPING_START
  - [ ] USER_UPDATE
  - [ ] VOICE_STATE_UPDATE
  - [ ] VOICE_SERVER_UPDATE
  - [ ] WEBHOOKS_UPDATE
  - [x] INTERACTION_CREATE
- Interactions
  - [x] Application Command Object
  - [x] Application Command Option Object
  - [x] Register Commands
    - [x] Global
    - [x] Guild
  