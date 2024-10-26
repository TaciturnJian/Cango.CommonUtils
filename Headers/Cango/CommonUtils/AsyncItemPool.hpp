#pragma once

#include <array>
#include <atomic>
#include <concepts>
#include <cstdint>
#include <memory>

#ifdef CANGO_COMMON_UTILS_ENABLE_SCOPE_NOTIFIER_FOR_AsyncItemPool
#include "ScopeNotifier.hpp"
/// @brief 为类添加日志记录实例的生命周期的功能
#define CANGO_TRIPLE_ITEM_POOL_ENABLE_LOG_LIFETIME : ::cango::enable_log_lifetime<TripleItemPool<TItem>>
#else
#define CANGO_TRIPLE_ITEM_POOL_ENABLE_LOG_LIFETIME
#endif

namespace Cango :: inline CommonUtils {
	/// @brief 三重字节缓冲区，用于在两个线程中无阻塞地存取较新的数据。
	/// @details
	///		擦除了类型信息，只能存取固定大小的字节数据。
	///		不提供任何异常处理，调用方保证指针有效。
	template<std::size_t TSingleItemSize>
	class NonblockTripleByteBuffer {
	public:
		static constexpr auto ItemSize = TSingleItemSize;
		static constexpr auto ItemCount = 3;

	private:
		enum Status : std::uint8_t {
			Empty = 0,
			Full = 1,
			Busy = 2
		};

		std::atomic_uint8_t WriterIndex{0};

		/// @brief 三个缓冲区，用于存储物品的字节
		std::array<std::array<std::uint8_t, ItemSize>, ItemCount> Buffer{};
		std::array<std::atomic_uint8_t, ItemCount> StatusList{};

	public:
		void WriteBytes(const void* data) noexcept {
			std::uint8_t index{WriterIndex};
			std::uint8_t busy;
			do {
				busy = Busy;
				index = (index + 1) % 3;
			}
			while (StatusList[index].compare_exchange_weak(busy, Busy));
			std::memcpy(Buffer[index].data(), data, ItemSize);
			StatusList[index] = Full;
			WriterIndex = index;
		}

		[[nodiscard]] bool ReadBytes(void* data) noexcept {
			for (std::uint8_t index = 0; index < 3; ++index) {
				if (std::uint8_t full = Full;
					!StatusList[index].compare_exchange_weak(full, Busy))
					continue;
				std::memcpy(data, Buffer[index].data(), ItemSize);
				StatusList[index] = Empty;
				return true;
			}
			return false;
		}
	};

	/// @brief 三重物品缓冲池，用于在两个线程中无阻塞地存取较新的数据。
	///	@details 
	///		缓冲池的工作时涉及到三个角色：读取者，写入者，数据池。
	///		读取者从数据池取出物品，写入者向数据池放入物品。
	///		二者调用的函数分别为 @c GetItem @c SetItem 。
	///	@tparam TItem 数据池中存储的物品的类型，要求支持默认构造和等号赋值。
	template <std::default_initializable TItem>
	class TripleItemPool final
	{
		NonblockTripleByteBuffer<sizeof(TItem)> Buffer{};

	public:
		using ItemType = TItem;

		/// @brief 向数据池中写入数据，不阻塞当前线程
		void SetItem(const TItem& item) noexcept {
			Buffer.WriteBytes(&item);
		}

		/// @brief 从数据池中获取数据，不阻塞当前线程。在没有找到任何准备好的物品时，操作将会失败
		[[nodiscard]] bool GetItem(TItem& item) noexcept {
			return Buffer.ReadBytes(&item);
		}
	};

	/// @brief 为了兼容之前的命名
	///	@details
	///		AsyncItemPool 名称并不准确，因为它不是异步池，只能让数据的获取和写入少一些阻塞。
	template <std::default_initializable TItem>
	using AsyncItemPool = TripleItemPool<TItem>;
}

#undef CANGO_TRIPLE_ITEM_POOL_ENABLE_LOG_LIFETIME
