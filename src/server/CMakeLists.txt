file(GLOB SOURCES "*.c")
message(STATUS "Server sources: ${SOURCES}")
add_executable(server ${SOURCES})
target_include_directories(server PUBLIC ${CMAKE_CURRENT_LIST_DIR})