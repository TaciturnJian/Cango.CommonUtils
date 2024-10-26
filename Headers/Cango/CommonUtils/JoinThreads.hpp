#pragma once

#include <functional>
#include <list>
#include <thread>
#include <vector>

namespace Cango:: inline CommonUtils {
	template <typename T>
	concept IsThreadContainer = requires(T& container, std::thread&& thread) {
		{ container.push_back(std::move(thread)) };
		{ container.emplace_back(std::move(thread)) };
	} && std::ranges::range<T>;

	using ThreadList = std::list<std::thread>;
	using ThreadVector = std::vector<std::thread>;

	auto& operator<<(IsThreadContainer auto& threads, std::thread&& thread) noexcept {
		threads.push_back(std::move(thread));
		return threads;
	}

	auto& operator<<(IsThreadContainer auto& threads, std::function<void()> function) noexcept {
		threads.emplace_back(function);
		return threads;
	}

	void JoinThreads(std::ranges::range auto& threads) noexcept { for (auto& thread : threads) thread.join(); }
}
