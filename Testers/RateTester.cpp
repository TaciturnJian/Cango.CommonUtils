#include <iostream>
#include <Cango/CommonUtils.hpp>
#include <fmt/format.h>
#include <vector>

using namespace Cango;
using namespace std::chrono_literals;

int main() {
	spdlog::set_level(spdlog::level::debug);

	CallRateCounterX<std::uint32_t> counter;
	IntervalSleeper sleeper{5ms};

	const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	ScopeNotifier _{"RateTester"};
	for (int i = 0; i < 1000; i++) {
		sleeper.Sleep();
		const auto now = std::chrono::steady_clock::now();
		const auto rating = counter.Call(now);
		const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - begin).count();

		std::cout << fmt::format("[{}ms] {}cps     \n", duration, rating);
		std::cout.flush();
	}
}
