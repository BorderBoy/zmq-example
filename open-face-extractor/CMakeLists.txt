# Local libraries
include_directories(${LandmarkDetector_SOURCE_DIR}/include)
	
add_executable(OwnExtractor OwnExtractor.cpp base64.cpp)

find_package(cppzmq)

target_link_libraries(OwnExtractor LandmarkDetector)
target_link_libraries(OwnExtractor FaceAnalyser)
target_link_libraries(OwnExtractor GazeAnalyser)
target_link_libraries(OwnExtractor Utilities)
target_link_libraries(OwnExtractor cppzmq)

install (TARGETS OwnExtractor DESTINATION bin)