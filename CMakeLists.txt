cmake_minimum_required(VERSION 3.20)

project(Ws-Dispatcher
    VERSION 0.1.0
    DESCRIPTION "Asynchronous C++ WebSocket server runtime with thread pool"
    LANGUAGES CXX
)

# -----------------------
# Options
# -----------------------
option(WSDISPATCHER_BUILD_EXAMPLES "Build examples" ON)
# option(WSDISPATCHER_BUILD_TESTS "Build tests" OFF)
# option(WSDISPATCHER_BUILD_SHARED "Build shared library" OFF)

# -----------------------
# C++ standard
# -----------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -----------------------
# Dependencies
# -----------------------
find_package(Boost 1.80 REQUIRED COMPONENTS system)

# -----------------------
# Library target
# -----------------------
set(WSDISPATCHER_SOURCES
    src/server.cpp
    src/session.cpp
    src/dispatcher.cpp
)

add_library(WsDispatcher ${WSDISPATCHER_SOURCES})

# if(WSDISPATCHER_BUILD_SHARED)
    # add_library(WsDispatcher SHARED ${WSDISPATCHER_SOURCES})
# else()
    # add_library(WsDispatcher STATIC ${WSDISPATCHER_SOURCES})
# endif()

add_library(WsDispatcher::WsDispatcher ALIAS WsDispatcher)

# -----------------------
# Include paths
# -----------------------
target_include_directories(WsDispatcher
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:inc>
)

# Included headers with library
# set(WSDISPATCHER_HEADERS
#     src/server.hpp
# )

# install(FILES WSDISPATCHER_HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
# install(EXPORT WsDispatcherTargets
#     FILE WsDispatcherTargets.cmake
#     NAMESPACE WsDispatcher::
#     DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/WsDispatcher
# )

# include(CMakePackageConfigHelpers)

# configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
#     "${CMAKE_CURRENT_BINARY_DIR}/WsDispatcher.cmake"
#     INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/WsDispatcher
# )

# -----------------------
# Link libraries
# -----------------------
target_link_libraries(WsDispatcher
    PRIVATE
        Boost::system
)

# -----------------------
# Compile options
# -----------------------
target_compile_features(WsDispatcher PUBLIC cxx_std_20)

target_compile_options(WsDispatcher
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)

# -----------------------
# Examples
# -----------------------
if(WSDISPATCHER_BUILD_EXAMPLES)
    add_executable(echo src/server.cpp)
    target_link_libraries(echo PRIVATE WsDispatcher::WsDispatcher)
endif()

# -----------------------
# Tests (placeholder)
# -----------------------
if(WSDISPATCHER_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
