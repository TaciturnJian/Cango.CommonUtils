#include <Cango/CommonUtils/IntervalSleeper.hpp>
#include <iostream>
#include <random>
#include <thread>

using namespace Cango;

using namespace std::chrono_literals;

int main() {
	constexpr auto interval = 0ms;
	constexpr auto work_interval_random = 4;
	IntervalSleeper sleeper{interval};

	const auto begin1 = std::chrono::steady_clock::now();
	float wps1 = 0;
	for (int i = 0; i < 1000; i++) {
		sleeper.Sleep();

		const auto now = std::chrono::steady_clock::now();
		const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - begin1);
		const auto next_work_ms = std::abs(std::rand()) % work_interval_random;
		wps1 = i == 0 ? 1 : (float)i * 1000 / diff.count();
		/*std::cout << '[' << i << "]: now(" << diff.count() << "ms), work(" << next_work_ms << "ms), wps(" <<
			wps1 << ")\n";*/
		std::this_thread::sleep_for(std::chrono::milliseconds{next_work_ms});
	}

	const auto begin2 = std::chrono::steady_clock::now();
	float wps2 = 0;
	for (int i = 0; i < 1000; i++) {
		sleeper.Sleep();

		const auto now = std::chrono::steady_clock::now();
		const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - begin2);
		const auto next_work_ms = std::abs(std::rand()) % work_interval_random;
		wps2 = i == 0 ? 1 : (float)i * 1000 / diff.count();
		/*std::cout << '[' << i << "]: now(" << diff.count() << "ms), work(" << next_work_ms << "ms), wps(" <<
			wps2 << ")\n";*/
		std::this_thread::sleep_for(std::chrono::milliseconds{next_work_ms});
	}


	std::cout << "wps1(" << wps1 << "), wps2(" << wps2 << ")\n";

	return 0;
}
