##----------------------------------------------------------------------
##----------------------------------------------------------------------
##
## bridge_hla 
## Implementation for IEEE HLA bridges between federations (Compliant with CERTI RTI)
## 
## Copyright (C) 2023  ISAE-SUPAERO
##
## Authors: Benoit Breholée
##          Jean-Baptiste Chaudron
##
## email:   jean-baptiste.chaudron@isae-supaero.fr
##
##----------------------------------------------------------------------
##----------------------------------------------------------------------

PROJECT(BRIDGE-HLA-PROJECT)

#################### CERTI requires CMake 2.8.11 version ###########################
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)
SET(CMAKE_CXX_FLAGS "-std=c++0x")

CMAKE_POLICY(SET CMP0014 OLD)
cmake_policy(SET CMP0003 NEW)

IF ("$ENV{CERTI_HOME}" STRGREATER "")
    FILE(TO_CMAKE_PATH "$ENV{CERTI_HOME}" CERTI_HOME_TEMP)
    SET(CERTI_HOME ${CERTI_HOME_TEMP} CACHE PATH "The CERTI install directory")
    LIST(APPEND CMAKE_MODULE_PATH "${CERTI_HOME}/share/scripts")

ELSE ("$ENV{CERTI_HOME}" STRGREATER "")
    LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/share/scripts")    
ENDIF ("$ENV{CERTI_HOME}" STRGREATER "")

OPTION(USE_FULL_RPATH
             "Use the full RPATH" OFF)
             
IF (USE_FULL_RPATH)       
    # see http://www.cmake.org/Wiki/CMake_RPATH_handling#Always_full_RPATH
    # use, i.e. don't skip the full RPATH for the build tree
    SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
    
    # when building, don't use the install RPATH already
    # (but later on when installing)
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
    
    # the RPATH to be used when installing
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    
    # add the automatically determined parts of the RPATH
    # which point to directories outside the build tree to the install RPATH
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
ENDIF(USE_FULL_RPATH)


OPTION(BUILD_SHARED
  "Build libraries as shared library" OF)
IF (BUILD_SHARED)
  SET(BUILD_SHARED_LIBS ON)
ELSE (BUILD_SHARED)
  SET(BUILD_SHARED_LIBS OFF)
ENDIF (BUILD_SHARED)

SET(CMAKE_BUILD_TYPE "Debug")
###
#SET(CMAKE_BUILD_TYPE "Release")

SET(PACKAGE_NAME      ${CMAKE_PROJECT_NAME})
SET(PACKAGE_VERSION_MAJOR "0")
SET(PACKAGE_VERSION_MINOR "0")
SET(PACKAGE_VERSION_PATCH "1")
SET(PACKAGE_VERSION   "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}.${PACKAGE_VERSION_PATCH}")
###
SET(PACKAGE_BUGREPORT "jean-baptiste.chaudron@isae.fr")

MESSAGE(STATUS "Configuring ${PACKAGE_NAME} version ${PACKAGE_VERSION}")

FIND_PACKAGE(RTI REQUIRED)
#INCLUDE_DIRECTORIES(${RTI_INCLUDE_DIR})
ADD_DEFINITIONS(${RTI_DEFINITIONS})


MESSAGE(STATUS "* CERTI FOUND *")

find_package(LibXml2 REQUIRED)
message(STATUS LIBXML2_FOUND = ${LIBXML2_FOUND})

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Bridge HLA using CERTI (for now)")

SET(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
SET(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})

IF ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
     SET(CPACK_SOURCE_IGNORE_FILES "/CVS/;/\\\\.svn/;\\\\.swp$;\\\\.#;/#;.*~;cscope.*;\\\\.cproject;\\\\.project;autom4te.cache/*")
ELSE ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
     SET(CPACK_SOURCE_IGNORE_FILES "/CVS/;/\\\\.svn/;\\\\.swp$;\\\\.#;/#;.*~;cscope.*;\\\\.cproject;\\\\.project;autom4te.cache/*;${CMAKE_BINARY_DIR}/*")
