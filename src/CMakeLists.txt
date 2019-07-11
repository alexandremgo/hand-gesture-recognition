file(
    GLOB 
    src_cpp_files
    *.cpp
    )

add_library(hand_gesture_recognition
    SHARED
    ${src_cpp_files}
    )

find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui imgcodecs)
target_link_libraries(hand_gesture_recognition ${OpenCV_LIBS} stdc++fs)

install (TARGETS hand_gesture_recognition
    DESTINATION lib)

file(
    GLOB 
    headers
    *.hpp
    )

install(FILES ${headers}
    DESTINATION include/${CMAKE_PROJECT_NAME})


