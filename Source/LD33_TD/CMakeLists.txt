SET(HEADERS
	Game/Enemies/Basic.hpp
	Game/Towers/Basic.hpp
	Game/UI/StatDisplay.hpp
	Game/UI/TowerRadial.hpp
	Game/Components.hpp
	Game/Enemy.hpp
	Game/GameScreen.hpp
	Game/Level.hpp
	Game/Path.hpp
	Game/Tower.hpp
	Menu/MainMenu.hpp
	Components.hpp
	IntroScreen.hpp
)
SET(SOURCES
	Game/Enemies/Basic.cpp
	Game/Towers/Basic.cpp
	Game/UI/StatDisplay.cpp
	Game/UI/TowerRadial.cpp
	Game/Components.cpp
	Game/Enemy.cpp
	Game/GameScreen.cpp
	Game/Level.cpp
	Game/Path.cpp
	Game/Tower.cpp
	Menu/MainMenu.cpp
	Components.cpp
	IntroScreen.cpp

	main.cpp
)

SOURCE_GROUP("Header files\\Game" REGULAR_EXPRESSION "Game/[^./]+.(hpp|inl)")
SOURCE_GROUP("Source files\\Game" REGULAR_EXPRESSION "Game/[^./]+.cpp")
SOURCE_GROUP("Header files\\Game\\Enemies" REGULAR_EXPRESSION "Game/Enemies/[^./]+.(hpp|inl)")
SOURCE_GROUP("Source files\\Game\\Enemies" REGULAR_EXPRESSION "Game/Enemies/[^./]+.cpp")
SOURCE_GROUP("Header files\\Game\\Towers" REGULAR_EXPRESSION "Game/Towers/[^./]+.(hpp|inl)")
SOURCE_GROUP("Source files\\Game\\Towers" REGULAR_EXPRESSION "Game/Towers/[^./]+.cpp")
SOURCE_GROUP("Header files\\Game\\UI" REGULAR_EXPRESSION "Game/UI/[^./]+.(hpp|inl)")
SOURCE_GROUP("Source files\\Game\\UI" REGULAR_EXPRESSION "Game/UI/[^./]+.cpp")

SOURCE_GROUP("Header files\\Menu" REGULAR_EXPRESSION "Menu/[^./]+.(hpp|inl)")
SOURCE_GROUP("Source files\\Menu" REGULAR_EXPRESSION "Menu/[^./]+.cpp")

SOURCE_GROUP("Header files\\" REGULAR_EXPRESSION "[^./]+.(hpp|inl)")
SOURCE_GROUP("Source files\\" REGULAR_EXPRESSION "[^./]+.cpp")

ADD_EXECUTABLE(LD33_TD ${HEADERS} ${SOURCES})
TARGET_LINK_LIBRARIES(LD33_TD
	LD33_Base
)
