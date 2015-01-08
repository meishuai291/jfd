PROJECT(org.sipesc.fems.bltdataexportor)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0)
IF(COMMAND CMAKE_POLICY)
    CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

FILE(TO_CMAKE_PATH $ENV{HOME} HOME_PATH)
SET(CMAKE_MODULE_PATH "${HOME_PATH}/.sipesc/share/cmake")

FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(MExtManagerCore REQUIRED)

SET(QT_DONT_USE_QTGUI 1)
INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../plugin
    ${CMAKE_CURRENT_SOURCE_DIR}/../pub/include/plugin
    ${HOME_PATH}/.sipesc/inlcude/plugin
    ${MEXTMGR_CORE_INCLUDE_DIRS}
)

SET(org_sipesc_fems_bltdataexportor_src_files
    src/org.sipesc.fems.bltdataexportor.cxx
    
    src/mbaraxialforceparserimpl.cxx    # 杆轴力解析
    
    src/mbltdataexportorimpl.cxx
    src/mbltresultheaderexportorimpl.cxx
    src/mbltresultexportorimpl.cxx
)

SET(org_sipesc_fems_bltdataexportor_hdr_files
    inc/org.sipesc.fems.bltdataexportor.h
)

SET(org_sipesc_fems_bltdataexportor_qui_files
)

MEXTMGR_WRAP_UI(
    org_sipesc_fems_bltdataexportor_uis_files
    ${org_sipesc_fems_bltdataexportor_qui_files}
)
MEXTMGR_WRAP_XML(
    org_sipesc_fems_bltdataexportor_xml_files
    doc/org.sipesc.fems.bltdataexportor.sipplugin
)
#MEXTMGR_ADD_RESOURCES(
#    org_sipesc_fems_bltdataexportor_qrc_files
#    qrc/org.sipesc.fems.bltdataexportor.qrc
#)
MEXTMGR_GEN_PLUGINHEADER(
    org_sipesc_fems_bltdataexportor_phd_files
    doc/org.sipesc.fems.bltdataexportor.sipplugin
    ${CMAKE_CURRENT_SOURCE_DIR}/../pub/include/plugin
)
MEXTMGR_GEN_PLUGINFEATURE(
    org_sipesc_fems_bltdataexportor_pfm_files
    doc/org.sipesc.fems.bltdataexportor.sipplugin
)
MEXTMGR_ADD_TRANSLATION(
    org_sipesc_fems_bltdataexportor_qms_files
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
#    qrc/org.sipesc.fems.bltdataexportor_zh_CN.ts
    ${org_sipesc_fems_bltdataexportor_src_files}
    ${org_sipesc_fems_bltdataexportor_xml_files}
    ${org_sipesc_fems_bltdataexportor_qui_files}
)

ADD_LIBRARY(
    org.sipesc.fems.bltdataexportor_1.0.0 SHARED
    ${org_sipesc_fems_bltdataexportor_src_files}
    ${org_sipesc_fems_bltdataexportor_xml_files}
    ${org_sipesc_fems_bltdataexportor_qms_files}
    ${org_sipesc_fems_bltdataexportor_phd_files}
    ${org_sipesc_fems_bltdataexportor_pfm_files}
    ${org_sipesc_fems_bltdataexportor_xml_files}
#    ${org_sipesc_fems_bltdataexportor_qrc_files}
    ${org_sipesc_fems_bltdataexportor_hdr_files}
    ${org_sipesc_fems_bltdataexportor_uis_files}
)
SET_TARGET_PROPERTIES(
    org.sipesc.fems.bltdataexportor_1.0.0 PROPERTIES
    POSTFIX "" PREFIX "" SUFFIX ".sep" AUTOMOC "1"
)
TARGET_LINK_LIBRARIES(
    org.sipesc.fems.bltdataexportor_1.0.0
    ${QT_LIBRARIES}
    ${MEXTMGR_CORE_LIBRARIES}
)

### Install
INSTALL(
    TARGETS org.sipesc.fems.bltdataexportor_1.0.0
    RUNTIME DESTINATION lib/plugins/${CMAKE_BUILD_TYPE}
    LIBRARY DESTINATION lib/plugins/${CMAKE_BUILD_TYPE}
    ARCHIVE DESTINATION lib/plugins/${CMAKE_BUILD_TYPE}
)
INSTALL(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../pub/include/plugin
    DESTINATION include
    FILES_MATCHING PATTERN "org.sipesc.fems.bltexport.*.h"
)
#INSTALL(
#    DIRECTORY qrc/
#    DESTINATION share/translations/zh_CN
#    FILES_MATCHING PATTERN "*_zh_CN.qm"
#    PATTERN ".svn" EXCLUDE
#)
INSTALL(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/org.sipesc.fems.bltdataexportor.features
    DESTINATION share/features
)
##
INSTALL(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/org.sipesc.fems.bltdataexportor.features
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../pub/share/features
)
SET(LIBRARY_OUTPUT_PATH 
	${CMAKE_CURRENT_SOURCE_DIR}/../pub/lib/plugins/${CMAKE_BUILD_TYPE}
)