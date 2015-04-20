# ======================================================================
# Create a build directory and an install direcotory which are outside the source code directory
# Declare the install path from the cmake command line using either a relative or full path
#
#   cd .../path/to/build/directory
#   source .../path/to/cetlib/ups/setup_for_development <-d|-p> [qual]
#   cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#         -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#         $CETPKG_SOURCE
#   make
#   make test
#   make install
#   make package (builds distribution tarfile)
# ======================================================================

cmake_minimum_required(VERSION 2.8)  # use cmake 2.8 or later

# project name
project(mu2ebintools)

# cetbuildtools contains our cmake modules
set( CETBUILDTOOLS_VERSION $ENV{CETBUILDTOOLS_VERSION} )
if( NOT CETBUILDTOOLS_VERSION )
  message( FATAL_ERROR "ERROR: setup cetbuildtools to get the cmake modules" )
endif()
set( CMAKE_MODULE_PATH $ENV{CETBUILDTOOLS_DIR}/Modules ${CMAKE_MODULE_PATH} )

include(CetCMakeEnv)
cet_cmake_env()

#cet_set_compiler_flags(DIAGS CAVALIER EXTRA_FLAGS -pedantic)
#cet_set_compiler_flags(DIAGS VIGILANT
cet_set_compiler_flags(DIAGS CAVALIER
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic
  )
cet_report_compiler_flags()

find_ups_product( cetbuildtools v4_00_00 )
find_ups_product( fhiclcpp v3_00_00 )

# The environment has been established, now generate the Makefiles.

# source
add_subdirectory (mu2ebintools)

# testing
#add_subdirectory(test)

# ups - table and config files
add_subdirectory(ups)

# This step enables building the package tarball packaging utility
include(UseCPack)
