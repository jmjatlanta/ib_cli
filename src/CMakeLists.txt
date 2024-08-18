cmake_minimum_required(VERSION 3.29)

project (ib_cli)

find_package(BOOST COMPONENTS log log_setup REQUIRED)

add_executable(ib_cli
    main.cpp
)

include_directories(ib_cli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../ib_helper/src/ib_helper
    ${IB_CLIENT_DIR}
)

target_link_libraries(ib_cli PRIVATE
    ib_helper
    ib_api
    dfp
    ${Boost_LIBRARIES}
)

