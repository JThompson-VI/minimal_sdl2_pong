cmake_minimum_required(VERSION 3.12)

set(PROJECT_NAME sdl2_pong)
project(${PROJECT_NAME})

set(SOURCE_FILES src/main.c src/utils/colors.c src/utils/misc.c)
set(HEADER_FILES src/utils/colors.h src/utils/misc.h)

add_executable(
  ${PROJECT_NAME}
  ${HEADER_FILES}
  ${SOURCE_FILES}
  )

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)


include_directories(/opt/homebrew/Cellar/sdl2/2.28.2/include)
link_directories(/opt/homebrew/Cellar/sdl2/2.28.2/lib)
target_link_libraries(${PROJECT_NAME} /opt/homebrew/Cellar/sdl2/2.28.2/lib/libSDL2.dylib)

