cmake_minimum_required(VERSION 3.5)

project(join_server)

find_package(Threads)

add_executable(${PROJECT_NAME} 
                main.cpp 
                sqlite3.c
                server/sql_server.cpp
                server/sql_server.h
                server/sql_session.cpp
                server/sql_session.h
                sql_handler/sql_handler.cpp
                sql_handler/sql_handler.h
                )

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        )

find_package(Boost COMPONENTS system REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME}
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES}
        dl
        )