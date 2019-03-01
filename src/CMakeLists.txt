# Copy resouce
file(COPY ${CMAKE_SOURCE_DIR}/res/ DESTINATION ${PROJECT_BINARY_DIR}/res/)
add_definitions(-DRESOURCE_DIR="${PROJECT_BINARY_DIR}/res/")

#not work -----------
#file(GLOB FILES_RSC "${CMAKE_SOURCE_DIR}/res")
#set_source_files_properties(${FILES_RSC} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)


add_definitions(-std=c++11)
file( GLOB SRCS *.cpp *.hpp *.mm *.h *. )

add_executable(
    rot
    ${SRCS}
)

target_link_libraries(rot "-framework Cocoa")
set_target_properties(rot PROPERTIES LINK_FLAGS "-Wl,-F/Library/Frameworks")

# For OpenCV
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    message(inc_at${OpenCV_INCLUDE_DIRS})
    include_directories(${OpenCV_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
    target_link_libraries(rot ${OpenCV_LIBS} ${PYTHON_LIBRARIES})
endif(OpenCV_FOUND)
