cmake_minimum_required (VERSION 3.4)

project(DistributedSimulatedAnnealing)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include("CMakeLists.Files.txt")

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${MPI_CXX_INCLUDE_PATH})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (MSVC) 
add_definitions(-D_DISABLE_EXTENDED_ALIGNED_STORAGE)
endif()

source_group("Headers" FILES ${HEADERS})
set(SOURCES ${SOURCES} ${HEADERS})

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${LIBS})
