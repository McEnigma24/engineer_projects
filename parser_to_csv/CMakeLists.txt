cmake_minimum_required(VERSION 3.18)
project(MyProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# Ustawienia builda (Debug, Release)
if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Release)
endif()

# Dodaj pliki źródłowe
include_directories(inc ../Ray_Tracer/inc ../Single_Operations/inc)
aux_source_directory(src SOURCES)

find_package(OpenMP REQUIRED)

# Domyślne flagi optymalizacji
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fopenmp -w")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -pg -w")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")


### ORIGINAL ###

# Targety
# set(EXE_NAME "single_op_avg_og.exe")
# add_executable(${EXE_NAME} ${SOURCES})
# target_compile_definitions(${EXE_NAME} PRIVATE LINUX SINGLE AVG ORIGINAL)
# set_target_properties(${EXE_NAME} PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
# target_link_libraries(${EXE_NAME} PUBLIC OpenMP::OpenMP_CXX)

# Targety
# set(EXE_NAME "single_op_dev_og.exe")
# add_executable(${EXE_NAME} ${SOURCES})
# target_compile_definitions(${EXE_NAME} PRIVATE LINUX SINGLE DEV ORIGINAL)
# set_target_properties(${EXE_NAME} PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
# target_link_libraries(${EXE_NAME} PUBLIC OpenMP::OpenMP_CXX)

set(EXE_NAME "main_avg_og.exe")
add_executable(${EXE_NAME} ${SOURCES})
target_compile_definitions(${EXE_NAME} PRIVATE LINUX MAIN AVG ORIGINAL)
set_target_properties(${EXE_NAME} PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
target_link_libraries(${EXE_NAME} PUBLIC OpenMP::OpenMP_CXX)

set(EXE_NAME "main_dev_og.exe")
add_executable(${EXE_NAME} ${SOURCES})
target_compile_definitions(${EXE_NAME} PRIVATE LINUX MAIN DEV ORIGINAL)
set_target_properties(${EXE_NAME} PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
target_link_libraries(${EXE_NAME} PUBLIC OpenMP::OpenMP_CXX)


### FILTERED ###

# Targety
# set(EXE_NAME "single_op_avg_filt.exe")
# add_executable(${EXE_NAME} ${SOURCES})
# target_compile_definitions(${EXE_NAME} PRIVATE LINUX SINGLE AVG FILTERED)
# set_target_properties(${EXE_NAME} PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
# target_link_libraries(${EXE_NAME} PUBLIC OpenMP::OpenMP_CXX)

# Targety
# set(EXE_NAME "single_op_dev_filt.exe")
# add_executable(${EXE_NAME} ${SOURCES})
# target_compile_definitions(${EXE_NAME} PRIVATE LINUX SINGLE DEV FILTERED)
# set_target_properties(${EXE_NAME} PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
# target_link_libraries(${EXE_NAME} PUBLIC OpenMP::OpenMP_CXX)

set(EXE_NAME "main_avg_filt.exe")
add_executable(${EXE_NAME} ${SOURCES})
target_compile_definitions(${EXE_NAME} PRIVATE LINUX MAIN AVG FILTERED)
set_target_properties(${EXE_NAME} PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
target_link_libraries(${EXE_NAME} PUBLIC OpenMP::OpenMP_CXX)

set(EXE_NAME "main_dev_filt.exe")
add_executable(${EXE_NAME} ${SOURCES})
target_compile_definitions(${EXE_NAME} PRIVATE LINUX MAIN DEV FILTERED)
set_target_properties(${EXE_NAME} PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
target_link_libraries(${EXE_NAME} PUBLIC OpenMP::OpenMP_CXX)