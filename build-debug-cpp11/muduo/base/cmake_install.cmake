# Install script for directory: /home/muduo/4_muduo_study_again/muduo/base

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/muduo/4_muduo_study_again/debug-install-cpp11")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/muduo/4_muduo_study_again/build-debug-cpp11/lib/libmuduo_base.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/muduo/base" TYPE FILE FILES
    "/home/muduo/4_muduo_study_again/muduo/base/AsyncLogging.h"
    "/home/muduo/4_muduo_study_again/muduo/base/Atomic.h"
    "/home/muduo/4_muduo_study_again/muduo/base/BlockingQueue.h"
    "/home/muduo/4_muduo_study_again/muduo/base/BoundedBlockingQueue.h"
    "/home/muduo/4_muduo_study_again/muduo/base/Condition.h"
    "/home/muduo/4_muduo_study_again/muduo/base/CountDownLatch.h"
    "/home/muduo/4_muduo_study_again/muduo/base/CurrentThread.h"
    "/home/muduo/4_muduo_study_again/muduo/base/Date.h"
    "/home/muduo/4_muduo_study_again/muduo/base/Exception.h"
    "/home/muduo/4_muduo_study_again/muduo/base/FileUtil.h"
    "/home/muduo/4_muduo_study_again/muduo/base/LogFile.h"
    "/home/muduo/4_muduo_study_again/muduo/base/LogStream.h"
    "/home/muduo/4_muduo_study_again/muduo/base/Logging.h"
    "/home/muduo/4_muduo_study_again/muduo/base/Mutex.h"
    "/home/muduo/4_muduo_study_again/muduo/base/ProcessInfo.h"
    "/home/muduo/4_muduo_study_again/muduo/base/Singleton.h"
    "/home/muduo/4_muduo_study_again/muduo/base/StringPiece.h"
    "/home/muduo/4_muduo_study_again/muduo/base/Thread.h"
    "/home/muduo/4_muduo_study_again/muduo/base/ThreadLocal.h"
    "/home/muduo/4_muduo_study_again/muduo/base/ThreadLocalSingleton.h"
    "/home/muduo/4_muduo_study_again/muduo/base/ThreadPool.h"
    "/home/muduo/4_muduo_study_again/muduo/base/TimeZone.h"
    "/home/muduo/4_muduo_study_again/muduo/base/Timestamp.h"
    "/home/muduo/4_muduo_study_again/muduo/base/Types.h"
    "/home/muduo/4_muduo_study_again/muduo/base/copyable.h"
    "/home/muduo/4_muduo_study_again/muduo/base/noncopyable.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/muduo/4_muduo_study_again/build-debug-cpp11/muduo/base/tests/cmake_install.cmake")

endif()

