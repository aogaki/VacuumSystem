cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

if(POLICY CMP0025)
    cmake_policy(SET CMP0025 NEW)
endif()

project(vac-mon LANGUAGES C CXX)

# Enforce the C++ standard, and disable extensions.
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# find_package(libmongocxx REQUIRED)
# find_package(libbsoncxx REQUIRED)
find_package(mongocxx REQUIRED)
find_package(bsoncxx REQUIRED)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hpp)
add_executable(vac-mon ./main.cpp  ${sources} ${headers})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                    /usr/include/libserial/
                    ${LIBMONGOCXX_INCLUDE_DIRS}
                    ${LIBBSONCXX_INCLUDE_DIRS}
                  )

target_link_libraries(vac-mon
  # ${LIBMONGOCXX_LIBRARIES}
  # ${LIBBSONCXX_LIBRARIES}
  mongo::bsoncxx_shared
  mongo::mongocxx_shared
  pthread
  serial
)

add_custom_target(run
    COMMAND vac-mon
    DEPENDS vac-mon
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)
