file(GLOB SOURCES "./*.c")

add_executable(client ${SOURCES})
target_include_directories(client PUBLIC ${CMAKE_CURRENT_LIST_DIR})