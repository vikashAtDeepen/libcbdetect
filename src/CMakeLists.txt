find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
link_libraries(${JSONCPP_LIBRARIES})

include_directories(${PROJECT_SOURCE_DIR}/include ${JSONCPP_INCLUDE_DIRS})
add_executable(chessboard_corner_detect chessboard_corner_detect.cc)
target_link_libraries(chessboard_corner_detect cbdetect ${OpenCV_LIBS} ${JSONCPP_LIBRARIES})
