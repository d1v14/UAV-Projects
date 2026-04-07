cmake_minimum_required(VERSION 3.0.2)
project(visual_navigation_node)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
)

include_directories(
include
  ${catkin_INCLUDE_DIRS}
)

add_executable(visual_navigation_executable src/visual_navigation_node.cpp)
target_link_libraries(visual_navigation_executable
  ${catkin_LIBRARIES}
)
set_target_properties(visual_navigation_executable PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CATKIN_DEVEL_PREFIX}/lib/${PROJECT_NAME})
install(TARGETS visual_navigation_executable
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
