cmake_minimum_required(VERSION 3.6)
project(hearts)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        Card.cpp
        Card.h
        Deck.cpp
        Deck.h
        Deck_iterator.cpp
        Game.cpp
        Game.h
        Game_Hand.cpp
        Game_Hand.h
        Gui.cpp
        Gui.h
        main.cpp
        Text_UI.cpp
        Text_UI.h)

add_executable(hearts ${SOURCE_FILES})

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED graphics window system)
if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(hearts ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
endif()

# -pthread
FIND_PACKAGE ( Threads REQUIRED )
target_link_libraries(hearts ${CMAKE_THREAD_LIBS_INIT})