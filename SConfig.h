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
}