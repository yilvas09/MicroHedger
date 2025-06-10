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

set(SOURCES_PLAYERS
    DeltaHedger.cpp
)
add_library(players_lib ${SOURCES_PLAYERS})

set(SOURCES_LOB
    Bar.cpp
    LOB.cpp
)
add_library(lob_lib ${SOURCES_LOB})
target_link_libraries(lob_lib ${Boost_LIBRARIES})

