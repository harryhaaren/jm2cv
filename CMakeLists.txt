cmake_minimum_required(VERSION 2.0)

PROJECT(jm2cv)

INCLUDE( ${CMAKE_ROOT}/Modules/FindPkgConfig.cmake )
INCLUDE( ${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake )

#pkg_check_modules(LV2CORE lv2core)
pkg_check_modules(JACK jack>=0.118)
#pkg_check_modules(GTKMM gtkmm-2.4>=2.4)
#pkg_check_modules(CAIROMM cairomm-1.0>=1.0)
#pkg_check_modules(DBUS dbus-glib-1)
#pkg_check_modules(PCRE libpcrecpp)

#ADD_CUSTOM_COMMAND(
#	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/jsweeper.ui
#	COMMAND gtk-builder-convert ${CMAKE_CURRENT_SOURCE_DIR}/src/jsweeper.glade ${CMAKE_CURRENT_BINARY_DIR}/jsweeper.ui
#	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/jsweeper.glade
#)

SET(SOURCES
	client.cpp
	client.h
	cvin.cpp
	cvin.h
	cvout.cpp
	cvout.h
	jm2cv.cpp
)

LINK_DIRECTORIES(
#	${LV2CORE_LIBRARY_DIRS}
	${JACK_LIBRARY_DIRS}
#	${GTKMM_LIBRARY_DIRS}
#	${CAIROMM_LIBRARY_DIRS}
#	${DBUS_LIBRARY_DIRS}
#	${PCRE_LIBRARY_DIRS}
)

INCLUDE_DIRECTORIES(
#	${LV2CORE_INCLUDE_DIRS}
	${JACK_INCLUDE_DIRS}
#	${GTKMM_INCLUDE_DIRS}
#	${CAIROMM_INCLUDE_DIRS}
#	${DBUS_INCLUDE_DIRS}
#	${PCRE_INCLUDE_DIRS}
)

ADD_DEFINITIONS(-g -Wall -Wextra -pedantic -D_GNU_SOURCE)
ADD_EXECUTABLE(jm2cv ${SOURCES})

TARGET_LINK_LIBRARIES(jm2cv
	-lm
#	${LV2CORE_LIBRARIES}
	${JACK_LIBRARIES}
#	${GTKMM_LIBRARIES}
#	${CAIROMM_LIBRARIES}
#	${DBUS_LIBRARIES}
#	${PCRE_LIBRARIES}
)

INSTALL(PROGRAMS jm2cv DESTINATION bin)
#INSTALL(FILES jsweeper.ui DESTINATION share/jsweeper)
