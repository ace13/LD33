#include "Path.hpp"

const Path Path::Invalid;


Path::Path()
{
}

void Path::begin(size_t size)
{
	mPoints.clear();
	mPoints.reserve(size);
}
void Path::push(const sf::Vector2i& pos)
{
	mPoints.push_back(pos);
}

Path::iterator Path::begin() const
{
	return mPoints.cbegin();
}
Path::iterator Path::end() const
{
	return mPoints.cend();
}