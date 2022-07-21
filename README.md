# Disco.c, an Async Discord C Bot Library
Disco-C is a part-time project to practice writing bigger projects in C and getting more experience in multiple directions (requests, websockets, APIs, etc.). It is also a nice way to learn about how Discord really works.

The library is built to work async, which means that every received event is handled in its own thread. The idea behind this is, that in a single function you want to work sequentially, but you also don't want functions like sending a message (which does a HTTP request that takes around ~200ms) block the whole bot from receiving anything anymore in that time. Another option would have been to make the HTTP request itself be async, but this then doesn't give you the guarantee that a message has been sent once the message-send function is done.

## Wiki
The wiki is still under developement, but it is available here: https://markc.su/Disco-C/

## Table of Contents
1. [Installing](#installing)
    1. [Copy Paste Command](#copy-paste-command)
    2. [Required Libraries](#required-libraries)
2. [File Explanations](#file-explanations)
3. [Progress](#progress)

## Installing
### Copy Paste Command
Use `git clone --recurse-submodules git@github.com:markbeep/Disco-C.git` to clone the submodules as well.

### Required Libraries:
- https://libwebsockets.org/
  - `sudo apt install libwebsockets-dev`
- https://curl.se/libcurl/c/
  - `sudo apt install libcurl4-gnutls-dev`
- https://github.com/DaveGamble/cJSON/
- https://github.com/sheredom/hashmap.h/
- https://github.com/ThrowTheSwitch/Unity (optional for testing)

*Note:* When building the project yourself, make sure to add `-Iinclude -Iexternal -I.` to add those three include directories. The local include path is because the `config.h` with the bot token will be in the local directory. Additionally you'll find the `cJSON.c` and `unity.c` files in the `external` directory. 

## File Explanations
The file layout is intended to follow the Pitchfork standard. Some things aren't yet done (like the main.c file being out in the open), but it's in the making.
- src/web/
  - **websocket**: Contains the required methods to abstract the background websocket connection.
  - **gateway**: Handles the websocket connection to the Discord Gateway. Making sure the heartbeat is regularly sent and the incoming responses are all properly handled.
  - **request**: Helper file that abstracts the HTTP GET/POST/PATCH/etc. requests.
- src/utils
  - **cache**: The cache uses the `hashmap.h` library to create a linked hashmap for messages, channels and guilds.
  - **disco_logging**: Personalized logging library which can be used to log messages to the console. The file path is additionally added to each message.
  - **prio_queue**: O(n) lazy priority queue implementation. Currently there's no use for this anymore.
  - **t_pool**: Thread pool library to manage the multiple threads which will then be distributed for each fired event.
- src/discord/
  - **disco**: The main functions of Disco-C. It contains the highest level of abstraction for running the bot.
  - **event**: Handles received Discord events.
  - **structures**: All of the Discord structure definitions.
  - Structure specific functions are all defined in their corresponding source files.
- **examples**: Example implementations of how the bot syntax looks like.
- **include**: All of the header definitions are in this folder.
- **external**: External libraries that are required.
- **docs**: Configuration files to automatically generate the wiki.
- **tests**: Basic tests which don't really test a lot.

## Problems
- Currently it is possible for the cache to delete a message/channel/guild structure while the struct is still being used in another thread. This happens very rarely and only with small cache sizes, but it is possible.

## Progress
- Interactions ![](https://progress-bar.dev/100/?title=4/4)
  - [x] Application Command Object
  - [x] Application Command Option Object
  - [x] Register Commands
    - [x] Global
    - [x] Guild
  - [x] Send interaction response
    - [ ] Components (untested, but implementation exists)
- Commands (API Calls)
  - Channel ![](https://progress-bar.dev/11/?title=2/18)
    - [ ] Get channel
    - [ ] Edit channel
      - [ ] Basic stuff (name, desc)
      - [ ] Add permissions
      - [ ] Remove permissions
    - [ ] Delete channel
    - [ ] Get channel message
    - [x] Send message
      - [x] Content
      - [x] Embeds
      - [ ] Components
      - [ ] Attachments
    - [x] Edit message
    - [ ] Delete message
    - [ ] Bulk delete messages
    - [ ] Add reaction
    - [ ] View reactions
    - [ ] Delete reactions
      - [ ] Own
      - [ ] User
      - [ ] All
    - [ ] Create channel invite
    - [ ] Get channel invites
    - [ ] Trigger typing indicator
    - [ ] Get pinned messages
    - [ ] Pin message
    - [ ] Unpin message
    - [ ] Group DM add/remove recipient
    - Thread ![](https://progress-bar.dev/0/?title=0/12)
      - [ ] Start thread from message
      - [ ] Start thread without message
      - [ ] Start thread in forum channel
      - [ ] Join thread
      - [ ] Leave thread
      - [ ] Add thread member
      - [ ] Remove thread member
      - [ ] Get thread member
      - [ ] List thread members
      - [ ] List public threads
      - [ ] List private threads
      - [ ] List joined private threads
  - Emoji ![](https://progress-bar.dev/0/?title=0/5)
    - [ ] List guild emojis
    - [ ] Get guild emoji
    - [ ] Create guild emoji
    - [ ] Modify guild emoji
    - [ ] Delete guild emoji
  - Guild (way too many functions to list)
  - Guild Scheduled Event ![](https://progress-bar.dev/0/?title=0/5)
    - [ ] Create guild event
    - [ ] Get guild event
    - [ ] Edit guild event
    - [ ] Delete guild event
    - [ ] Get event users
  - User ![](https://progress-bar.dev/0/?title=0/7)
    - [ ] Get (current) user 
    - [ ] Modify current user
    - [ ] Get current guilds
    - [ ] Get current guild member
    - [ ] Leave guild
    - [ ] Create DM
    - [ ] Create group DM
  - ... (no complete list of methods yet)
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
  
