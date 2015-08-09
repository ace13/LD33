SET(Headers
	Engine.hpp
	Music.hpp
	Particles.hpp
	Tweening.hpp
	VectorMath.hpp
)

SET(Sources
	Engine.cpp
	Music.cpp
	Particles.cpp
	Tweening.cpp
	VectorMath.cpp
)

ADD_LIBRARY(LD33_Base STATIC ${Headers} ${Sources})