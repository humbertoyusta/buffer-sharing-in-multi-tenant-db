find_package(yaml-cpp REQUIRED)

add_library(src
    types/tenant.cpp
    types/page_access.cpp
    input_reader.cpp
    single_lru_cache.cpp
    checker.cpp
    solutions/cache_eviction_algorithms/lru.cpp
    solutions/lru_policy_1_solution.cpp
    solutions/lru_policy_2_solution.cpp
    scorer.cpp
)

target_link_libraries(src PUBLIC yaml-cpp::yaml-cpp)

target_include_directories(src PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(main main.cpp)
target_link_libraries(main src)