cmake_minimum_required(VERSION 3.15)
project(r2d2 VERSION 1.0
                  DESCRIPTION "R2D2 control service"
                  LANGUAGES CXX)
                  set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/src/include)


set(project_sources
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/logger/logger.cpp
    ${PROJECT_SOURCE_DIR}/src/server/server.cpp
    ${PROJECT_SOURCE_DIR}/src/arduino_connector/arduino_connector.cpp
    ${PROJECT_SOURCE_DIR}/src/soundpad/sound_player.cpp
)

set(project_headers
    ${PROJECT_SOURCE_DIR}/src/logger/logger.hpp
    ${PROJECT_SOURCE_DIR}/src/server/server.hpp
    ${PROJECT_SOURCE_DIR}/src/server/resolvers.hpp
    ${PROJECT_SOURCE_DIR}/src/arduino_connector/arduino_connector.hpp
    ${PROJECT_SOURCE_DIR}/src/soundpad/sound_player.hpp
)

add_executable(
    ${PROJECT_NAME}
    ${project_sources}
    ${project_headers}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    dl
    pthread
    m
)
