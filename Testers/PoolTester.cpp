#include <Cango/CommonUtils/AsyncItemPool.hpp>
#include <Cango/CommonUtils/IntervalSleeper.hpp>
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

	struct StaticSleeper {
		std::chrono::milliseconds duration;
		void Sleep() noexcept { std::this_thread::sleep_for(duration); }
	};
}

int main() {
	TripleItemPool<Package> pool{};
	std::atomic_bool writer_done{};

	using clock_type = std::chrono::high_resolution_clock;
	using sleeper_type = IntervalSleeper;

	static constexpr auto package_to_write = 1000;
	static constexpr auto writer_interval = 4ms;
	static constexpr auto reader_interval = 2ms;
	auto print_writer_reader_info_csv = [](const int id, const long writer, const long reader) {
		std::cout << id << "," << writer << "," << reader << "," << reader - writer <<"\n";
	};
	auto print_writer_info_csv = [](const int id, const long writer) {
		std::cout << id << "," << writer << ",\n";
	};
	
	std::map<int, clock_type::time_point> writer_times{};
	std::thread writer{
		[&pool, &writer_times, &writer_done] {
			static auto wait_for_reader = [] { std::this_thread::sleep_for(1s); };

			sleeper_type sleeper{writer_interval};
			Package package{};
			wait_for_reader();
			for (int i = 0; i < package_to_write; i++) {
				sleeper.Sleep();
				const auto now = clock_type::now();
				package.ID = i;
				pool.SetItem(package);
				writer_times[i] = now;
			}
			wait_for_reader();
			writer_done = true;
		}
	};

	std::map<int, clock_type::time_point> reader_times{};
	std::thread reader{
		[&pool, &reader_times, &writer_done] {
			Package package{};
			sleeper_type sleeper{reader_interval};
			while (!writer_done) {
				if (!pool.GetItem(package)) {
					sleeper.Sleep();
					continue;
				} 
				const auto now = clock_type::now();
				reader_times[package.ID] = now;
			}
		}
	};

	const auto begin = clock_type::now();
	reader.join();
	writer.join();

	const std::size_t total{writer_times.size()};
	const std::size_t loss{writer_times.size() - reader_times.size()};
	std::size_t delay{};
	for (const auto& [item, time] : writer_times) {
		const auto writer_diff = time - begin;
		const auto writer_time = std::chrono::duration_cast<std::chrono::microseconds>(writer_diff).count();
		if (!reader_times.contains(item)) {
			print_writer_info_csv(item, writer_time);
			continue;
		}
		const auto reader_diff = reader_times[item] - begin;
		const auto reader_time = std::chrono::duration_cast<std::chrono::microseconds>(reader_diff).count();
		delay += reader_time - writer_time;
		print_writer_reader_info_csv(item, writer_time, reader_time);
	}

	const auto avg_delay = static_cast<float>(delay) / static_cast<float>(total - loss);
	std::cout << "[Summary] total(" << total << ") loss(" << loss << ") avg_delay(" << avg_delay << "us)\n";

	return 0;
}
