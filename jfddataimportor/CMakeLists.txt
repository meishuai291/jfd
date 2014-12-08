PROJECT(org.sipesc.fems.bltdataprocess)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0)
IF(COMMAND CMAKE_POLICY)
    CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

FILE(TO_CMAKE_PATH $ENV{HOME} HOME_PATH)
SET(CMAKE_MODULE_PATH "${HOME_PATH}/.sipesc/share/cmake")

FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(MExtManagerCore REQUIRED)

#SET(QT_DONT_USE_QTGUI 1)
INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../plugin
    ${CMAKE_CURRENT_SOURCE_DIR}/../pub/include/plugin
    ${MEXTMGR_CORE_INCLUDE_DIRS}
)

SET(org_sipesc_fems_bltdataprocess_src_files
    src/org.sipesc.fems.bltdataprocess.cxx
    src/mjfddataimportorimpl.cxx
    src/mjfdnodehandlerimpl.cxx
    src/mjfdmpchandlerimpl.cxx
    src/mjfdlineelementhandlerimpl.cxx
#    src/mjfd3dsolidelementhandlerimpl.cxx	# 8节点六面体
    src/mjfd3d12solidelementhandlerimpl.cxx	# 12节点六面体
    src/mjfdnlineelementhandlerimpl.cxx
    src/mjfdcrodelementhandlerimpl.cxx
    src/mjfdforcehandlerimpl.cxx
    src/mjfdgravhandlerimpl.cxx
    src/mtabled5parserimpl.cxx
    
    src/mjfdshellelementhandlerimpl.cxx
    src/mjfdbeamelementhandlerimpl.cxx
    
    src/mbaraxialforceparserimpl.cxx    # 杆轴力解析
    
    src/mbltdataexportorimpl.cxx
    src/mbltresultheaderexportorimpl.cxx
#    src/mbltresultexportorimpl.cxx
)

SET(org_sipesc_fems_bltdataprocess_hdr_files
    inc/org.sipesc.fems.bltdataprocess.h
)

SET(org_sipesc_fems_bltdataprocess_qui_files
)

MEXTMGR_WRAP_UI(
    org_sipesc_fems_bltdataprocess_uis_files
    ${org_sipesc_fems_bltdataprocess_qui_files}
)
MEXTMGR_WRAP_XML(
    org_sipesc_fems_bltdataprocess_xml_files
    doc/org.sipesc.fems.bltdataprocess.sipplugin
)
MEXTMGR_ADD_RESOURCES(
    org_sipesc_fems_bltdataprocess_qrc_files
    qrc/org.sipesc.fems.bltdataprocess.qrc
)
MEXTMGR_GEN_PLUGINHEADER(
    org_sipesc_fems_bltdataprocess_phd_files
    doc/org.sipesc.fems.bltdataprocess.sipplugin
    ${CMAKE_CURRENT_SOURCE_DIR}/../pub/include/plugin
)
MEXTMGR_GEN_PLUGINFEATURE(
    org_sipesc_fems_bltdataprocess_pfm_files
    doc/org.sipesc.fems.bltdataprocess.sipplugin
)
MEXTMGR_ADD_TRANSLATION(
    org_sipesc_fems_bltdataprocess_qms_files
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    qrc/org.sipesc.fems.bltdataprocess_zh_CN.ts
    ${org_sipesc_fems_bltdataprocess_src_files}
    ${org_sipesc_fems_bltdataprocess_xml_files}
    ${org_sipesc_fems_bltdataprocess_qui_files}
)

ADD_LIBRARY(
    org.sipesc.fems.bltdataprocess_2.0.0 SHARED
    ${org_sipesc_fems_bltdataprocess_src_files}
    ${org_sipesc_fems_bltdataprocess_xml_files}
    ${org_sipesc_fems_bltdataprocess_qms_files}
    ${org_sipesc_fems_bltdataprocess_phd_files}
    ${org_sipesc_fems_bltdataprocess_pfm_files}
    ${org_sipesc_fems_bltdataprocess_xml_files}
    ${org_sipesc_fems_bltdataprocess_qrc_files}
    ${org_sipesc_fems_bltdataprocess_hdr_files}
    ${org_sipesc_fems_bltdataprocess_uis_files}
)
SET_TARGET_PROPERTIES(
    org.sipesc.fems.bltdataprocess_2.0.0 PROPERTIES
    POSTFIX "" PREFIX "" SUFFIX ".sep" AUTOMOC "1"
)
IF(WIN32)
    SET_TARGET_PROPERTIES(
        org.sipesc.fems.bltdataprocess_2.0.0
        PROPERTIES LINK_FLAGS "-mthreads"
        COMPILE_FLAGS "-frtti -fexceptions -mthreads"
    )
ENDIF(WIN32)
TARGET_LINK_LIBRARIES(
    org.sipesc.fems.bltdataprocess_2.0.0
    ${QT_LIBRARIES}
    ${MEXTMGR_CORE_LIBRARIES}
)

### Install
INSTALL(
    TARGETS org.sipesc.fems.bltdataprocess_2.0.0
    RUNTIME DESTINATION lib/plugins/${CMAKE_BUILD_TYPE}
    LIBRARY DESTINATION lib/plugins/${CMAKE_BUILD_TYPE}
    ARCHIVE DESTINATION lib/plugins/${CMAKE_BUILD_TYPE}
)
INSTALL(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../pub/include/plugin
    DESTINATION include
    FILES_MATCHING PATTERN "org.sipesc.fems.jfdimport.*.h"
)
INSTALL(
    DIRECTORY qrc/
    DESTINATION share/translations/zh_CN
    FILES_MATCHING PATTERN "*_zh_CN.qm"
    PATTERN ".svn" EXCLUDE
)
INSTALL(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/org.sipesc.fems.bltdataprocess.features
    DESTINATION share/features
)
##
INSTALL(
    DIRECTORY qrc/
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../pub/share/translations/zh_CN
    FILES_MATCHING PATTERN "*_zh_CN.qm"
    PATTERN ".svn" EXCLUDE
)
INSTALL(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/org.sipesc.fems.bltdataprocess.features
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../pub/share/features
)
SET(LIBRARY_OUTPUT_PATH 
	${CMAKE_CURRENT_SOURCE_DIR}/../pub/lib/plugins/${CMAKE_BUILD_TYPE}
)