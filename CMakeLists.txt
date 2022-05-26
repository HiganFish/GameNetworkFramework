cmake_minimum_required(VERSION 3.19)
project(GameProject)

set(CMAKE_CXX_STANDARD 17)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS -stdlib=libc++)
endif()

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
    Buffer/ByteOrder.h)
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
    Utils/ThreadSafeQueue.h
)
add_library(utils ${UTILS_SRC})

set(NETWORK
    Network/GameConnection.cpp
    Network/MsgDispatcher.cpp
    Network/TcpConnection.cpp
)
add_library(network ${NETWORK})
target_include_directories(network PUBLIC ${FMT_INCLUDE} ${ASIO_INCLUDE})

set(GAME_CLIENT_SRC
    GameClient/GameClient.cpp
)
add_library(client STATIC ${GAME_CLIENT_SRC})
target_compile_options(client PUBLIC -DASIO_STANDALONE)
target_include_directories(client PRIVATE ${ASIO_INCLUDE})
if (WIN32)
    target_link_libraries(client PRIVATE network message utils wsock32 ws2_32)
else()
    target_link_libraries(client PRIVATE network message utils pthread)
endif()


set(GAME_SERVER_SRC
    GameServer/GameServer.cpp
    GameServer/GameServerExample.cpp
    GameServer/MKServer.cpp
)
add_library(server STATIC ${GAME_SERVER_SRC})
target_compile_options(server PRIVATE -DASIO_STANDALONE)
target_include_directories(server PRIVATE ${ASIO_INCLUDE})
if (WIN32)
    target_link_libraries(server PRIVATE network message utils wsock32 ws2_32)
else()
    target_link_libraries(server PRIVATE network message utils pthread)
endif()

add_executable(GameServerTest TestMain.cpp)
target_include_directories(GameServerTest PUBLIC ${ASIO_INCLUDE} ${FMT_INCLUDE})
target_link_libraries(GameServerTest PUBLIC server client)

