#include <iostream>
#include <Cango/CommonUtils.hpp>
#include <fmt/format.h>
#include <vector>

using namespace Cango;
using namespace std::chrono_literals;
using namespace std::chrono;

int main() {

	spdlog::set_level(spdlog::level::debug);

	CallRateCounter counter;
	IntervalSleeper sleeper{5ms};

	const steady_clock::time_point begin = steady_clock::now();
	ScopeNotifier notifier{"RateTester"};
	for (int i = 0; i < 1000; i++) {
		sleeper.Sleep();
		const auto now = steady_clock::now();
		const auto rating = counter.Call(now);
		const auto duration = duration_cast<milliseconds>(now - begin).count();
		std::cout << fmt::format("[{}ms] {}cps     \n", duration, rating);
		std::cout.flush();
	}
}
