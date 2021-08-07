# *************************************************************************************************
# Copyright (c) 2021 Calypso Networks Association                                                 *
# https://www.calypsonet-asso.org/                                                                *
#                                                                                                 *
# See the NOTICE file(s) distributed with this work for additional information regarding          *
# copyright ownership.                                                                            *
#                                                                                                 *
# This program and the accompanying materials are made available under the terms of the Eclipse   *
# Public License 2.0 which is available at http://www.eclipse.org/legal/epl-2.0                   *
#                                                                                                 *
# SPDX-License-Identifier: EPL-2.0                                                                *
# *************************************************************************************************/

PROJECT(KeypleServiceCppLib C CXX)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(VERSION_MAJOR "2")
SET(VERSION_MINOR "0")
SET(VERSION_PATCH "0")

SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

SET(PACKAGE_NAME "keyple-service-cpp-lib")
SET(PACKAGE_VERSION ${VERSION})
SET(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")

SET(CMAKE_MACOSX_RPATH 1)
SET(CMAKE_CXX_STANDARD 11)

SET(EXECTUABLE_NAME keypleservicecpplib_ut)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")

SET(CALYPSONET_CARD_DIR    "../../../github.com.calypsonet.calypsonet-terminal-card-cpp-api")
SET(CALYPSONET_READER_DIR  "../../../github.com.calypsonet.calypsonet-terminal-reader-cpp-api")
SET(KEYPLE_COMMON_DIR      "../../../github.com.calypsonet.keyple-common-cpp-api")
SET(KEYPLE_PLUGIN_DIR      "../../../github.com.calypsonet.keyple-plugin-cpp-api")
SET(KEYPLE_SERVICE_DIR     "../..")
SET(KEYPLE_SERVICE_LIB     "keypleservicecpplib")
SET(KEYPLE_UTIL_DIR        "../../../github.com.calypsonet.keyple-util-cpp-lib")
SET(KEYPLE_UTIL_LIB        "keypleutilcpplib")

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../main
    ${CMAKE_CURRENT_SOURCE_DIR}/../main/cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../main/spi

    ${CALYPSONET_CARD_DIR}/src/main
    ${CALYPSONET_CARD_DIR}/src/main/spi

    ${CALYPSONET_READER_DIR}/src/main
    ${CALYPSONET_READER_DIR}/src/main/selection
    ${CALYPSONET_READER_DIR}/src/main/selection/spi
    ${CALYPSONET_READER_DIR}/src/main/spi

    ${KEYPLE_COMMON_DIR}/src/main

    ${KEYPLE_PLUGIN_DIR}/src/main
    ${KEYPLE_PLUGIN_DIR}/src/main/spi
    ${KEYPLE_PLUGIN_DIR}/src/main/spi/reader
    ${KEYPLE_PLUGIN_DIR}/src/main/spi/reader/observable/
    ${KEYPLE_PLUGIN_DIR}/src/main/spi/reader/observable/state/insertion
    ${KEYPLE_PLUGIN_DIR}/src/main/spi/reader/observable/state/processing
    ${KEYPLE_PLUGIN_DIR}/src/main/spi/reader/observable/state/removal

    ${KEYPLE_UTIL_DIR}/src/main
    ${KEYPLE_UTIL_DIR}/src/main/cpp
    ${KEYPLE_UTIL_DIR}/src/main/cpp/exception
)

ADD_EXECUTABLE(
    ${EXECTUABLE_NAME}

    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractReaderAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AutonomousObservableLocalPluginAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardSelectionResultAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LocalPluginAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LocalPoolPluginAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LocalReaderAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableLocalPluginAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MainTest.cpp
)

FIND_LIBRARY(UTIL ${KEYPLE_UTIL_LIB} HINTS ${KEYPLE_UTIL_DIR}/build/bin)
FIND_LIBRARY(SERVICE ${KEYPLE_SERVICE_LIB} HINTS ${KEYPLE_SERVICE_DIR}/build/bin)

TARGET_LINK_LIBRARIES(${EXECTUABLE_NAME} gtest gmock ${UTIL} ${SERVICE})
