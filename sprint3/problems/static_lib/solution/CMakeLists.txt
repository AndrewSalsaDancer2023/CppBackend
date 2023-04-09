cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)
set(CONAN_DISABLE_CHECK_COMPILER TRUE CACHE BOOL "" FORCE )
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
conan_basic_setup(TARGETS)

find_package(Boost 1.78.0 REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Добавляем библиотеку, указывая, что она статическая.
add_library(MyLib STATIC 
	src/model.h
	src/model.cpp
	src/tagged.h
	src/sdk.h
	src/boost_json.cpp
	
	src/json_loader.h
	src/json_loader.cpp
	src/json_serializer.h
	src/json_serializer.cpp
	
	src/dog.cpp
	src/dog.h
	src/game_session.cpp
	src/game_session.h
	
	src/event_logger.cpp
	src/event_logger.h
	
	src/player_tokens.cpp
	src/player_tokens.h
	src/server_exceptions.h
	
	src/ticker.h
	src/loot_generator.cpp
	src/loot_generator.h
	src/utils.h
)

# они должны быть ввидны и в библиотеке MyLib и в зависимостях.

target_link_libraries(MyLib PUBLIC Threads::Threads CONAN_PKG::boost)
target_include_directories(MyLib PUBLIC CONAN_PKG::boost)

add_executable(game_server
	src/main.cpp
	src/http_server.cpp
	src/http_server.h

	src/request_handler.cpp
	src/request_handler.h

	src/api_handler.cpp
	src/api_handler.h
)

#add_executable(game_test 
#	tests/loot_generator_tests.cpp
#)


target_link_libraries(game_server PRIVATE MyLib)
#target_link_libraries(game_test PRIVATE MyLib)
