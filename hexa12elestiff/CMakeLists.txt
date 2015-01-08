PROJECT(org.sipesc.fems.hexabrickelement12)

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

SET(org_sipesc_fems_hexabrickelement12_src_files
    src/org.sipesc.fems.hexabrickelement12.cxx
    src/mhexabrick12eleconstidparserimpl.cxx
    src/mhexabrick12elementdataimpl.cxx
    src/mhexabrick12elevolumeimpl.cxx
    src/mgau3d12integformimpl.cxx
    src/mhexabrick12lumpedmassmatriximpl.cxx
    src/mhexabrick12nodestresscalculatorimpl.cxx
    src/mhexabrick12shpfunctionimpl.cxx
    src/mhexabrick12strainmatriximpl.cxx
    src/melementdataimpl.cxx
)

SET(org_sipesc_fems_hexabrickelement12_hdr_files
    inc/org.sipesc.fems.hexabrickelement12.h
    inc/mhexabrick12eleconstidparserfactoryimpl.h
    inc/mhexabrick12eleconstidparserimpl.h
    inc/mhexabrick12elemasscalculatorfactoryimpl.h
    inc/mhexabrick12elementdataimpl.h
    inc/mhexabrick12elestiffcalculatorfactoryimpl.h
    inc/mhexabrick12elestraincalculatorfactoryimpl.h
    inc/mhexabrick12elestresscalculatorfactoryimpl.h
    inc/mhexabrick12elevolumefactoryimpl.h
    inc/mhexabrick12elevolumeimpl.h
    inc/mgau3d12integformfactoryimpl.h
    inc/mgau3d12integformimpl.h
    inc/mhexabrick12lumpedmassmatrixfactoryimpl.h
    inc/mhexabrick12lumpedmassmatriximpl.h
    inc/mhexabrick12nodestresscalculatorfactoryimpl.h
    inc/mhexabrick12nodestresscalculatorimpl.h
    inc/mhexabrick12shpfunctionfactoryimpl.h
    inc/mhexabrick12shpfunctionimpl.h
    inc/mhexabrick12strainmatrixfactoryimpl.h
    inc/mhexabrick12strainmatriximpl.h
    inc/melementdataimpl.h
)

SET(org_sipesc_fems_hexabrickelement12_qui_files
)

MEXTMGR_WRAP_UI(
    org_sipesc_fems_hexabrickelement12_uis_files
    ${org_sipesc_fems_hexabrickelement12_qui_files}
)
MEXTMGR_WRAP_XML(
    org_sipesc_fems_hexabrickelement12_xml_files
    doc/org.sipesc.fems.hexabrickelement12.sipplugin
)
MEXTMGR_ADD_RESOURCES(
    org_sipesc_fems_hexabrickelement12_qrc_files
    qrc/org.sipesc.fems.hexabrickelement12.qrc
)
MEXTMGR_GEN_PLUGINHEADER(
    org_sipesc_fems_hexabrickelement12_phd_files
    doc/org.sipesc.fems.hexabrickelement12.sipplugin
    ${CMAKE_CURRENT_SOURCE_DIR}/../pub/include/plugin
)
MEXTMGR_GEN_PLUGINFEATURE(
    org_sipesc_fems_hexabrickelement12_pfm_files
    doc/org.sipesc.fems.hexabrickelement12.sipplugin
)
MEXTMGR_ADD_TRANSLATION(
    org_sipesc_fems_hexabrickelement12_qms_files
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    qrc/org.sipesc.fems.hexabrickelement12_zh_CN.ts
    ${org_sipesc_fems_hexabrickelement12_src_files}
    ${org_sipesc_fems_hexabrickelement12_xml_files}
    ${org_sipesc_fems_hexabrickelement12_qui_files}
)

ADD_LIBRARY(
    org.sipesc.fems.hexabrickelement12_2.0.0 SHARED
    ${org_sipesc_fems_hexabrickelement12_src_files}
    ${org_sipesc_fems_hexabrickelement12_xml_files}
    ${org_sipesc_fems_hexabrickelement12_qms_files}
    ${org_sipesc_fems_hexabrickelement12_phd_files}
    ${org_sipesc_fems_hexabrickelement12_pfm_files}
    ${org_sipesc_fems_hexabrickelement12_xml_files}
    ${org_sipesc_fems_hexabrickelement12_qrc_files}
    ${org_sipesc_fems_hexabrickelement12_hdr_files}
    ${org_sipesc_fems_hexabrickelement12_uis_files}
)
SET_TARGET_PROPERTIES(
    org.sipesc.fems.hexabrickelement12_2.0.0 PROPERTIES
    POSTFIX "" PREFIX "" SUFFIX ".sep" AUTOMOC "1"
)
TARGET_LINK_LIBRARIES(
    org.sipesc.fems.hexabrickelement12_2.0.0
    ${QT_LIBRARIES}
    ${MEXTMGR_CORE_LIBRARIES}
)

### Install
INSTALL(
    TARGETS org.sipesc.fems.hexabrickelement12_2.0.0
    RUNTIME DESTINATION lib/plugins/${CMAKE_BUILD_TYPE}
    LIBRARY DESTINATION lib/plugins/${CMAKE_BUILD_TYPE}
    ARCHIVE DESTINATION lib/plugins/${CMAKE_BUILD_TYPE}
)
INSTALL(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../pub/include/plugin
    DESTINATION include
    FILES_MATCHING PATTERN "org.sipesc.fems.hexaelement12.*.h"
)
INSTALL(
    DIRECTORY qrc/
    DESTINATION share/translations/zh_CN
    FILES_MATCHING PATTERN "*_zh_CN.qm"
    PATTERN ".svn" EXCLUDE
)
INSTALL(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/org.sipesc.fems.hexabrickelement12.features
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
    FILES ${CMAKE_CURRENT_BINARY_DIR}/org.sipesc.fems.hexabrickelement12.features
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../pub/share/features
)
SET(LIBRARY_OUTPUT_PATH 
	${CMAKE_CURRENT_SOURCE_DIR}/../pub/lib/plugins/${CMAKE_BUILD_TYPE}
)