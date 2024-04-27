cmake_minimum_required(VERSION 3.13)
project(music)

include_directories(./src ./etc)

add_executable(playback ./src/playback.cpp)
target_link_libraries(playback pulse-simple pulse m ssm)

add_executable(record ./src/record.cpp)
target_link_libraries(record pulse-simple pulse m ssm)

add_executable(dplayback ./etc/dplayback.cpp)
target_link_libraries(dplayback pulse-simple pulse m ssm pthread)

#add_executable(pulsesound ./src/pulsesound.cpp)
#target_link_libraries(pulsesound pulse-simple pulse m ssm)
