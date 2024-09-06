#pragma once

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

	/// @brief 通知生命周期，在构造和销毁时打印日志
	///	@details
	///		使用 CRTP 模式，需要对象继承此类，不会影响对象的内存布局。
	///		继承后，对象的构造和销毁时会调用额外函数，打印日志。
	template<spdlog::level::level_enum TLevel, typename TObject>
	struct LifeTimeNotifier {
		using ObjectType = TObject;

		LifeTimeNotifier() noexcept {
			auto&& id = typeid(TObject);
			spdlog::log(TLevel, "({}:{})> 构造对象", id.hash_code(), id.name());
		}

		~LifeTimeNotifier() noexcept {
			auto&& id = typeid(TObject);
			spdlog::log(TLevel, "({}:{})> 销毁对象", id.hash_code(), id.name());
		}
	};

	template<typename TObject>
	using LifeTimeTraceNotifier = LifeTimeNotifier<spdlog::level::trace, TObject>;

	template<typename TObject>
	using LifeTimeDebugNotifier = LifeTimeNotifier<spdlog::level::debug, TObject>;

	template<typename TObject>
	using LifeTimeInfoNotifier = LifeTimeNotifier<spdlog::level::info, TObject>;

	template<typename TObject>
	using LifeTimeWarnNotifier = LifeTimeNotifier<spdlog::level::warn, TObject>;

	template<typename TObject>
	using LifeTimeErrorNotifier = LifeTimeNotifier<spdlog::level::err, TObject>;

	template<typename TObject>
	struct EnableLogLifetime {
		EnableLogLifetime() noexcept {
			auto&& id = typeid(TObject);
			spdlog::debug("({}:{})> 构造对象", id.hash_code(), id.name());
		}

		~EnableLogLifetime() noexcept {
			auto&& id = typeid(TObject);
			spdlog::debug("({}:{})> 销毁对象", id.hash_code(), id.name());
		}
	};
}
