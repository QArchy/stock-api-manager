add_library(tests_lib
    tests.h
    loggerTest/loggerTest.h
    loggerTest/loggerworker.h
    loggerTest/loggerworker.cpp
)

target_include_directories(tests_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(tests_lib PUBLIC
    Qt6::Core
    controllers_lib
)
