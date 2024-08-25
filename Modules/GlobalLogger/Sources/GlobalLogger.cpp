#include <Cango/CommonUtils/GlobalLogger.hpp>
#include <ctime>
#include <iostream>
#include <spdlog/sinks/rotating_file_sink.h>
#include <sstream>

namespace Cango:: inline CommonUtils {
	/// @brief 初始化默认日志记录器
	/// 该日志记录器会同时输出到文件和控制台，并在文件达到指定大小时自动滚动
	///	@param filename 日志文件名
	///	@param level 日志记录器的最低日志等级，低于此等级的日志将不会被记录
	[[nodiscard]] bool InitializeDefaultLogger(
		const std::string_view filename,
		const spdlog::level::level_enum level) noexcept {
#pragma region make file sink helper
		static auto make_file_sink = [](const std::string_view file) {
			static constexpr std::size_t mb = 1024ull * 1024;
			static constexpr std::size_t file_size = 5 * mb;
			static constexpr std::size_t file_count = 0xffff;
			return std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
				std::string{file}, file_size, file_count, true);
		};
#pragma endregion

		try {
			spdlog::default_logger()->set_level(level);
			spdlog::default_logger()->sinks().emplace_back(make_file_sink(filename));
			return true;
		}
		catch (const std::exception& ex) {
			std::cerr << "无法初始化通用日志：" << ex.what() << '\n';
			return false;
		}
		catch (...) {
			std::cerr << "无法初始化通用日志：未知异常\n";
			return false;
		}
	}

	/// @brief 初始化全局日志记录器
	/// 该日志记录器会同时输出到文件和控制台，并在文件达到指定大小时自动滚动
	///	@param appName 输出的文件将会保存在 logs/appName_年-月-日_时-分-秒-[.%d]
	///	@return 是否初始化成功，错误消息将会输出到 std::err
	[[nodiscard]] bool InitializeGlobalLogger(std::string_view appName) noexcept {
		const std::time_t current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		const auto* current_tm = std::localtime(&current_time); // NOLINT(concurrency-mt-unsafe)
		std::ostringstream stream{};
		const auto path = fmt::format(
			"logs/{}_{:0>4}-{:0>2}-{:0>2}_{:0>2}-{:0>2}-{:0>2}_",
			appName,
			current_tm->tm_year + 1900,
			current_tm->tm_mon + 1,
			current_tm->tm_mday,
			current_tm->tm_hour,
			current_tm->tm_min,
			current_tm->tm_sec
		);

		return InitializeDefaultLogger(path);
	}
}
