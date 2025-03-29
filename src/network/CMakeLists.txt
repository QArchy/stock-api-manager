add_library(network_lib
    HttpManager/httpmanager.cpp
    HttpManager/httpmanager.h
    WebSocketManager/websocketmanager.cpp
    WebSocketManager/websocketmanager.h
    nam.h
)

target_include_directories(network_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(network_lib PUBLIC
    Qt6::Network
    Qt6::WebSockets
)
