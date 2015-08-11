SET(HEADERS
	Game/Components.hpp
	Game/Player.hpp
	Menu/MainMenu.hpp
	Menu/MenuPane.hpp
	Menu/OptionsMenu.hpp
	State/GameState.hpp
	State/IntroState.hpp
	State/MenuState.hpp
	Components.hpp
)

SET(SOURCES
	Game/Components.cpp
	Game/Player.cpp
	Menu/MainMenu.cpp
	Menu/MenuPane.cpp
	Menu/OptionsMenu.cpp
	State/GameState.cpp
	State/IntroState.cpp
	State/MenuState.cpp
	Components.cpp
	main.cpp
)

ADD_EXECUTABLE(MiniLD61 ${HEADERS} ${SOURCES})
TARGET_LINK_LIBRARIES(MiniLD61
	LD33_Base
)

