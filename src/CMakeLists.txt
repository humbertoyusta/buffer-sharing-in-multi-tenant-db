find_package(yaml-cpp REQUIRED)

add_library(src
    types/tenant.cpp
    types/page_access.cpp
    input_reader.cpp
    single_lru_cache.cpp
    checker.cpp
    solutions/cache_eviction_algorithms/lru.cpp
    solutions/cache_eviction_algorithms/lru_2.cpp
    solutions/cache_eviction_algorithms/lfu.cpp
    solutions/cache_eviction_algorithms/_2q.cpp
    solutions/cache_eviction_algorithms/lrfu.cpp
    solutions/cache_eviction_algorithms/mq.cpp
    solutions/cache_eviction_algorithms/belady.cpp
    solutions/cache_eviction_algorithms/lirs.cpp
    solutions/lru_policy_1_solution.cpp
    solutions/lru_policy_2_solution.cpp
    solutions/lru_policy_3_solution.cpp
    solutions/belady_solution.cpp
    solutions/lru_2_solution.cpp
    solutions/_2q_solution.cpp
    solutions/lrfu_solution.cpp
    solutions/lfu_policy_1_solution.cpp
    solutions/lfu_policy_2_solution.cpp
    solutions/mq_solution.cpp
    solutions/lirs_solution.cpp
    solutions/naive_lru_solution.cpp
    scorer.cpp
)

target_link_libraries(src PUBLIC yaml-cpp::yaml-cpp)

target_include_directories(src PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(main main.cpp)
target_link_libraries(main src)

add_executable(tuner tuner.cpp)
target_link_libraries(tuner src)

add_executable(hit_ratio_per_cache_size_exp hit_ratio_per_cache_size_exp.cpp)
target_link_libraries(hit_ratio_per_cache_size_exp src)

add_executable(fault_and_hit_ratio_exp fault_and_hit_ratio_exp.cpp)
target_link_libraries(fault_and_hit_ratio_exp src)