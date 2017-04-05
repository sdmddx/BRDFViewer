#include "stdafx.h"


using namespace Utilities;

StepTimer::StepTimer() :
	m_elapsedTicks(0),
	m_totalTicks(0),
	m_leftOverTicks(0),
	m_frameCount(0),
	m_framesPerSecond(0),
	m_framesThisSecond(0),
	m_qpcSecondCounter(0),
	m_isFixedTimeStep(false),
	m_targetElapsedTicks(TicksPerSecond / 60)
{
	if (!QueryPerformanceFrequency(&m_qpcFrequency))
	{
		//throw ref new Platform::FailureException();
	}

	if (!QueryPerformanceCounter(&m_qpcLastTime))
	{
		//throw ref new Platform::FailureException();
	}
	// 将最大增量初始化为 1/10 秒。
	m_qpcMaxDelta = m_qpcFrequency.QuadPart / 10;
}

// 在故意中断计时(例如，阻止性 IO 操作)之后
// 调用此函数以避免固定时间步长逻辑尝试一系列弥补
// Update 调用。

void StepTimer::ResetElapsedTime()
{

	m_leftOverTicks = 0;
	m_framesPerSecond = 0;
	m_framesThisSecond = 0;
	m_qpcSecondCounter = 0;
}

