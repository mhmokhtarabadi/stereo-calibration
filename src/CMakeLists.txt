
add_executable(${PROJECT_NAME} 
    main.cpp 
    my_time.cpp 
    cameraCalibration.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/dependencies/tomlplusplus/include)
target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${OpenCV_LIBS})