ENDIF ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")

SET(CPACK_SOURCE_GENERATOR "ZIP;TGZ")
IF (WIN32)
  SET(CPACK_GENERATOR "ZIP;NSIS")
  GET_FILENAME_COMPONENT(CPACK_COMMAND ${CMAKE_COMMAND} PATH)
  SET(CPACK_COMMAND ${CPACK_COMMAND}/cpack)
  ADD_CUSTOM_TARGET(PACKAGE_SOURCE_ZIP
              COMMAND ${CPACK_COMMAND} -G ZIP --config CPackSourceConfig.cmake
              COMMENTS "Build a ZIP file containing the source")  
  INCLUDE(InstallRequiredSystemLibraries)
ELSE (WIN32)
  SET(CPACK_GENERATOR "ZIP;TGZ")
ENDIF(WIN32)
SET(CPACK_PACKAGE_VERSION ${PACKAGE_VERSION})
SET(CPACK_PACKAGE_VERSION_MAJOR ${PACKAGE_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${PACKAGE_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${PACKAGE_VERSION_PATCH})

SET(PACKAGE_BUGREPORT "jean-baptiste.chaudron@isae-supaero.fr")

INCLUDE(CPack)

# FOR BENCHMARK_FED 
SET(BRIDGE_HLA_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)
SET(BRIDGE_HLA_INCLUDE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)
ADD_SUBDIRECTORY(${BRIDGE_HLA_SOURCE_DIRECTORY})
INCLUDE_DIRECTORIES(${BRIDGE_HLA_INCLUDE_DIRECTORY})

INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR})

###########   the executable name & the source list  ############################
## HLA 1.3 specific code follows
set(FEDERATE_TARGETNAME "bridgehla")
add_executable(${FEDERATE_TARGETNAME} 
				${BRIDGE_HLA_SOURCE_DIRECTORY}/bridge.cc
				${BRIDGE_HLA_SOURCE_DIRECTORY}/cmdline.c
				${BRIDGE_HLA_SOURCE_DIRECTORY}/cmdline.h
				${BRIDGE_HLA_SOURCE_DIRECTORY}/ContainerEntity.hh
				${BRIDGE_HLA_SOURCE_DIRECTORY}/Entity.hh
				${BRIDGE_HLA_SOURCE_DIRECTORY}/Fed.cc
				${BRIDGE_HLA_SOURCE_DIRECTORY}/Federate.cc
				${BRIDGE_HLA_SOURCE_DIRECTORY}/Federate.hh
				${BRIDGE_HLA_SOURCE_DIRECTORY}/Federation.cc
				${BRIDGE_HLA_SOURCE_DIRECTORY}/Federation.hh
				${BRIDGE_HLA_SOURCE_DIRECTORY}/Fed.hh
				${BRIDGE_HLA_SOURCE_DIRECTORY}/getopt1.c
				${BRIDGE_HLA_SOURCE_DIRECTORY}/getopt.c
				${BRIDGE_HLA_SOURCE_DIRECTORY}/getopt.h

               )
set_target_properties(${FEDERATE_TARGETNAME} PROPERTIES COMPILE_FLAGS "-DHLA_13")
target_include_directories(${FEDERATE_TARGETNAME} PUBLIC ${CERTI_HOME}/include/hla13)
target_link_libraries(${FEDERATE_TARGETNAME} ${RTI_LIBRARIES} ${LIBXML2_LIBRARIES})
install(TARGETS ${FEDERATE_TARGETNAME}
    RUNTIME DESTINATION bin)
    
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/data/ DESTINATION bin)

MESSAGE(STATUS "************************************************************************")
MESSAGE(STATUS "**********                                                    **********")
MESSAGE(STATUS "********** ${CMAKE_PROJECT_NAME} has been successfully configured **********")
MESSAGE(STATUS "**********                                                    **********")
MESSAGE(STATUS "************************************************************************")

###################################################################################
########################### FIN COMMANDES CMAKE ###################################
###################################################################################
