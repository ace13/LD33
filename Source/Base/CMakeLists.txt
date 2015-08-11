SET(Headers
	Engine.hpp
	Fonts.hpp
	Music.hpp
	Particles.hpp
	Profiling.hpp
	Tweening.hpp
	VectorMath.hpp
)

SET(Sources
	Engine.cpp
	Fonts.cpp
	Music.cpp
	Particles.cpp
	Profiling.cpp
	Tweening.cpp
	VectorMath.cpp
)

IF (NOT WIN32 AND NOT APPLE)
    FIND_PACKAGE(FontConfig)
    IF (FONTCONFIG_FOUND)
        ADD_DEFINITIONS(-DFONTCONFIG)
    ENDIF()
    INCLUDE_DIRECTORIES(${FONTCONFIG_INCLUDE_DIR})
ENDIF()

ADD_LIBRARY(LD33_Base STATIC ${Headers} ${Sources})
TARGET_LINK_LIBRARIES(LD33_Base ${KUNLABORO_LIBRARIES} ${SFML_LIBRARIES})

IF (NOT WIN32 AND NOT APPLE)
    TARGET_LINK_LIBRARIES(LD33_Base ${FONTCONFIG_LIBRARIES})
ENDIF()
