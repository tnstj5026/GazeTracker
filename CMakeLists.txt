
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(TARGET_NAME	EyeX)
SET(PROJECT_NAME ${TARGET_NAME}_Proj)

PROJECT(${PROJECT_NAME})

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

#######################################################################################

SET(EyeX_SRCS
	./src/main.cpp
	./src/mainwindow.cpp
	./src/controlwidget.cpp
	./src/keyboardwidget.cpp
	./src/picopng.cpp
)

SET(EyeX_HEADERS
	./src/mainwindow.h
	./src/controlwidget.h
	./src/keyboardwidget.h
	./src/gazetracker.h
)

SET(EyeX_MOC_HDRS)

SET(ALL_FILES
	${EyeX_SRCS} 
	${EyeX_HEADERS}
)

FILE(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/Release")

#######################################################################################

SET(QT_USE_QT3SUPPORT TRUE)
SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTCORE TRUE)
SET(QT_USE_QTOPENGL TRUE)

SET(QT_USE_QTMAIN TRUE)
SET(QT_USE_QTGUI TRUE)
SET(QT_USE_QTWIDGETS TRUE)

FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5OpenGL REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Concurrent REQUIRED)

GET_TARGET_PROPERTY(Qt5Core_location Qt5::Core LOCATION)

INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Core_LIBRARIES})
INCLUDE_DIRECTORIES(${Qt5Gui_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5OpenGL_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})

FIND_PACKAGE(OpenGL)

#######################################################################################

SET(TOBII_EYEX_INCLUDE_DIR ./include/ CACHE STRING "Build mode type.")
SET(TOBII_EYEX_DLL64 ./lib/x64/Tobii.EyeX.Client.dll)

# Searching for library files
FIND_LIBRARY(TOBII_EYEX_LIB Tobii.EyeX.Client
	HINTS ./lib/x64)

FIND_PATH(TOBII_EYEX_DLL_DIR Tobii.EyeX.Client.dll
	HINTS ./lib/x64)

IF (TOBII_EYEX_INCLUDE_DIR AND TOBII_EYEX_LIB AND TOBII_EYEX_DLL_DIR)
    # set the appropriate flag
    SET(TOBII_EYEX_FOUND TRUE)

    # add definition for conditional compilation
    ADD_DEFINITIONS(-DHAVE_LIBEYEX=1)

    # include the lib directory
    INCLUDE_DIRECTORIES(${TOBII_EYEX_INCLUDE_DIR})

    # append the lib directory to a variable
    SET(ALL_LIBS ${ALL_LIBS} ${TOBII_EYEX_LIB})
ENDIF ()

ADD_LIBRARY(Tobii.EyeX.Client ${ALL_FILES})

#######################################################################################

SOURCE_GROUP("Source Files" FILES ${EyeX_SRCS})
SOURCE_GROUP("Header Files" FILES ${EyeX_HEADERS})

#######################################################################################

QT5_WRAP_CPP(EyeX_MOC_SRCS ${EyeX_MOC_HDRS})
SOURCE_GROUP("Generated Files" FILES ${EyeX_MOC_SRCS})

#######################################################################################

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${Qt5Concurrent_INCLUDE_DIRS})

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(${Qt5Concurrent_DEFINITIONS})

#######################################################################################

ADD_EXECUTABLE(${TARGET_NAME}
	${ALL_FILES}
	${EyeX_MOC_SRCS}
)

SET(EyeX_QtLibs 
	Qt5Core
	Qt5Gui
	Qt5Widgets
	Qt5OpenGL
)

QT5_USE_MODULES(${TARGET_NAME} Widgets Core OpenGL Gui Multimedia MultimediaWidgets)

TARGET_LINK_LIBRARIES(${TARGET_NAME}
	${OPENGL_LIBRARIES}
	Qt5::Core
	${TOBII_EYEX_LIB}
	${Qt5Concurrent_LIBRARIES}
)

#######################################################################################

FILE(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/Data")

FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/img"
        DESTINATION "${CMAKE_CURRENT_BINARY_DIR}"
)

FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/Tobii.EyeX.Client.dll"
        DESTINATION "${CMAKE_CURRENT_BINARY_DIR}"
)

FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/Tobii.EyeX.Client.lib"
        DESTINATION "${CMAKE_CURRENT_BINARY_DIR}"
)
