#pragma once

#include <atomic>
#include <cstdint>
#include <memory>

namespace Cango :: inline CommonUtils {
	/// @brief
	///		用于异步存储和取值的数据池，用于在两个线程中不重复读取到较新的写入数据。
	///		工作方式如下：
	///		          *--Pool---*
	///		writer ->-+ SetItem |
	///		          | GetItem +->- reader
	///		          *---------*
	///		此数据池在同一时间只支持一个写入器(writer)和一个读取器(reader)。
	///		writer 写入器，调用此类型对象的 @c AsyncItemPool::SetItem 函数写入数据，一定成功；
	///		reader 读取器，调用此类型对象的 @c AsyncItemPool::GetItem 函数获取数据，可能失败；
	///		当且仅当 writer 写入速度大于 reader 读取速度时，GetItem 操作才能每次都成功。
	///	@tparam TItem 数据池中存储的物品的类型
	///	@todo 关于多线程中读写的原子性需要更专业的测试
	template <std::default_initializable TItem>
	class AsyncItemPool final {
		/// @brief 用于标记数据池中的数据状态，分别为空（可写入），满（可写入、可读取），忙（不可写入、不可读取）
		enum ViewStatus : std::uint8_t {
			Empty = 0, Full = 1, Busy = 2
		};

		struct ItemInfo final {
			std::atomic<ViewStatus> Status{ViewStatus::Empty};
			TItem Item{};
		};

		std::atomic_uchar WriterIndex{0};
		std::array<ItemInfo, 3> ItemInfoList{};

	public:
		using ItemType = TItem;

		/// @brief 向数据池中写入数据，正常情况下不阻塞当前线程，必定成功
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

		/// @brief 从数据池中获取数据，几乎不阻塞当前线程，可能会失败
		///	@return 当获取数据失败时，也就是没有任何已经准备好读取的数据时，返回 @c false
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
}
