# Create config.h.in template
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

add_subdirectory(controllers)
add_subdirectory(dao)
add_subdirectory(models)
add_subdirectory(network)
add_subdirectory(ui)
add_subdirectory(tests)
add_subdirectory(libs)

# Main executable
qt_add_executable(stock-api-manager
    WIN32 MACOSX_BUNDLE
    main.cpp
    ${FORMS}
    ${RESOURCES}
)

# Link components
target_link_libraries(stock-api-manager PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::WebSockets
    controllers_lib
    dao_lib
    models_lib
    network_lib
    ui_lib
    tests_lib
    fftw3
)

# Include directories
target_include_directories(stock-api-manager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/libs  	# Eigen headers
    ${CMAKE_CURRENT_BINARY_DIR}  	# For generated config.h
)

# Add debug definitions to each library
if(DEBUG_CONTROLLERS_LIB)
    target_compile_definitions(controllers_lib PRIVATE DEBUG_CONTROLLERS_LIB)
endif()

if(DEBUG_DAO_LIB)
    target_compile_definitions(dao_lib PRIVATE DEBUG_DAO_LIB)
endif()

if(DEBUG_MODELS_LIB)
    target_compile_definitions(models_lib PRIVATE DEBUG_MODELS_LIB)
endif()

if(DEBUG_NETWORK_LIB)
    target_compile_definitions(network_lib PRIVATE DEBUG_NETWORK_LIB)
endif()

if(DEBUG_UI_LIB)
    target_compile_definitions(ui_lib PRIVATE DEBUG_UI_LIB)
endif()

if(DEBUG_TESTS_LIB)
    target_compile_definitions(tests_lib PRIVATE DEBUG_TESTS_LIB)
endif()

# Resources and translations
set(RESOURCES ../resources/resources.qrc)
set(FORMS src/ui/mainwindow/mainwindow.ui)

qt_add_translations(stock-api-manager
    TS_FILES ../resources/translations/stock-api-manager_en_US.ts
)

# Installation
include(GNUInstallDirs)
install(TARGETS stock-api-manager
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION /opt/stock-api-manager/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Deployment script
qt_generate_deploy_app_script(
    TARGET stock-api-manager
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
