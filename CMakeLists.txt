cmake_minimum_required(VERSION 3.10)
project(pc_2018_04 C)

set(CMAKE_C_STANDARD 11)

#include_directories(utils)
#include_directories(utils/buffer)
#include_directories(utils/selector)
#


set(CMAKE_C_FLAGS    "--std=c99 -pedantic -pedantic-errors -Wall -Wextra -Werror -Wno-unused-parameter -Wno-implicit-fallthrough -D_POSIX_C_SOURCE=200112L ")

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
set(CMAKE_C_FLAGS, "${CMAKE_C_FLAGS} -pedantic -pedantic-errors -Wall -Wextra
-Werror -Wno-unused-parameter -D_POSIX_C_SOURCE=200112L")

#add_subdirectory(utils)

add_executable(requestLine
        utils/parser/method.c
        utils/parser/parser.c
        utils/parser/parser_utils.c
        utils/parser/requestLine.c
        utils/parser/requestLine_test.c)

add_executable(multiParser
        utils/parser/parser.c
        utils/parser/parser_utils.c
        utils/parser/multi_parser.c
        utils/parser/multi_parser_test.c)

add_executable(header
        utils/parser/parser.c
        utils/parser/parser_utils.c
        utils/parser/multi_parser.c
        utils/parser/header.c
        utils/parser/header_test.c)

add_executable(headerGroup
        utils/parser/parser.c
        utils/parser/parser_utils.c
        utils/parser/multi_parser.c
        utils/parser/header.c
        utils/parser/headerGroup.c
        utils/parser/headerGroup_test.c)

add_executable(body
        utils/parser/body.c
        utils/parser/body_test.c
        utils/parser/identity.c
        utils/parser/chunkGroup.c
        utils/parser/chunk.c)

add_executable(request
        utils/parser/parserHostReqLine.c
        utils/parser/parser.c
        utils/parser/parser_utils.c
        utils/parser/multi_parser.c
        utils/parser/header.c
        utils/parser/headerGroup.c
        utils/parser/body.c
        utils/parser/method.c
        utils/parser/requestLine.c
        utils/parser/request.c
        utils/parser/request_test.c
        utils/parser/identity.c
        utils/parser/chunkGroup.c
        utils/parser/chunk.c)

add_executable(response
        utils/parser/parser.c
        utils/parser/parser_utils.c
        utils/parser/multi_parser.c
        utils/parser/header.c
        utils/parser/headerGroup.c
        utils/parser/body.c
        utils/parser/method.c
        utils/parser/statusLine.c
        utils/parser/response.c
        utils/parser/response_test.c
        utils/parser/identity.c
        utils/parser/chunkGroup.c
        utils/parser/chunk.c)


add_executable(identity
        utils/parser/identity.c)

add_executable(parserHostreqline
        utils/parser/parserHostReqLine.c
        utils/parser/parserHostReqLine_test.c)

add_executable(parseint
        utils/parser/funcionParaSegundito.c)

