cmake_minimum_required(VERSION 3.29)

project(ib_cli_root)

#set(Boost_USE_STATIC_LIBS On)
#add_definitions(-DBOOST_LOG_DYN_LINK)

add_subdirectory(ib_helper)
add_subdirectory(src)
