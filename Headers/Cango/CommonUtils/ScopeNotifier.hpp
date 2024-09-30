#pragma once

#include <boost/type_index.hpp>
#include <spdlog/spdlog.h>

namespace Cango :: inline CommonUtils {
	/// @brief 较为复杂的生命周期通知器，在运行时指定范围名称，日志器，日志等级，在构造和销毁时打印日志
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

	/// @brief 调试用，启动类实例的生命周期日志记录，在实例创建和销毁时，打印日志。
	/// @tparam TDerived 继承类，使用 CRTP 以确保功能正常
	template<typename TDerived>
	class EnableLogLifetime {
	public:
		EnableLogLifetime() noexcept {
			spdlog::debug("{}> 构造对象", boost::typeindex::type_id<TDerived>().pretty_name());
		}

		~EnableLogLifetime() noexcept {
			spdlog::debug("{}> 销毁对象", boost::typeindex::type_id<TDerived>().pretty_name());
		}
	};
}

namespace cango {
	template<typename T>
	using enable_log_lifetime = Cango::CommonUtils::EnableLogLifetime<T>;
}
