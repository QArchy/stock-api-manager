add_library(dao_lib
    OrderBook/orderbook.cpp
    OrderBook/orderbook.h
)

target_include_directories(dao_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/..
)

target_link_libraries(dao_lib PUBLIC
    Qt6::Core
    eigen_headers
)
