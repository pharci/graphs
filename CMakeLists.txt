cmake_minimum_required(VERSION 3.16)
project(app)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include_directories(include)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

file(GLOB SRC_FILES "src/*.cpp")
file(GLOB HEADER_FILES "include/*.h")
file(GLOB LIB_FILES "lib/*.hpp")


if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/res/application.rc")
endif()

qt_add_executable(${PROJECT_NAME} 
    ${SRC_FILES} 
    ${HEADER_FILES} 
    ${LIB_FILES} 
    ${APP_ICON_RESOURCE_WINDOWS}
)
qt_add_resources(${PROJECT_NAME} "resources"
    PREFIX "/"
    FILES res/icon.ico
)

target_include_directories(${PROJECT_NAME} PRIVATE include lib)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets
)