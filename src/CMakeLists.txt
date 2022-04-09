add_library(obc2_lib
    run.cpp
)

target_include_directories(obc2_lib PUBLIC .)

target_link_libraries(obc2_lib PRIVATE ccl)