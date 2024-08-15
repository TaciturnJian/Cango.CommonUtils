#include <thread>
#include <Cango/CommonUtils/IntervalSleeper.hpp>

namespace Cango :: inline CommonUtils {
	IntervalSleeper::IntervalSleeper(const std::chrono::milliseconds interval) noexcept: Interval(interval) {}

	void IntervalSleeper::Sleep(const std::chrono::steady_clock::time_point& now) noexcept {
		if (Interval.count() == 0) return; // 对于 0 延时的特别处理

		const auto diff = now - LastSleepTime;
		if (diff >= Interval) {
			LastSleepTime = now;
			return;
		}
		std::this_thread::sleep_for(Interval - diff);
		LastSleepTime += Interval;
	}

	void IntervalSleeper::Sleep() noexcept { Sleep(std::chrono::steady_clock::now()); }
}
