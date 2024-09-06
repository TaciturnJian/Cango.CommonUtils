#pragma once

#include <array>
#include <atomic>
#include <concepts>
#include <cstdint>
#include <memory>

#include <Cango/CommonUtils/ScopeNotifier.hpp>

namespace Cango :: inline CommonUtils {
	/// @brief 三重物品缓冲池，用于在两个线程中无阻塞地存取较新的数据。
	///	@details 
	///		缓冲池的工作时涉及到三个角色：读取者，写入者，数据池。
	///		读取者从数据池取出物品，写入者向数据池放入物品。
	///		二者调用的函数分别为 @c GetItem @c SetItem 。
	///	@tparam TItem 数据池中存储的物品的类型，要求支持默认构造和等号赋值。
	template <std::default_initializable TItem>
	class TripleItemPool final
#ifdef _DEBUG
		: EnableLogLifetime<TripleItemPool<TItem>>
#endif
	{
		/// @brief 指示资源为空，可写不可读
		static constexpr std::uint8_t Empty = 0;

		/// @brief 指示资源已满，可写可读
		static constexpr std::uint8_t Full = 1;

		/// @brief 指示资源正在被占用
		static constexpr std::uint8_t Busy = 2;

		std::atomic_uint8_t WriterIndex{0};
		std::array<std::atomic_uint8_t, 3> StatusList{};
		std::array<TItem, 3> ItemList{};

	public:
		using ItemType = TItem;

		/// @brief 向数据池中写入数据，几乎不阻塞当前线程
		void SetItem(const TItem& item) noexcept {
			std::uint8_t index{WriterIndex};
			std::uint8_t busy;
			do {
				busy = Busy;
				index = (index + 1) % 3;
			}
			while (StatusList[index].compare_exchange_weak(busy, Busy));
			ItemList[index] = item;
			StatusList[index] = Full;
			WriterIndex = index;
		}

		/// @brief 从数据池中获取数据，几乎不阻塞当前线程。在没有找到任何准备好的物品时，操作将会失败
		[[nodiscard]] bool GetItem(TItem& item) noexcept {
			for (std::uint8_t index = 0; index < 3; ++index) {
				if (std::uint8_t full = Full;
					!StatusList[index].compare_exchange_weak(full, Busy))
					continue;
				item = ItemList[index];
				StatusList[index] = Empty;
				return true;
			}
			return false;
		}
	};

	/// @brief 为了兼容之前的命名
	///	@details
	///		AsyncItemPool 名称并不准确，因为它不是异步池，只能让数据的获取和写入少一些阻塞。
	template <std::default_initializable TItem>
	using AsyncItemPool = TripleItemPool<TItem>;
}
