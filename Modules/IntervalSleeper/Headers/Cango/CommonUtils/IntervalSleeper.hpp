#pragma once

#include <chrono>
#include <thread>

#ifdef _DEBUG
#include <Cango/CommonUtils/ScopeNotifier.hpp>
#define CANGO_INTERVAL_SLEEPER_ENABLE_LOG_LIFETIME : EnableLogLifetime<IntervalSleeperX<TDuration, TClock>>
#else
#define CANGO_INTERVAL_SLEEPER_ENABLE_LOG_LIFETIME
#endif

namespace Cango :: inline CommonUtils {
	/// @brief 固定间隔休眠器，用来解决一些循环中帧率忽高忽低，不太平滑的问题。
	///	@details
	///		通过尽量确保相邻两次调用 Sleep 的时间间隔约为给定的 Interval，来保证帧率的稳定。
	///		从上一次调用开始计时，如果时间间隔小于 Interval，则等待 Interval - 时间间隔。
	///		若超过了 Interval，则直接返回，并且从现在开始计时。
	///
	///	@note
	///		对于写入器，之前的做法是无论有没有写入数据，都会等待一段比较小的固定时间，使用这个工具就可以平滑写入。
	///		如果你希望设置延时为 0 ，请注意这里会有一个很小的调用延时，而且它还会变化。
	///		Windows 和 Linux 上表现差距巨大。尤其是 Windows 平台上，在 Interval 太小时方差巨大，我真的爱死微软了。
	template <typename TDuration, typename TClock>
	class IntervalSleeperX final CANGO_INTERVAL_SLEEPER_ENABLE_LOG_LIFETIME {
		std::chrono::steady_clock::time_point LastSleepTime{};

	public:
		using DurationType = TDuration;
		using ClockType = TClock;

		static constexpr TDuration DefaultInterval{100};
		static constexpr std::chrono::microseconds MinInterval{100};

		TDuration Interval;

		explicit IntervalSleeperX(const TDuration interval = DefaultInterval) noexcept :
			Interval(interval) {}

		/// @brief 休眠，补足时间间隔到当前给定的时间
		void Sleep(const typename ClockType::time_point& now) noexcept {
			if (Interval.count() == 0) return; // 对于 0 延时的特别处理

			const auto diff = now - LastSleepTime;
			if (diff >= Interval) {
				LastSleepTime = now;
				return;
			}
			std::this_thread::sleep_for(Interval - diff);
			LastSleepTime += Interval;
		}

		/// @brief 休眠，补足时间间隔到 @c std::chrono::steady_clock::now()
		void Sleep() noexcept { Sleep(ClockType::now()); }
	};

	using IntervalSleeper = IntervalSleeperX<std::chrono::milliseconds, std::chrono::steady_clock>;
}

#undef CANGO_INTERVAL_SLEEPER_ENABLE_LOG_LIFETIME
