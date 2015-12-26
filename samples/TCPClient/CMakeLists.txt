
#if (NOT CMAKE_BUILD_TYPE STREQUAL 'DEBUG')
#	add_definitions(-std=c++11 -DNODEBUG ${CMAKE_OPTION})
#	set(CMAKE_EXE_LINKER_FLAGS "-g -Wall -lpthread -O0")
#else()
#	add_definitions(-DHAVE_EPOLL -g  -gdwarf-2  -std=c++11 ${CMAKE_OPTION})
#	set(CMAKE_EXE_LINKER_FLAGS "-pg -Wall -lpthread -O3")
#endif()
add_definitions(-std=c++11 -g -gdwarf-2 -DHAVE_EPOLL ${CMAKE_OPTION})
set(CMAKE_EXE_LINKER_FLAGS "-lpthread")
aux_source_directory(. SOURCES)
message("TCPClient SOURCES = " ${SOURCES})

add_executable(TCPClient ${SOURCES})

TARGET_LINK_LIBRARIES(TCPClient ${CMAKE_EXE_LINKER_FLAGS})
TARGET_LINK_LIBRARIES(TCPClient qing)
TARGET_LINK_LIBRARIES(TCPClient ${CMAKE_EXE_LINKER_FLAGS} ${LOG_SYS})


