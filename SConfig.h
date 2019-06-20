/// @file SConfig.h
/// @brief ���û�����������
/// @date 2019/3/31

#pragma once
#include "stdafx.h"


namespace fl {
	constexpr int MILISECOND_PER_FRAME = 30;                                     ///< ÿ֡�趨�ĺ�����
	constexpr clock_t CLOCKS = CLOCKS_PER_SEC / 1000;                            ///< ÿ�����ʱ����
	const wstring TRANSLATE_FONT[] = { L"΢���ź�",L"����",L"����",L"����" };      ///< ��������
	constexpr int MAX_LOADSTRING = 100;                                          ///< ����������󳤶�

#ifdef ILL_USE_DOUBLE_PRECISION
	constexpr double PI = 3.141592653589793238462643;
	constexpr double EPS = DBL_EPSILON;
	constexpr double INF = DBL_MAX;
#else
	constexpr float PI = 3.141592653589793238462643f;
	constexpr float EPS = FLT_EPSILON;
	constexpr float INF = FLT_MAX;
#endif
}