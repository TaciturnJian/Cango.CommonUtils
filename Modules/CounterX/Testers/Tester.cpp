#include <Cango/CommonUtils/CallCounterX.hpp>
#include <iostream>
#include <thread>

using namespace Cango;
using namespace std::chrono_literals;

int main() {
	CallRateCounterX<std::uint32_t> counter;

	const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	while (true) {
		std::this_thread::sleep_for(200ms);
		const auto now = std::chrono::steady_clock::now();
		const auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - begin).count();
		const auto rating = counter.Call(now);

		std::cout << '[' << duration << "] " << rating << '\r';
		std::cout.flush();
	}
}
