#include <Cango/CommonUtils/CallRateCounterX.hpp>
#include <iostream>
#include <thread>

using namespace Cango;
using namespace std::chrono_literals;

int main() {
	CallRateCounter counter;

	const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	for (int i = 0; i < 100; ++i) {
		std::this_thread::sleep_for(1s);
		const auto now = std::chrono::steady_clock::now();
		const auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - begin).count();
		const auto rating = counter.Call(now);

		std::cout << '[' << duration << "s] " << rating << "       " <<'\r';
		std::cout.flush();
	}

	return 0;
}
