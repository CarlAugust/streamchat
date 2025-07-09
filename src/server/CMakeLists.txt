file(GLOB SOURCES "./*.c")

add_executable(server ${SOURCES})
target_include_directories(server PUBLIC ${CMAKE_CURRENT_LIST_DIR})