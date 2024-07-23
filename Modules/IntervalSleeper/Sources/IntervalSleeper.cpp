#include <Cango/CommonUtils/IntervalSleeper.hpp>

namespace Cango :: inline CommonUtils {
	void IntervalSleeper::Sleep(const std::chrono::steady_clock::time_point& now) noexcept {
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
