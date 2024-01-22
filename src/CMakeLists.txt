find_package(Python2 COMPONENTS Interpreter Development)
find_package(Python3 COMPONENTS Interpreter Development)

set(PYBIND11_NOPYTHON ON)
find_package(pybind11 REQUIRED)

find_package(fparser)
if(fparser_FOUND)
  if(EXISTS "${fparser_INCLUDE_DIR}/fparser")
    set(fparser_INCLUDE_DIR "${fparser_INCLUDE_DIR}/fparser")
  endif()
endif()

if(Python2_FOUND)
execute_process(
  COMMAND ${Python2_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; from os.path import relpath; print(relpath(get_python_lib(1,prefix='${CMAKE_INSTALL_PREFIX}'),'${CMAKE_INSTALL_PREFIX}'))"
  OUTPUT_VARIABLE PYTHON2_INSTALL_DIR OUTPUT_STRIP_TRAILING_WHITESPACE
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
add_library(functionparser SHARED pybind11.cpp)
target_include_directories(functionparser PRIVATE ${fparser_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${Python2_INCLUDE_DIRS})
target_link_directories(functionparser PUBLIC ${fparser_LIBRARY_DIR})
set_target_properties(functionparser PROPERTIES PREFIX "" COMPILE_FLAGS "")
target_link_libraries(functionparser PUBLIC fparser ${Python2_LIBRARIES} -Wl,--no-undefined)
install(TARGETS functionparser DESTINATION ${PYTHON2_INSTALL_DIR})
endif()

if(Python3_FOUND)
execute_process(
  COMMAND ${Python3_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; from os.path import relpath; print(relpath(get_python_lib(1,prefix='${CMAKE_INSTALL_PREFIX}'),'${CMAKE_INSTALL_PREFIX}'))"
  OUTPUT_VARIABLE PYTHON3_INSTALL_DIR OUTPUT_STRIP_TRAILING_WHITESPACE
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
add_library(functionparserpy3 SHARED pybind11.cpp)
target_include_directories(functionparserpy3 PRIVATE ${fparser_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${Python3_INCLUDE_DIRS})
target_link_directories(functionparserpy3 PUBLIC ${fparser_LIBRARY_DIR})
set_target_properties(functionparserpy3 PROPERTIES PREFIX "" COMPILE_FLAGS "" LIBRARY_OUTPUT_DIRECTORY "python3" OUTPUT_NAME "functionparser")
target_link_libraries(functionparserpy3 PUBLIC fparser ${Python3_LIBRARIES} -Wl,--no-undefined)
install(TARGETS functionparserpy3 DESTINATION ${PYTHON3_INSTALL_DIR})
endif()
