#pragma once

#include <cstdint>
#include <concepts>

namespace Cango:: inline CommonUtils {
	/// @brief 整型计数器，指定最大值，计数到最大值后返回 @c true 否则每次计数时返回 @c false
	template <std::integral TNumber>
	struct CounterX final {
		TNumber Current{};
		TNumber MaxCount{};

		[[nodiscard]] bool Count() noexcept {
			if (Current < MaxCount - 1) {
				++Current;
				return false;
			}
			Current = MaxCount;
			return true;
		}

		/// @brief 检查是否到达了最大值 ( @c Counter::Current >= @c Counter::MaxCount )
		[[nodiscard]] bool IsReached() const noexcept { return Current >= MaxCount; }

		void Reset() noexcept { Current = 0; }
	};

	using Counter8 = CounterX<std::uint8_t>;
	using Counter16 = CounterX<std::uint16_t>;
	using Counter32 = CounterX<std::uint32_t>;
	using Counter64 = CounterX<std::uint64_t>;

	using Counter = Counter32;
}
