#include "Profiling.hpp"

#include <iomanip>
#include <iostream>

#ifdef SFML_CLOCK
#include <SFML/System/Clock.hpp>
#endif

namespace
{
#ifdef SFML_CLOCK
	sf::Clock _ProfilingClock;
#endif
	Profiler::Node ProfilingRoot("root");
	Profiler::Node* sCurNode = &ProfilingRoot;
}

#ifdef SFML_CLOCK
const bool Profiler::Clock::is_steady = true;
Profiler::Clock::time_point Profiler::Clock::now()
{
	return time_point(duration(_ProfilingClock.getElapsedTime().asMicroseconds()));
}
#endif

void Profiler::startBlock(const std::string& name)
{
	if (name != sCurNode->mName)
		sCurNode = sCurNode->getChild(name);
	sCurNode->startCall();
}
void Profiler::endBlock()
{
	if (sCurNode->endCall())
		if (sCurNode->mParent)
			sCurNode = sCurNode->mParent;
}
void Profiler::resetBlocks()
{
	ProfilingRoot.reset();
}
void Profiler::releaseMemory()
{
	ProfilingRoot.cleanup();
}
Profiler::Node* Profiler::getRoot()
{
	return &ProfilingRoot;
}
Profiler::Iterator Profiler::begin()
{
	return Iterator(&ProfilingRoot);
}
Profiler::Iterator Profiler::end()
{
	return Iterator(&ProfilingRoot, true);
}

Profiler::Block::Block(const std::string& name)
{
	Profiler::startBlock(name);
}
Profiler::Block::~Block()
{
	Profiler::endBlock();
}

Profiler::Node::Node(const std::string& name, Node* parent):
	mName(name), mStartTime(TimePoint {}), mMaxTime(TimeSpan {}),
	mMinTime(TimeSpan {}), mTotalTime(TimeSpan {}), mCallCount(0),
	mRecursion(0), mParent(parent), mChild(nullptr), mSibling(nullptr)
{
}
Profiler::Node::~Node()
{
	cleanup();
}

Profiler::Iterator Profiler::Node::begin() const
{
	return Iterator(this);
}
Profiler::Iterator Profiler::Node::end() const
{
	return Iterator(this, true);
}
void Profiler::Node::startCall()
{
	mCallCount++;
	if (mRecursion++ == 0)
	{
		mStartTime = Clock::now();
	}
}
bool Profiler::Node::endCall()
{
	if (--mRecursion == 0)
	{
		TimePoint endTime = Clock::now();
		TimeSpan diff = endTime - mStartTime;
		if (mMinTime == TimeSpan{} || mMinTime > diff)
			mMinTime = diff;
		if (mMaxTime < diff)
			mMaxTime = diff;
		mTotalTime += diff;
		mStartTime = {};
	}

	return mRecursion == 0;
}
void Profiler::Node::reset()
{
	mCallCount = 0;
	mStartTime = {};
	mTotalTime = {};
	mMinTime = {};
	mMaxTime = {};

	if (mChild)
		mChild->reset();
	if (mSibling)
		mSibling->reset();
}
void Profiler::Node::cleanup()
{
	if (mChild)
		delete mChild;
	if (mSibling)
		delete mSibling;

	mChild = nullptr;
	mSibling = nullptr;
}
Profiler::Node* Profiler::Node::getChild(const std::string& name)
{
	Node* newNode = nullptr;
	if (!mChild)
	{
		newNode = new Node(name, this);
		mChild = newNode;
		return newNode;
	}

	Node* it = mChild, *old = nullptr;
	do
	{
		old = it;
		if (it->mName == name)
			return it;
	} while ((it = it->mSibling));

	newNode = new Node(name, this);
	old->mSibling = newNode;

	return newNode;
}
void Profiler::Node::toStream(std::ostream& os, unsigned int level) const
{
	for (unsigned int i = 0; i < level; ++i)
		os << "  ";

	os << mName;
	if (mCallCount > 0)
	{
		float avgTime = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(getAvgTime()).count();
		os << " - " << std::setprecision(2) << std::fixed << avgTime << " ms";
	}
	if (mCallCount > 1)
		os << " * " << mCallCount;

	if (level > 0 && mParent && mParent != &ProfilingRoot)
	{
		float perc = mTotalTime / mParent->mTotalTime;
		if (perc > 0.05)
			os << " (" << std::setprecision(2) << "%)";
	}

	if (mChild)
	{
		if (mChild->mCallCount > 0)
		{
			os << std::endl;
			mChild->toStream(os, level + 1);
		}

		Node* si = mChild->mSibling;
		while (si)
		{
			if (si->mCallCount > 0)
			{
				os << std::endl;
				si->toStream(os, level + 1);
			}
			si = si->mSibling;
		}
	}
}
std::string Profiler::Node::getName() const
{
	return mName;
}
Profiler::TimeSpan Profiler::Node::getAvgTime() const
{
	return mTotalTime / mCallCount;
}
Profiler::TimeSpan Profiler::Node::getMinTime() const
{
	return mMinTime;
}
Profiler::TimeSpan Profiler::Node::getMaxTime() const
{
	return mMaxTime;
}
Profiler::TimeSpan Profiler::Node::getTotalTime() const
{
	return mTotalTime;
}
unsigned int Profiler::Node::getCallCount() const
{
	return mCallCount;
}

Profiler::Iterator::Iterator(const Iterator& rhs) :
	mCurChild(rhs.mCurChild), mCurParent(rhs.mCurParent)
{
}
Profiler::Iterator::Iterator(Iterator&& rhs) :
	mCurChild(std::move(rhs.mCurChild)), mCurParent(std::move(rhs.mCurParent))
{
}
Profiler::Iterator::~Iterator()
{
}
Profiler::Iterator& Profiler::Iterator::operator=(const Iterator& rhs)
{
	if (this == &rhs)
		return *this;

	mCurParent = rhs.mCurParent;
	mCurChild = rhs.mCurChild;

	return *this;
}
Profiler::Iterator& Profiler::Iterator::operator++()
{
	if (mCurChild)
		mCurChild = mCurChild->mSibling;
	return *this;
}
Profiler::Iterator& Profiler::Iterator::operator+=(int num)
{
	while (mCurChild && num-- > 0)
		mCurChild = mCurChild->mSibling;
	return *this;
}
Profiler::Iterator::reference Profiler::Iterator::operator*() const
{
	return *mCurChild;
}
Profiler::Iterator::pointer Profiler::Iterator::operator->() const
{
	return mCurChild;
}
Profiler::Iterator::pointer Profiler::Iterator::getParent() const
{
	return mCurParent;
}
Profiler::Iterator::Iterator(const Node* node, bool atEnd) :
	mCurChild(nullptr), mCurParent(node)
{
	if (!atEnd && node)
		mCurChild = node->mChild;
}
