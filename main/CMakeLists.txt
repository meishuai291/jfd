# 工程名称
PROJECT(mainlu)
SET(main_version 1.0.0)
SET(main_soversion 1.0)

# CMake最低版本要求
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)
IF(COMMAND CMAKE_POLICY)
    CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

FILE(TO_CMAKE_PATH $ENV{HOME} HOME_PATH)
SET(CMAKE_MODULE_PATH "${HOME_PATH}/.sipesc/share/cmake")

# 依赖宏包设置
FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(MExtManagerCore REQUIRED)

#SET(QT_DONT_USE_QTGUI 1)
#SET(QT_USE_QTTEST 1)
#SET(QT_USE_QTSCRIPT 1)
INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../plugin
    ${CMAKE_CURRENT_SOURCE_DIR}/../pub/include/plugin
    ${MEXTMGR_CORE_INCLUDE_DIRS}
)

# 文件设置
SET(main_src_files
    src/main.cxx
    src/test.cxx
#    src/debugger.cxx
)

SET(main_inc_files
#    inc/debugger.h
    inc/test.h
)
# 文件预处理
SET(main_moc_hdrs
)

# 翻译文件设置
QT4_WRAP_CPP(main_moc_srcs ${main_moc_hdrs})

# 插件动态库生成
ADD_EXECUTABLE(
    mainlu
    ${main_src_files}
    ${main_inc_files}
    ${main_moc_srcs}
)

TARGET_LINK_LIBRARIES(
    mainlu
    ${QT_LIBRARIES}
    ${MEXTMGR_CORE_LIBRARIES}
)

SET_TARGET_PROPERTIES(
    mainlu PROPERTIES AUTOMOC "1"
)

### Install
INSTALL(
    TARGETS mainlu
    RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../pub/bin
    LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../pub/bin
    ARCHIVE DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../pub/bin
)

IF(WIN32)
ELSE(WIN32)
SET(EXECUTABLE_OUTPUT_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../exec
)  
ENDIF(WIN32)   