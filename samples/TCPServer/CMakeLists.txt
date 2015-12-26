
#if (NOT CMAKE_BUILD_TYPE STREQUAL 'DEBUG')
#	add_definitions(-std=c++11 -gstabs+ ${CMAKE_OPTION})
#	set(CMAKE_EXE_LINKER_FLAGS "-g  -Wall -lpthread -O0")
#else()
#    add_definitions(-DHAVE_EPOLL -g -w -gstabs -std=c++11 ${CMAKE_OPTION})
#    set(CMAKE_EXE_LINKER_FLAGS "-g -w -gstabs -Wall -lpthread")
#endif()
add_definitions(-std=c++11 -g -gdwarf-2 -DHAVE_EPOLL ${CMAKE_OPTION})
set(CMAKE_EXE_LINKER_FLAGS "-lpthread")
aux_source_directory(. SOURCES)
message("TCPServer SOURCES = " ${SOURCES})

add_executable(TCPServer ${SOURCES})

TARGET_LINK_LIBRARIES(TCPServer ${CMAKE_EXE_LINKER_FLAGS})
TARGET_LINK_LIBRARIES(TCPServer qing)
TARGET_LINK_LIBRARIES(TCPServer ${CMAKE_EXE_LINKER_FLAGS} ${LOG_SYS})


