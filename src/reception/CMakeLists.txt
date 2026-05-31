add_library(reception STATIC
    kitchenHandle/KitchenHandle.cpp
    loadBalancer/LoadBalancer.cpp
)

target_include_directories(reception PUBLIC ..)
target_link_libraries(reception PUBLIC concurrency ipc pizza kitchen logger)
