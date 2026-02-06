cmake_minimum_required(VERSION 3.16)

project(mailadler
    VERSION 0.1.0
    DESCRIPTION "E-Mail Client"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    Sql
)

add_executable(mailadler WIN32
    main.cpp
    imapconnection.cpp
    imapconnection.h
    accountsettings.cpp
    accountsettings.h
)

target_link_libraries(mailadler PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Sql
)

# Windows-spezifisch
if(WIN32)
    set_target_properties(mailadler PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
