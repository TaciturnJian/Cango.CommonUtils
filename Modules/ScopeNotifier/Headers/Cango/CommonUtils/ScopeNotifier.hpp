#pragma once

#include <string_view>
#include <spdlog/logger.h>

namespace Cango :: inline CommonUtils {
	class ScopeNotifier {
		std::string ScopeName;
		std::shared_ptr<spdlog::logger> Logger;
		spdlog::level::level_enum LogLevel;

	public:
		ScopeNotifier(
			std::string_view name,
			std::shared_ptr<spdlog::logger> logger,
			spdlog::level::level_enum level) noexcept;

		explicit ScopeNotifier(std::string_view name) noexcept;

		ScopeNotifier(const ScopeNotifier&) = delete;
		ScopeNotifier& operator=(const ScopeNotifier&) = delete;

		~ScopeNotifier() noexcept;
	};

	using LifeTimeNotifier = ScopeNotifier;
}
