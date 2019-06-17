/// @file SConfig.h
/// @brief 设置基本常量参数
/// @date 2019/3/31

#pragma once
#include "stdafx.h"


namespace fl {
	constexpr int MILISECOND_PER_FRAME = 30;                                     ///< 每帧设定的毫秒数
	constexpr clock_t CLOCKS = CLOCKS_PER_SEC / 1000;                            ///< 每毫秒的时钟数
	const wstring TRANSLATE_FONT[] = { L"微软雅黑",L"宋体",L"黑体",L"仿宋" };      ///< 基本字体
	constexpr int MAX_LOADSTRING = 100;                                          ///< 窗体类名最大长度
}