#pragma once

#include <chrono>

namespace Cango :: inline CommonUtils {
	/// @brief 调用次数计数器，返回指定 Duration 内的调用次数
	///	统计最大周期将是三个 Duration 的长度
	///	当每次到达第三个 Duration 后，将重置计数从最后一个 Duration 开始
	///	Duration 计数
	template <typename TNumber>
	class CallCounterX {
		bool IsSetSomeTime{false};
		std::chrono::steady_clock::time_point BeginTime{};
		std::chrono::steady_clock::time_point SomeTime{};
		std::chrono::milliseconds Duration{1000};
		std::chrono::milliseconds TripleDuration{3000};
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
		[[nodiscard]] std::chrono::milliseconds GetDuration() const noexcept { return Duration; }

		void SetDuration(const std::chrono::milliseconds duration) noexcept {
			Duration = duration;
			TripleDuration = Duration * 3;
		}

		float Call(const std::chrono::steady_clock::time_point& now) {
			++Count;

			const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - BeginTime);

			// 如果连第一段时间都没有到达，那么直接返回计数即可
			if (diff < Duration) return static_cast<float>(Count);

			// 如果到达了第一段时间，没有超过第三段时间，那么记录当前的计数，用于后续避免溢出的计算
			if (diff < TripleDuration) {
				if (!IsSetSomeTime) RecordSomeTime(now);
				return static_cast<float>(Count) * 1000.0f / diff.count();
			}

			// 如果到达了第三段时间，但是没有记录到第二段时间，那么计数器陷入错误的状态（观测调用Duration太短），重置计数器
			if (!IsSetSomeTime) {
				Reset(now);
				return Call(now);
			}

			// 利用记录的第二段时间中某个节点的时间点，减少计数防止溢出
			ReduceCount();
			return static_cast<float>(Count) * 1000.0f /
				std::chrono::duration_cast<std::chrono::milliseconds>(now - BeginTime).count();
		}
	};
}
