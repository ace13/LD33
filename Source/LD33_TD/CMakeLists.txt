SET(HEADERS
	Game/Components.hpp
	Menu/MainMenu.hpp
	Components.hpp
	IntroScreen.hpp
)
SET(SOURCES
	Game/Components.cpp
	Menu/MainMenu.cpp
	Components.cpp
	IntroScreen.cpp

	main.cpp
)

SOURCE_GROUP("Header files\\Game" REGULAR_EXPRESSION "Game/[^./]+.(hpp|inl)")
SOURCE_GROUP("Source files\\Game" REGULAR_EXPRESSION "Game/[^./]+.cpp")

SOURCE_GROUP("Header files\\Menu" REGULAR_EXPRESSION "Menu/[^./]+.(hpp|inl)")
SOURCE_GROUP("Source files\\Menu" REGULAR_EXPRESSION "Menu/[^./]+.cpp")

SOURCE_GROUP("Header files\\" REGULAR_EXPRESSION "[^./]+.(hpp|inl)")
SOURCE_GROUP("Source files\\" REGULAR_EXPRESSION "[^./]+.cpp")

ADD_EXECUTABLE(LD33_TD ${HEADERS} ${SOURCES})
TARGET_LINK_LIBRARIES(LD33_TD
	LD33_Base
)
