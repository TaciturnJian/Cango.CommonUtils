#include <Cango/CommonUtils/ScopeNotifier.hpp>
#include <spdlog/spdlog.h>

namespace Cango :: inline CommonUtils {
	ScopeNotifier::ScopeNotifier(const std::string_view name) noexcept :
		ScopeName(name),
		Logger(spdlog::default_logger()),
		LogLevel(spdlog::level::debug) {
		const auto message = fmt::format("进入范围：{}", ScopeName);
		if (Logger != nullptr) Logger->log(LogLevel, message);
		else spdlog::log(LogLevel, message);
	}

	ScopeNotifier::~ScopeNotifier() noexcept {
		const auto message = fmt::format("退出范围：{}", ScopeName);
		if (Logger != nullptr) Logger->log(LogLevel, message);
		else spdlog::log(LogLevel, message);
	}

	ScopeNotifier::ScopeNotifier(
		const std::string_view name,
		std::shared_ptr<spdlog::logger> logger,
		const spdlog::level::level_enum level) noexcept :
		ScopeName(name), Logger(std::move(logger)), LogLevel(level) {}
}
