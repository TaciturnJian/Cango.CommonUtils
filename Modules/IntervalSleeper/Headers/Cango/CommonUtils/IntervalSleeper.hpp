#pragma once

#include <chrono>
#include <thread>

namespace Cango :: inline CommonUtils {
	/// @brief 尽量确保相邻两次调用 Sleep 的时间间隔约为给定的 Interval(+0.7ms，系统CPU调度延时)
	///		从上一次调用开始计时，如果时间间隔小于 Interval，则等待 Interval - 时间间隔
	///		若超过了 Interval，则直接返回，并且从现在开始计时
	///	@note
	///	此工具用来解决一些循环中帧率不太平滑的问题
	///		例如写入器，之前的做法是无论有没有写入数据，都会等待一段比较小的固定时间，这显然不太合理
	///		或者一些需要固定时间间隔的任务
	///	如果你希望设置延时为 0ms ，请注意这里会有一个 约为 0.7ms 的延时（没有具体测试过，不同情况变化很大）
	class IntervalSleeper {
		std::chrono::steady_clock::time_point LastSleepTime{};

	public:
		std::chrono::milliseconds Interval{std::chrono::milliseconds{100}};

		IntervalSleeper() noexcept = default;
		explicit IntervalSleeper(const std::chrono::milliseconds& interval) noexcept : Interval(interval) {}

		/// @brief 休眠，补足时间间隔到当前给定的时间
		void Sleep(const std::chrono::steady_clock::time_point& now) noexcept;

		/// @brief 休眠，补足时间间隔到 std::chrono::steady_clock::now()
		void Sleep() noexcept;
	};
}
