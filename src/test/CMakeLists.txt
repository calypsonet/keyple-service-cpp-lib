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

SET(EXECTUABLE_NAME keypleservicecpplib_ut)

SET(CALYPSONET_CARD_DIR    "../../../calypsonet-terminal-card-cpp-api")
SET(CALYPSONET_READER_DIR  "../../../calypsonet-terminal-reader-cpp-api")
SET(KEYPLE_COMMON_DIR      "../../../keyple-common-cpp-api")
SET(KEYPLE_PLUGIN_DIR      "../../../keyple-plugin-cpp-api")
SET(KEYPLE_SERVICE_DIR     "../..")
SET(KEYPLE_SERVICE_LIB     "keypleservicecpplib")
SET(KEYPLE_UTIL_DIR        "../../../keyple-util-cpp-lib")
SET(KEYPLE_UTIL_LIB        "keypleutilcpplib")

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/mock
    ${CMAKE_CURRENT_SOURCE_DIR}/util
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
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableLocalReaderAutonomousAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableLocalReaderAutonomousAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableLocalReaderBlockingAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableLocalReaderNonBlockingAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableLocalReaderSelectionScenarioTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SmartCardServiceAdapterTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MainTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/util/ReaderAdapterTestUtils.cpp
)

# Add Google Test
SET(GOOGLETEST_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
INCLUDE(CMakeLists.txt.googletest)

TARGET_LINK_LIBRARIES(${EXECTUABLE_NAME} gtest gmock ${KEYPLE_UTIL_LIB} ${KEYPLE_SERVICE_LIB})
