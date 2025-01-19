add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_sig
    YARP::YARP_dev
)
