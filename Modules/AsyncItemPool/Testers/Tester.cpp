#include <Cango/CommonUtils/AsyncItemPool.hpp>
#include <iostream>
#include <chrono>
#include <map>
#include <thread>
#include <vector>

using namespace Cango;
using namespace std::chrono_literals;

namespace {
	struct Package {
		int ID{};
		std::array<int, 10> HeavyData{};
	};
}

int main() {
	TripleItemPool<Package> pool{};
	std::atomic_bool writer_done{};

	std::map<int, std::chrono::steady_clock::time_point> writer_times{};
	std::thread writer{
		[&pool, &writer_times, &writer_done] {
			std::this_thread::sleep_for(10ms);
			Package package{};
			for (int i = 0; i < 10000; i++) {
				package.ID = i;
				pool.SetItem(package);
				const auto time = std::chrono::steady_clock::now();
				writer_times[i] = time;
				std::this_thread::sleep_for(4ms);
			}

			std::this_thread::sleep_for(1s);
			writer_done = true;
		}
	};

	std::map<int, std::chrono::steady_clock::time_point> reader_times{};
	std::thread reader{
		[&pool, &reader_times, &writer_done] {
			Package package{};
			while (!writer_done) {
				if (pool.GetItem(package)) {
					const auto now = std::chrono::steady_clock::now();
					reader_times[package.ID] = now;
				}
				std::this_thread::sleep_for(1ms);
			}
		}
	};

	const auto begin = std::chrono::steady_clock::now();
	reader.join();
	writer.join();

	std::size_t total{writer_times.size()}, loss{writer_times.size() - reader_times.size()};
	std::size_t delay{};
	for (const auto& [item, time] : writer_times) {
		const auto writer_time = std::chrono::duration_cast<std::chrono::milliseconds>(time - begin).count();
		if (reader_times.contains(item)) {
			const auto reader_time =
				std::chrono::duration_cast<std::chrono::milliseconds>(reader_times[item] - begin).count();
			delay += reader_time - writer_time;
		}
	}

	const auto avg_delay = static_cast<float>(delay) / static_cast<float>(total - loss);
	std::cout << "[Summary] total(" << total << ") loss(" << loss << ") avg_delay(" << avg_delay << "ms)\n";

	return 0;
}
