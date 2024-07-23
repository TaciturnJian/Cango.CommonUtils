#include <Cango/CommonUtils/CallCounterX.hpp>
#include <iostream>
#include <thread>

int main() {
	Cango::CommonUtils::CallCounterX<std::uint32_t> counter;
	counter.SetDuration(std::chrono::milliseconds(1000));

	const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	while (true) {
		const auto now = std::chrono::steady_clock::now();
		std::this_thread::sleep_for(std::chrono::milliseconds(
			//std::abs(std::rand()) % 500 + 
			100
		));

		std::cout << '[' << std::chrono::duration_cast<std::chrono::seconds>(now - begin).count() << "] " << counter.Call(now) << '\n';
	}
}
