cmake_minimum_required(VERSION 3.20)

project(TripMaster C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(TripMaster
        src/main.c
        src/booking.c
        src/fileio.c
        src/guest.c
        src/report.c
        src/room.c
        src/utils.c
)

target_include_directories(TripMaster PRIVATE src)