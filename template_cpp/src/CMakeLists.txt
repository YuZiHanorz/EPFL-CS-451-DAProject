# DO NOT CHANGE THE SYMBOLIC VARIABLE `SOURCES`.
# You can, however, change the list of files that comprise this variable.

include_directories(include)
set(SOURCES src/main.cpp src/hello.c src/util.cpp src/sender.cpp src/receiver.cpp 
            src/process.cpp src/perfectLink.cpp src/beb.cpp src/urb.cpp src/fiforb.cpp src/latagr.cpp)

# DO NOT EDIT THE FOLLOWING LINES
find_package(Threads)
add_executable(da_proc ${SOURCES})
target_link_libraries(da_proc ${CMAKE_THREAD_LIBS_INIT})
