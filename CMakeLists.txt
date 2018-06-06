cmake_minimum_required(VERSION 3.5)
project(pc_2018_04 C)




set(CMAKE_C_FLAGS    "--std=c99 -pedantic -pedantic-errors -Wall -Wextra -Werror -Wno-unused-parameter -Wno-implicit-fallthrough -D_POSIX_C_SOURCE=200112L ")

#set(CMAKE_C_FLAGS "-g -pthread -pedantic -pedantic-errors -Wall -Wextra -Werror -Wno-unused-parameter -D_POSIX_C_SOURCE=200112L")

if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    find_program(CLANGTIDY NAMES clang-tidy)
    if(CLANGTIDY)
        message(STATUS "Found clang-tidy: ${CLANGTIDY}")
        set(CMAKE_C_CLANG_TIDY clang-tidy
                -checks=${CLANG_TIDY_CHECKS}
                -warnings-as-errors=*
                -fix)
    endif()
endif()

set(CMAKE_C_FLAGS, "${CMAKE_C_FLAGS} -pthread -pedantic -pedantic-errors -Wall -Wextra -Werror -Wno-unused-parameter -D_POSIX_C_SOURCE=200112L")

#set(CMAKE_C_FLAGS, "${CMAKE_C_FLAGS} -lpthread")

#add_subdirectory(utils)

add_executable(proxy
        parser/parserHostReqLine.c
        parser/parser.c
        parser/parser_utils.c
        parser/multi_parser.c
        parser/header.c
        parser/headerGroup.c
        parser/body.c
        parser/method.c
        parser/requestLine.c
        parser/request.c
        parser/identity.c
        parser/chunkGroup.c
        parser/chunk.c
        parser/statusLine.c
        parser/response.c
        server.c
        serverComponents/proxyActiveHandlers.c
        serverComponents/proxyPassiveHandlers.c
        serverComponents/proxyStm.c
        utils/buffer/buffer.c
        utils/selector/selector.c
        utils/stm/stm.c
        serverComponents/proxyStates/requestState.c
        serverComponents/proxyStates/responseState.c
        serverComponents/transformationFork.c
        serverComponents/proxyStates/errorState.c)


        add_executable(response
        parser/parser.c
        parser/parser_utils.c
        parser/multi_parser.c
        parser/header.c
        parser/headerGroup.c
        parser/body.c
        parser/method.c
        parser/statusLine.c
        parser/response.c
        parser/response_test.c
        parser/identity.c
        parser/chunkGroup.c
        parser/chunk.c)
