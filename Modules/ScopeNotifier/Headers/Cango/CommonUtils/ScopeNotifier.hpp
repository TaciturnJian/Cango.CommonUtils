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

/// @brief 监控进入退出当前上下文，或者实例的创建与销毁
/// @code
/// CANGO_SCOPE("Scope1");
/// {
/// 	CANGO_SCOPE("Scope1.1");
/// 	{
/// 		CANGO_SCOPE("Scope1.1.1");
/// 	}
/// 	{
/// 		CANGO_SCOPE("Scope1.1.2");
/// 	}
/// }
///	@endcode
#define CANGO_SCOPE(...) ::Cango::CommonUtils::ScopeNotifier CangoScopeNotifier{__VA_ARGS__}

#ifdef _DEBUG
/// @brief 在调试模式下使用 @c CANGO_SCOPE 监控，在非调试模式下不使用
#define DebugScopeNotifier(...) CANGO_SCOPE(__VA_ARGS__)
#else
/// @brief 在调试模式下使用 @c CANGO_SCOPE 监控，在非调试模式下不使用
#define DebugScopeNotifier(...)
#endif
