#pragma once

#include <cmath>

/** Easing functions
 * \note These functions are from https://github.com/warrenm/AHEasing
 */
namespace Tween
{
	typedef float(*TweenFunc)(float);

	/// f(t) = t
	inline float LinearIn(float t) { return t; }
	/// f(t) = t
	inline float LinearOut(float t) { return t; }
	/// f(t) = t^2
	inline float QuadraticIn(float t) { return t * t; }
	/// f(t) = -t^2+2t
	inline float QuadraticOut(float t) { return -(t * (t - 2)); }
	/// f(t) = t^3
	inline float CubicIn(float t) { return t * t * t; }
	/// f(t) = (t-1)^3+1
	inline float CubicOut(float t) { float f = t - 1;  return f * f * f + 1; }
	/// f(t) = t^4
	inline float QuarticIn(float t) { return t * t * t * t; }
	/// f(t) = 1-(t-1)^4
	inline float QuarticOut(float t) { float f = t - 1;  return f * f * f * (1 - f) + 1; }
	/// f(t) = t^5
	inline float QuinticIn(float t) { return t * t * t * t * t; }
	/// f(t) = (t-1)^5+1
	inline float QuinticOut(float t) { float f = t - 1; return f * f * f * f * f + 1; }
	/// f(t) = 2^(10(t-1))
	inline float ExponentialIn(float t) { return t == 0 ? 0 : std::pow(2.f, 10.f * (t - 1)); }
	/// f(t) = 2^(-10t) + 1
	inline float ExponentialOut(float t) { return t == 1 ? 1 : 1 - std::pow(2.f, -10.f * t); }
	/// f(t) = t^3-t*sin(t*pi)
	inline float BackIn(float t) { return t * t * t - t * std::sin(t * float(M_PI)); }
	/// f(t) = 1-((1-t)^3-(1-t)sin((1-t)pi)
	inline float BackOut(float t) { float f = 1 - t; return 1 - (f * f * f - f * std::sin(f * float(M_PI))); }
	float BounceOut(float);
	/// f(t) = MAGIC
	inline float BounceIn(float t) { return 1 - BounceOut(1 - t); }
	/// f(t) = MAGIC
	inline float BounceOut(float t) {
		if (t < 4.f / 11.f) return (121.f * t * t) / 16.f;
		else if (t < 8.f / 11.f) return (363.f / 40.f * t * t) - (99.f / 10.f * t) + 17.f / 5.f;
		else if (t < 9.f / 10.f) return (4356.f / 361.f * t * t) - (35442.f / 1805.f * t) + 16061.f / 1805.f;
		else return (54.f / 5.f * t * t) - (513.f / 25.f * t) + 268.f / 25.f;
	}
}

/// Keeps track of a delayed easing
class Easer
{
public:
	Easer(Tween::TweenFunc func = Tween::LinearIn);
	Easer(const Easer&) = default;
	~Easer() = default;
	Easer& operator=(const Easer&) = default;

	/// Select the tweening function
	void setFunc(Tween::TweenFunc func);
	/// Starts a new ease from begin to end over duration
	void start(float begin, float end, float duration);
	/// Resets the ease back to the beginning, optionally also reversing
	void reset(bool reverse = false);
	/// Reverses the ease values
	void reverse();
	/// Moves the ease forwards one dt
	void update(float dt);

	/// Gets the current value
	float operator*() const;

	/// Has the ease reached the end
	bool atEnd() const;

private:
	float mBegin, mEnd, mDuration, mCurrent;
	Tween::TweenFunc mFunc;
};
