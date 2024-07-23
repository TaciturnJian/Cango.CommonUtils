#pragma once

#include <chrono>

namespace Cango :: inline CommonUtils {
	/// @brief 调用速率计数器，单位是 次/s
	template <typename TNumber>
	class CallRateCounterX {
		static constexpr std::chrono::milliseconds UpdateDuration{1000};
		static constexpr std::chrono::milliseconds TripleDuration{3000};

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

	public:
		float Call(const std::chrono::steady_clock::time_point& now) {
			++Count;

			const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - BeginTime);

			// 如果连第一段时间都没有到达，那么直接返回计数即可
			if (diff < UpdateDuration) return static_cast<float>(Count);

			// 如果到达了第一段时间，没有超过第三段时间，那么记录当前的计数，用于后续避免溢出的计算
			if (diff < TripleDuration) {
				if (!IsSetSomeTime) RecordSomeTime(now);
				return static_cast<float>(Count) * 1000.0f / static_cast<float>(diff.count());
			}

			// 如果到达了第三段时间，但是没有记录到第二段时间，那么计数器陷入错误的状态（观测调用Duration太短），重置计数器
			if (!IsSetSomeTime) {
				Reset(now);
				return Call(now);
			}

			// 利用记录的第二段时间中某个节点的时间点，减少计数防止溢出
			ReduceCount();
			const auto new_diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - BeginTime);
			return static_cast<float>(Count) * 1000.0f / static_cast<float>(new_diff.count());
		}
	};
}
