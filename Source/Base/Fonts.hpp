#pragma once

#include <SFML/Graphics/Font.hpp>

namespace sf
{
	sf::Font& getDefaultFont();
	sf::Font getFont(const std::string& family, const std::string& style = "");
}

