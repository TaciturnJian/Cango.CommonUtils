#pragma once

#include <array>
#include <atomic>
#include <cstdint>
#include <memory>

namespace Cango :: inline CommonUtils {
	/// @brief 三重物品缓冲池，用于在两个线程中无阻塞地存取数据。
	///		缓冲池的工作时涉及到三个角色：读取者(reader)，写入者(writer)，数据池(Pool)。
	///		写入者向数据池放入物品，读取者从数据池取出物品，二者调用的函数分别为 @c GetItem @c SetItem 。
	///	@tparam TItem 数据池中存储的物品的类型，要求可默认初始化( @c std::default_initializable )，还要支持赋值(等号表达式)构造。
	/// @note 模板的限制并不严谨，但是满足大多数使用情况。
	template <std::default_initializable TItem>
	class TripleItemPool final {
		/// @brief 指示资源为空，可写不可读
		static constexpr std::uint8_t Empty = 0;

		/// @brief 指示资源已满，可写可读
		static constexpr std::uint8_t Full = 1;

		/// @brief 指示资源正在被占用
		static constexpr std::uint8_t Busy = 2;

		struct ItemInfo final {
			std::atomic_uint8_t Status{Empty};
			TItem Item{};
		};

		std::atomic_uint8_t WriterIndex{0};
		std::array<ItemInfo, 3> ItemInfoList{};

	public:
		using ItemType = TItem;

		/// @brief 向数据池中写入数据，几乎不阻塞当前线程
		void SetItem(const TItem& item) noexcept {
			ItemInfo* info_ptr;
			do {
				WriterIndex = (WriterIndex + 1) % 3;
				info_ptr = &ItemInfoList[WriterIndex];
			}
			while (info_ptr->Status == Busy);
			info_ptr->Status = Busy;
			info_ptr->Item = item;
			info_ptr->Status = Full;
		}

		/// @brief 从数据池中获取数据，几乎不阻塞当前线程，如果当前没有任何准备好的物品时，操作将会失败
		[[nodiscard]] bool GetItem(TItem& item) noexcept {
			for (auto& [status, my_item] : ItemInfoList) {
				if (status != Full) continue;
				status = Busy;
				item = my_item;
				status = Empty;
				return true;
			}

			return false;
		}
	};

	/// @brief 为了兼容之前的命名
	///		实际上 AsyncItemPool 名称不准确，因为它并不是异步的，只是无阻塞的。
	template <std::default_initializable TItem>
	using AsyncItemPool = TripleItemPool<TItem>;
}
