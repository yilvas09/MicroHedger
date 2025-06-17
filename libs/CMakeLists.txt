set(SOURCES_UTILS
    Utils.cpp
    Random.cpp
)
add_library(utils_lib ${SOURCES_UTILS})

set(SOURCES_OPTION
    Option.cpp
)
add_library(option_lib ${SOURCES_OPTION})
target_link_libraries(option_lib utils_lib)

set(SOURCES_LOB
    Bar.cpp
    LOB.cpp
)
add_library(lob_lib ${SOURCES_LOB})
target_link_libraries(lob_lib ${Boost_LIBRARIES})

set(SOURCES_PLAYERS
    DeltaHedger.cpp
)
add_library(players_lib ${SOURCES_PLAYERS})
target_link_libraries(players_lib option_lib lob_lib)

set(SOURCES_PATHS
    PathCollection.cpp
)
add_library(path_lib ${SOURCES_PATHS})
