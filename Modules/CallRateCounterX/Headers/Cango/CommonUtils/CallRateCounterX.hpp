#pragma once

#include <cstdint>
#include <chrono>
#include <concepts>

namespace Cango :: inline CommonUtils {
	/// @brief 调用速率计数器，单位是(次/s)，支持的频率范围为 (1, @c std::numeric_limits<TNumber>::max() )。
	///	@details
	///		内部最小时间单位为毫秒。
	///		如果调用速率小于等于 1hz 那么输出结果极其不稳定。
	///		理论可以避免整数溢出。
	template <std::integral TNumber>
	class CallRateCounterX {
		static constexpr std::chrono::milliseconds UpdateDuration{1000};
		static constexpr std::chrono::milliseconds TripleDuration{3000};

		// UpdateDuration
		// vvv
		// [ ]|[ ]|[ ]
		// ^^^^^^^
		// TripleDuration

		bool IsSetSomeTime{false};
		std::chrono::steady_clock::time_point BeginTime{};
		std::chrono::steady_clock::time_point SomeTime{};
		TNumber Count{};
		TNumber CountAtSomeTime{};

		void Reset(const std::chrono::steady_clock::time_point& now) noexcept {
			BeginTime = now;
			Count = 0;
		}

		void RecordSomeTime(const std::chrono::steady_clock::time_point& now) noexcept {
			IsSetSomeTime = true;
			SomeTime = now;
			CountAtSomeTime = Count;
		}

		void ReduceCount() noexcept {
			IsSetSomeTime = false;
			BeginTime = SomeTime;
			Count -= CountAtSomeTime;
		}

		static std::chrono::milliseconds ToMS(const auto& duration) {
			return std::chrono::duration_cast<std::chrono::milliseconds>(duration);
		}

		static float GetFrequency(const TNumber count, const std::chrono::milliseconds& duration) noexcept {
			return static_cast<float>(count) * 1000.0f / static_cast<float>(duration.count());
		}

	public:
		/// @brief 使用提供的当前时间更新计数
		float Call(const std::chrono::steady_clock::time_point& now) noexcept {
			// [x]|[ ]|[ ]

			++Count;
			const auto diff = ToMS(now - BeginTime);
			if (diff < UpdateDuration) return static_cast<float>(Count);

			// [ ]|[x]|[ ]

			if (diff < TripleDuration) {
				if (!IsSetSomeTime) RecordSomeTime(now);
				return GetFrequency(Count, diff);
			}

			// [ ]|[ ]|[x]

			// 检查是否经过了第二次时间，如果没有经过，则陷入错误状态，更新时间太长，计数器无法正常工作，需要重置计数器
			if (!IsSetSomeTime) {
				Reset(now);
				return 0;
			}

			// 利用记录的第二段时间中某个节点的时间点，减少计数防止溢出
			ReduceCount();
			return GetFrequency(Count, ToMS(now - BeginTime));
		}

		/// @brief 使用 @c std::chrono::steady_clock 提供的当前时间更新计数
		float Call() noexcept { return Call(std::chrono::steady_clock::now()); }
	};

	using CallRateCounter16 = CallRateCounterX<std::uint16_t>;
	using CallRateCounter32 = CallRateCounterX<std::uint32_t>;
	using CallRateCounter64 = CallRateCounterX<std::uint64_t>;
	using CallRateCounter = CallRateCounter32;
}
