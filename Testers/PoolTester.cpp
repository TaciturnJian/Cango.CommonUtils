#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <thread>
#include <vector>
#include <Cango/CommonUtils/AsyncItemPool.hpp>
#include <Cango/CommonUtils/IntervalSleeper.hpp>
#include <Cango/CommonUtils/JoinThreads.hpp>
#include <fmt/format.h>

using namespace Cango;
using namespace std::chrono_literals;

namespace
{
	struct Package
	{
		int ID{};
		std::array<int, 10> HeavyData{};
	};

	struct StaticSleeper
	{
		std::chrono::milliseconds duration;
		void Sleep() noexcept { std::this_thread::sleep_for(duration); }
	};

	int main()
	{
		TripleItemPool<Package> pool{};
		std::atomic_bool writer_done{};

		using clock_type = std::chrono::high_resolution_clock;
		using sleeper_type = IntervalSleeper;

		static constexpr auto package_to_write = 1000;
		static constexpr auto writer_interval = 4ms;
		static constexpr auto reader_interval = 2ms;
		auto print_writer_reader_info_csv = [](const int id, const long writer, const long reader)
			{
				std::cout << id << "," << writer << "," << reader << "," << reader - writer << "\n";
			};
		auto print_writer_info_csv = [](const int id, const long writer)
			{
				std::cout << id << "," << writer << ",\n";
			};

		std::map<int, clock_type::time_point> writer_times{};
		std::thread writer{
			[&pool, &writer_times, &writer_done]
	 {
static auto wait_for_reader = [] { std::this_thread::sleep_for(1s); };

sleeper_type sleeper{writer_interval};
Package package{};
wait_for_reader();
for (int i = 0; i < package_to_write; i++)
{
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
			[&pool, &reader_times, &writer_done]
	 {
Package package{};
sleeper_type sleeper{reader_interval};
while (!writer_done)
{
if (!pool.GetItem(package))
{
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

		const std::size_t total{ writer_times.size() };
		const std::size_t loss{ writer_times.size() - reader_times.size() };
		std::size_t delay{};
		for (const auto& [item, time] : writer_times)
		{
			const auto writer_diff = time - begin;
			const auto writer_time = std::chrono::duration_cast<std::chrono::microseconds>(writer_diff).count();
			if (!reader_times.contains(item))
			{
				print_writer_info_csv(item, writer_time);
				continue;
			}
			const auto reader_diff = reader_times[item] - begin;
			const auto reader_time = std::chrono::duration_cast<std::chrono::microseconds>(reader_diff).count();
			delay += reader_time - writer_time;
			print_writer_reader_info_csv(item, writer_time, reader_time);
		}

		void PrintCSVInfo(std::ostream & stream, const int id, const std::int64_t writer)
		{
			stream << fmt::format("{}, {}\n", id, writer);
		}

		class Tester
		{
		public:
			using ClockType = std::chrono::high_resolution_clock;
			using SleeperType = IntervalSleeper;

			int PackageCount = 1000;
			std::chrono::milliseconds WriterInterval = 4ms;
			std::chrono::milliseconds ReaderInterval = 2ms;

			TripleItemPool<Package> Pool{};
			std::atomic_bool WriterDone{};
			std::map<int, ClockType::time_point> WriterTimes{};
			std::map<int, ClockType::time_point> ReaderTimes{};
			ClockType::time_point begin{};

		private:
			void Write()
			{
				static auto wait_for_reader = [] { std::this_thread::sleep_for(1s); };
				std::mt19937 generator{ std::random_device{}() };
				const auto mean = WriterInterval.count() * 1000;
				const auto double_mean = mean * 2;
				std::normal_distribution distribution{
					static_cast<double>(WriterInterval.count()) * 1000.0
				};
				auto narrow_result = [double_mean](std::int64_t result)
					{
						if (result < 1000) result = 1000;
						else if (result > double_mean) result = double_mean;
						return result / 1000;
					};

				Package package{};
				wait_for_reader();
				for (int i = 0; i < PackageCount; i++)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds{
						narrow_result(static_cast<std::int64_t>(distribution(generator)))
						});
					const auto now = ClockType::now();
					package.ID = i;
					Pool.SetItem(package);
					WriterTimes[i] = now;
				}
				wait_for_reader();
				WriterDone = true;
			}

			void Read()
			{
				Package package{};
				SleeperType sleeper{ ReaderInterval };
				while (!WriterDone)
				{
					if (!Pool.GetItem(package))
					{
						sleeper.Sleep();
						continue;
					}
					const auto now = ClockType::now();
					ReaderTimes[package.ID] = now;
				}
			}

		public:
			void Begin()
			{
				ThreadList threads{};
				auto writer = [this] { Write(); };
				auto reader = [this] { Read(); };
				threads << writer << reader;

				begin = ClockType::now();
				JoinThreads(threads);
			}

			void Summary(std::ostream& stream)
			{
				const std::size_t total{ WriterTimes.size() };
				const std::size_t loss{ WriterTimes.size() - ReaderTimes.size() };
				std::size_t delay{};
				stream << "id, writer_time, reader_time, delay\n";
				for (const auto& [item, time] : WriterTimes)
				{
					const auto writer_diff = time - begin;
					const auto writer_time = std::chrono::duration_cast<std::chrono::microseconds>(writer_diff).count();
					if (!ReaderTimes.contains(item))
					{
						PrintCSVInfo(stream, item, writer_time);
						continue;
					}
					const auto reader_diff = ReaderTimes[item] - begin;
					const auto reader_time = std::chrono::duration_cast<std::chrono::microseconds>(reader_diff).count();
					delay += reader_time - writer_time;
					PrintCSVInfo(stream, item, writer_time, reader_time);
				}

				const auto avg_delay = static_cast<float>(delay) / static_cast<float>(total - loss);
				const auto summary = fmt::format("[Summary] total({}) loss({}) avg_delay({}us)\n", total, loss, avg_delay);
				if (loss > 0) stream << summary;
				std::cout << summary;
			}

			void Summary(const std::filesystem::path& directory, const char suffix)
			{
				auto file_name = fmt::format("w{}r{}{}.csv", WriterInterval.count(), ReaderInterval.count(), suffix);
				auto file_path = directory / file_name;
				std::fstream stream{ file_path, std::ios::out };
				if (!stream.is_open())
				{
					std::cerr << fmt::format("无法写入文件: {}\n", file_path.string());
					return;
				}
				Summary(stream);
			}
		};
	}

	int main(const int argc, const char* argv[])
	{
		if (argc < 3)
		{
			fmt::print("参数： <directory> <suffix>");
			return 1;
		}

		const std::filesystem::path directory{ argv[1] };
		const char suffix = argv[2][0];

		auto test = [&directory, suffix](const std::chrono::milliseconds reader)
			{
				Tester tester{};
				tester.WriterInterval = 4ms;
				tester.ReaderInterval = reader;
				tester.Begin();
				tester.Summary(directory, suffix);
			};

		test(1ms);
		test(2ms);
		test(3ms);
		test(4ms);

		return 0;
	}
