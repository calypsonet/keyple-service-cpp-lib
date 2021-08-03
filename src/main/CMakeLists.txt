#/*************************************************************************************************
# * Copyright (c) 2021 Calypso Networks Association https://calypsonet.org/                       *
# *                                                                                               *
# * See the NOTICE file(s) distributed with this work for additional information regarding        *
# * copyright ownership.                                                                          *
# *                                                                                               *
# * This program and the accompanying materials are made available under the terms of the Eclipse *
# * Public License 2.0 which is available at http://www.eclipse.org/legal/epl-2.0                 *
# *                                                                                               *
# * SPDX-License-Identifier: EPL-2.0                                                              *
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

SET(LIBRARY_NAME keypleservicecpplib)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DKEYPLESERVICE_EXPORT")

SET(CALYPSONET_CARD_DIR    "../../../github.com.calypsonet.calypsonet-terminal-card-cpp-api")
SET(CALYPSONET_READER_DIR  "../../../github.com.calypsonet.calypsonet-terminal-reader-cpp-api")
SET(KEYPLE_COMMON_DIR      "../../../github.com.calypsonet.keyple-common-cpp-api")
SET(KEYPLE_PLUGIN_DIR      "../../../github.com.calypsonet.keyple-plugin-cpp-api")
SET(KEYPLE_UTIL_DIR        "../../../github.com.calypsonet.keyple-util-cpp-lib")
SET(KEYPLE_UTIL_LIB        "keypleutilcpplib")

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/spi

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
    ${KEYPLE_PLUGIN_DIR}/src/main/spi/reader/observable
    ${KEYPLE_PLUGIN_DIR}/src/main/spi/reader/observable/state/insertion
    ${KEYPLE_PLUGIN_DIR}/src/main/spi/reader/observable/state/processing
    ${KEYPLE_PLUGIN_DIR}/src/main/spi/reader/observable/state/removal

    ${KEYPLE_UTIL_DIR}/src/main
    ${KEYPLE_UTIL_DIR}/src/main/cpp
    ${KEYPLE_UTIL_DIR}/src/main/cpp/exception
)

ADD_LIBRARY(
    ${LIBRARY_NAME}

    ${LIBRARY_TYPE}

    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractMonitoringJobAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractPluginAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractReaderAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractObservableLocalPluginAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractObservableStateAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ApduRequestAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ApduResponseAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AutonomousObservableLocalPluginAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardInsertionActiveMonitoringJobAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardInsertionPassiveMonitoringJobAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardRemovalActiveMonitoringJobAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardRemovalPassiveMonitoringJobAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardResponseAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardSelectionResponseAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardSelectionResultAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CardSelectionScenarioAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LocalPluginAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LocalPoolPluginAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LocalReaderAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MonitoringState.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableLocalReaderAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObservableReaderStateServiceAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PluginEventAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ReaderEventAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ScheduledCardSelectionsResponseAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WaitForCardInsertionStateAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WaitForCardProcessingStateAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WaitForCardRemovalStateAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WaitForStartDetectStateAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/ExecutorService.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/Job.cpp
)

FIND_LIBRARY(UTIL ${KEYPLE_UTIL_LIB} HINTS ${KEYPLE_UTIL_DIR}/build/bin)

TARGET_LINK_LIBRARIES(${LIBRARY_NAME} ${UTIL})
