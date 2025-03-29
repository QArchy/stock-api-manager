cmake_minimum_required(VERSION 3.19)
project(stock-api-manager LANGUAGES CXX)

# Add debug options for each module
option(DEBUG_CONTROLLERS_LIB "Enable debug for controllers" OFF)
option(DEBUG_DAO_LIB "Enable debug for DAO" OFF)
option(DEBUG_MODELS_LIB "Enable debug for models" OFF)
option(DEBUG_NETWORK_LIB "Enable debug for network" OFF)
option(DEBUG_UI_LIB "Enable debug for UI" OFF)
option(DEBUG_TESTS_LIB "Enable debug for tests" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.8.2 REQUIRED COMPONENTS
    Core Gui Widgets Network WebSockets LinguistTools
)

qt_standard_project_setup()
add_subdirectory(src)
