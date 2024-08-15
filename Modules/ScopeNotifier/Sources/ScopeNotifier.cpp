#include <Cango/CommonUtils/ScopeNotifier.hpp>
#include <spdlog/spdlog.h>

namespace Cango :: inline CommonUtils {
	ScopeNotifier::ScopeNotifier(const std::string_view name) noexcept :
		ScopeNotifier(name, spdlog::default_logger(), spdlog::level::debug) {}

	ScopeNotifier::~ScopeNotifier() noexcept {
		const auto message = fmt::format("退出范围：{}", ScopeName);
		if (Logger == nullptr) spdlog::log(LogLevel, message);
		else Logger->log(LogLevel, message);
	}

	ScopeNotifier::ScopeNotifier(
		const std::string_view name,
		std::shared_ptr<spdlog::logger> logger,
		const spdlog::level::level_enum level) noexcept :
		ScopeName(name), Logger(std::move(logger)), LogLevel(level) {
		const auto message = fmt::format("进入范围：{}", ScopeName);
		if (Logger == nullptr) spdlog::log(LogLevel, message);
		else Logger->log(LogLevel, message);
	}
}
