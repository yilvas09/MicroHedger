set(SOURCES
    Bar.cpp
    LOB.cpp
)

add_library(lob_lib ${SOURCES})
target_link_libraries(lob_lib ${Boost_LIBRARIES})

add_library(utils_lib utilities.cpp)