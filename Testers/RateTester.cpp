#include <iostream>
#include <Cango/CommonUtils.hpp>
#include <fmt/format.h>

using namespace Cango;
using namespace std::chrono_literals;

int main() {
	CallRateCounterX<std::uint32_t> counter;
	IntervalSleeper sleeper{5ms};

	const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	while (true) {
		sleeper.Sleep();
		const auto now = std::chrono::steady_clock::now();
		const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - begin).count();
		const auto rating = counter.Call(now);

		std::cout << fmt::format("[{}ms] {}cps     \r", duration, rating);
		std::cout.flush();
	}
}
