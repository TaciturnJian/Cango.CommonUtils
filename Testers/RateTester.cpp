
#include <iostream>
#include <thread>
#include <Cango/CommonUtils.hpp>

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

		std::cout << '[' << duration << "ms] " << rating << "cps     \r";
		std::cout.flush();
	}
}
