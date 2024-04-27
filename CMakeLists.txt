cmake_minimum_required(VERSION 3.13)
project(music)
add_executable(playmusic ./src/pmusic.cpp)
target_link_libraries(playmusic pulse-simple pulse m ssm)

add_executable(recordmusic ./src/recordmusic.cpp)
target_link_libraries(recordmusic pulse-simple pulse m ssm)