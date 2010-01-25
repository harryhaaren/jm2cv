cmake_minimum_required(VERSION 2.0)

PROJECT(jm2cv)

INCLUDE( ${CMAKE_ROOT}/Modules/FindPkgConfig.cmake )
INCLUDE( ${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake )

pkg_check_modules(JACK jack>=0.118)

SET(SOURCES
	client.cpp
	client.h
	cvbase.cpp
	cvbase.h
	cvin.cpp
	cvin.h
	cvout.cpp
	cvout.h
	jm2cv.cpp
)

LINK_DIRECTORIES(
	${JACK_LIBRARY_DIRS}
)

INCLUDE_DIRECTORIES(
	${JACK_INCLUDE_DIRS}
)

ADD_DEFINITIONS(-g -Wall -Wextra -pedantic -D_GNU_SOURCE)
ADD_EXECUTABLE(jm2cv ${SOURCES})

TARGET_LINK_LIBRARIES(jm2cv
	-lm
	${JACK_LIBRARIES}
)

INSTALL(PROGRAMS jm2cv DESTINATION bin)
