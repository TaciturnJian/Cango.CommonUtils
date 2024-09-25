#pragma once

#include <spdlog/spdlog.h>

namespace Cango :: inline CommonUtils {
	/// @brief 初始化默认日志记录器
	/// 该日志记录器会同时输出到文件和控制台，并在文件达到指定大小时自动滚动
	///	@param filename 日志文件名
	///	@param level 日志记录器的最低日志等级，低于此等级的日志将不会被记录
	[[nodiscard]] bool InitializeDefaultLogger(
		std::string_view filename,
		spdlog::level::level_enum level = spdlog::level::level_enum::trace) noexcept;

	/// @brief 初始化全局日志记录器
	/// 该日志记录器会同时输出到文件和控制台，并在文件达到指定大小时自动滚动
	///	@param appName 输出的文件将会保存在 logs/appName_年-月-日_时-分-秒-[.%d]
	///	@return 是否初始化成功，错误消息将会输出到 std::err
	[[nodiscard]] bool InitializeGlobalLogger(std::string_view appName) noexcept;
}
