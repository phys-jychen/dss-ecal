#----------------------------------------
# Project: dss-ecal
#----------------------------------------

# Minimum version of CMake
cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

# Name and version of the project
project(dss-ecal VERSION 1.0.0 LANGUAGES C CXX)

# External Packages
# Geant4
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()
# ROOT
find_package(ROOT REQUIRED COMPONENTS TMVA ROOTVecOps ROOTDataFrame)
# YAML
find_package(yaml-cpp REQUIRED)

# Set runtime output directory as bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Add the directories and files
include(${Geant4_USE_FILE})
include(${ROOT_USE_FILE})
include_directories(${YAML_CPP_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include 
                    ${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIR})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

# Add executables
add_executable(ecal ecal.cc ${sources} ${headers})

# Link libraries
target_link_libraries(ecal ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} yaml-cpp)

# Copy all scripts to the build directory
set(ecal_SCRIPTS
    vis.mac
#    Co60.mac  DarkSHINEECAL.in  vis.mac  fullChain.mac  pythia.mac  pythia_event.data
    )
foreach(_script ${DarkSHINEECAL_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
install(TARGETS ecal DESTINATION bin)

# Add commands to set up the environment with the help of setup.sh...
execute_process(COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/config/setup.sh ${PROJECT_BINARY_DIR})
execute_process(COMMAND sed -i "s:<PROJECT>:${CMAKE_CURRENT_SOURCE_DIR}:g" ${PROJECT_BINARY_DIR}/setup.sh)
execute_process(COMMAND chmod 774 ${PROJECT_BINARY_DIR}/setup.sh)