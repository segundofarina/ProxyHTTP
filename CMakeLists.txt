cmake_minimum_required(VERSION 3.5)
project(pc_2018_04 C)




set(CMAKE_C_FLAGS    "-g -fsanitize=address -fno-omit-frame-pointer --std=c99 -pthread -pedantic -pedantic-errors -Wall -Wextra -Werror -Wno-unused-parameter -Wno-implicit-fallthrough -D_POSIX_C_SOURCE=200112L ")

set(CLANG_TIDY_CHECKS *
   -boost-*
   -mpi-*
   -llvm-*
   -cppcoreguidelines-*
   -clang-analyzer-osx.*
   -cert-*-cpp
   -misc-unused-parameters,
   -clang-diagnostic-missing-declarations)
string(REPLACE ";" "," CLANG_TIDY_CHECKS "${CLANG_TIDY_CHECKS}")

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


add_executable(proxy
        parser/parserHostReqLine.c
        parser/parser.c
        parser/parser_utils.c
        parser/multi_parser.c
        parser/header.c
        parser/headerGroup.c
        parser/body.c
        parser/requestLine.c
        parser/request.c
        parser/identity.c
        parser/chunkGroup.c
        parser/chunk.c
        parser/statusLine.c
        parser/response.c
        parser/parsing_utils.c
        parser/response_manager.c
        server.c
        serverComponents/proxyActiveHandlers.c
        serverComponents/proxyPassiveHandlers.c
        serverComponents/proxyStm.c
        utils/buffer/buffer.c
        utils/selector/selector.c
        utils/stm/stm.c
        utils/transformation/transformation.c
        serverComponents/proxyStates/requestState.c
        serverComponents/proxyStates/responseState.c
        serverComponents/transformationFork.c
        serverComponents/proxyStates/errorState.c
        serverComponents/adminActiveHandlers.c
        serverComponents/adminPassiveHandlers.c
        serverComponents/adminStm.c
        serverComponents/adminStates/authenticateReadState.c
        serverComponents/adminStates/authenticateWriteState.c
        serverComponents/adminStates/adminReadState.c
        serverComponents/adminStates/adminWriteState.c
        serverComponents/adminStates/adminErrorState.c
        serverComponents/maxFdHandler.c
        serverComponents/metrics.c
        serverComponents/transformationManager.c
<<<<<<< HEAD
        logger/logger.c
        serverComponents/adminStates/adminParser/processAdminRequest.c)
=======
        serverComponents/transErrorManager.c
        logger/logger.c)
>>>>>>> 4bee4ce4320a11a178a7823c73b7ee674f043599


add_executable(requestLine
        parser/multi_parser.c
        parser/parser.c
        parser/parser_utils.c
        parser/requestLine.c
        parser/requestLine_test.c)

add_executable(multiParser
        parser/parser.c
        parser/parser_utils.c
        parser/multi_parser.c
        parser/multi_parser_test.c)

add_executable(header
        parser/parser.c
        parser/parser_utils.c
        parser/multi_parser.c
        parser/header.c
        parser/header_test.c)

add_executable(headerGroup
        parser/parser.c
        parser/parser_utils.c
        parser/multi_parser.c
        parser/header.c
        parser/headerGroup.c
        parser/headerGroup_test.c)

add_executable(body
        parser/body.c
        parser/body_test.c
        parser/identity.c
        parser/chunkGroup.c
        parser/chunk.c)

add_executable(request
        parser/parserHostReqLine.c
        parser/parser.c
        parser/parser_utils.c
        parser/multi_parser.c
        parser/header.c
        parser/headerGroup.c
        parser/body.c
        parser/requestLine.c
        parser/request.c
        parser/request_test.c
        parser/identity.c
        parser/chunkGroup.c
        parser/chunk.c
        parser/parsing_utils.c)

add_executable(response
        parser/parser.c
        parser/parser_utils.c
        parser/multi_parser.c
        parser/header.c
        parser/headerGroup.c
        parser/body.c
        parser/statusLine.c
        parser/response.c
        parser/response_test.c
        parser/identity.c
        parser/chunkGroup.c
        parser/chunk.c
        parser/parsing_utils.c)


add_executable(manager
        parser/parser.c
        parser/parser_utils.c
        parser/multi_parser.c
        parser/header.c
        parser/headerGroup.c
        parser/body.c
        parser/statusLine.c
        parser/response.c
        parser/response_manager.c
        parser/response_manager_test.c
        parser/identity.c
        parser/chunkGroup.c
        parser/chunk.c
        parser/parsing_utils.c)


add_executable(parserHostreqline
        parser/parserHostReqLine.c
        parser/parserHostReqLine_test.c)

add_executable(admin
        admin.c)



