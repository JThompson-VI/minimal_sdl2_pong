cmake_minimum_required(VERSION 3.12)

set(PROJECT_NAME sdl2_pong)
project(${PROJECT_NAME})

set(SOURCE_FILES src/main.cpp src/utils/colors.cpp src/utils/misc.cpp)
set(HEADER_FILES src/utils/colors.hpp src/utils/misc.hpp)

add_executable(
  ${PROJECT_NAME}
  ${HEADER_FILES}
  ${SOURCE_FILES}
  )

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR})

include_directories(/opt/homebrew/Cellar/sdl2/2.28.2/include)
link_directories(/opt/homebrew/Cellar/sdl2/2.28.2/lib)
target_link_libraries(${PROJECT_NAME} /opt/homebrew/Cellar/sdl2/2.28.2/lib/libSDL2.dylib)

