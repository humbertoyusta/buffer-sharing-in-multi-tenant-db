add_library(src
    types/tenant.cpp
    types/page_access.cpp
    input_reader.cpp
)

target_include_directories(src PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(main main.cpp)
target_link_libraries(main src)