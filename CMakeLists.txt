# ======================================================================
#  duneprototypes main build file
# ======================================================================

cmake_minimum_required (VERSION 3.19 FATAL_ERROR)

project(duneprototypes LANGUAGES CXX)
set(${PROJECT_NAME}_CMAKE_PROJECT_VERSION_STRING 09.61.00d00G1810b0211a)

# cetbuildtools contains our cmake modules
find_package(cetbuildtools REQUIRED)

list(APPEND CMAKE_MODULE_PATH $ENV{CANVAS_ROOT_IO_DIR}/Modules)
list(APPEND CMAKE_MODULE_PATH $ENV{ART_DIR}/Modules)


include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic -Wno-unused-local-typedefs
)

cet_report_compiler_flags()

find_ups_boost( )
find_ups_product( art )
find_ups_product( art_root_io )
find_ups_product( canvas_root_io )
find_ups_root()
find_ups_product( cetbuildtools )
find_ups_product( larcore )
find_ups_product( larcorealg )
find_ups_product( larpandora )
find_ups_product( dunecore )
find_ups_product( lardataobj )
find_ups_product( lardataalg )
find_ups_product( lardata )
find_ups_product( larevt )
find_ups_product( duneopdet )
find_ups_product( dunecalib )
find_ups_product( dunesim )
find_ups_product( nusimdata )
find_ups_product( dunepdlegacy )
find_ups_product( artdaq_core )
find_ups_product( clhep )
find_ups_product( hdf5 )
find_ups_product( dunedaqdataformats )
find_ups_product( dunedetdataformats )
find_ups_product( postgresql )
find_ups_product( libwda )
find_ups_product( ifdh_art )
find_ups_product( ifdhc )
find_ups_product( ifbeam )
find_ups_geant4( )

string(TOUPPER ${CMAKE_BUILD_TYPE} BTYPE_UC )
if( ${BTYPE_UC} MATCHES "DEBUG" )
 find_library( LIBHDF5 NAMES hdf5_debug PATHS ENV HDF5_LIB NO_DEFAULT_PATH)
else()
 find_library( LIBHDF5 NAMES hdf5 PATHS ENV HDF5_LIB NO_DEFAULT_PATH)
endif()
include_directories(${HDF5_INCLUDE_DIRS})

# macros for artdaq_dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

add_subdirectory(duneprototypes)
add_subdirectory(ups)

include(UseCPack)
