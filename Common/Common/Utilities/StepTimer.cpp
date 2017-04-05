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
	// �����������ʼ��Ϊ 1/10 �롣
	m_qpcMaxDelta = m_qpcFrequency.QuadPart / 10;
}

// �ڹ����жϼ�ʱ(���磬��ֹ�� IO ����)֮��
// ���ô˺����Ա���̶�ʱ�䲽���߼�����һϵ���ֲ�
// Update ���á�

void StepTimer::ResetElapsedTime()
{

	m_leftOverTicks = 0;
	m_framesPerSecond = 0;
	m_framesThisSecond = 0;
	m_qpcSecondCounter = 0;
}

