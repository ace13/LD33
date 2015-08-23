#include "Tweening.hpp"

#include <algorithm>

Easer::Easer(Tween::TweenFunc func) :
	mBegin(0), mEnd(0), mDuration(0), mCurrent(0),
	mFunc(func)
{

}

void Easer::setFunc(Tween::TweenFunc func)
{
	mFunc = func;
}
void Easer::start(float begin, float end, float duration)
{
	mBegin = begin;
	mEnd = end;
	mDuration = duration;

	mCurrent = 0;
}
void Easer::reset(bool reverse)
{
	mCurrent = 0;

	if (reverse)
		Easer::reverse();
}
void Easer::reverse()
{
	float temp = mEnd;
	mEnd = mBegin;
	mBegin = temp;
}
void Easer::update(float dt)
{
	mCurrent = std::min(mDuration, mCurrent + dt);
}
float Easer::operator*() const
{
	if (mDuration == 0)
		return mEnd;

	float t = mFunc(mCurrent / mDuration);

	return mBegin + t * (mEnd - mBegin);
}
bool Easer::atEnd() const
{
	return mCurrent == mDuration;
}
