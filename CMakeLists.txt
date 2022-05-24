cmake_minimum_required(VERSION 3.23)
project(GameProject)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS -stdlib=libc++)

# undefined reference to `std::result_of`
add_definitions(-DASIO_HAS_STD_INVOKE_RESULT)
# undefined reference to `fmt::v7::detail::vformat`
add_definitions(-DFMT_HEADER_ONLY)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(ASIO_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/Third/asio/include)
set(FMT_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/Third/fmt/include)
message(${FMT_INCLUDE})

set(BUFFER_SRC
    Buffer/Buffer.cpp
    Buffer/ByteOrder.h
        Utils/TimeUtils.cpp)
add_library(buffer ${BUFFER_SRC})

set(MESSAGE_SRC
    Message/BaseMessage.cpp
    Message/ControlMessage.h
    Message/EnterRoomMessage.h
    Message/MessageUtils.h
    Message/Messages.cpp
    Message/PingMessage.h
)
add_library(message ${MESSAGE_SRC})
target_link_libraries(message PUBLIC buffer)
target_include_directories(message PUBLIC ${FMT_INCLUDE})

set(UTILS_SRC
    Utils/TimeUtils.cpp
)
add_library(utils ${UTILS_SRC})

set(GAME_SERVER_SRC
    GameServer/GameConnection.cpp
    GameServer/GameServer.cpp
    GameServer/GameServerExample.cpp
    GameServer/main.cpp
    GameServer/MKServer.cpp
    GameServer/MsgDispatcher.cpp
    GameServer/TcpConnection.cpp
)
add_library(server ${GAME_SERVER_SRC})
target_compile_options(server PUBLIC -DASIO_STANDALONE)
target_include_directories(server PUBLIC ${ASIO_INCLUDE})
target_link_libraries(server PUBLIC message utils pthread)

add_executable(GameServerTest TestMain.cpp)
target_link_libraries(GameServerTest PRIVATE server)

