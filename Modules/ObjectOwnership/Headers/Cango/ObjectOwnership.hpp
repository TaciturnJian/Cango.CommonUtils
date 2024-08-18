#pragma once

#include <concepts>
#include <memory>

namespace Cango:: inline CommonUtils :: inline ObjectOwnership {
	namespace InternalDetails {
		struct VerySmallStruct {};

		static constexpr VerySmallStruct VerySmallStructInstance{};
	}

	template <typename T>
	using Credential = std::weak_ptr<T>;

	template <typename T>
	using ObjectUser = std::shared_ptr<T>;

	template <typename T>
	class Owner {
		std::shared_ptr<T> UserPointer;

		explicit Owner(InternalDetails::VerySmallStruct) noexcept : UserPointer{} {}

	public:
		/// @brief 创建空 Owner
		static Owner CreateEmpty() noexcept { return Owner{InternalDetails::VerySmallStructInstance}; }

		/// @brief 让 Owner 支持默认构造，Owner 会尽可能地创建一个 T 类型的对象，如果不支持，则创建空 Owner
		Owner() noexcept { if constexpr (std::default_initializable<T>) { UserPointer = std::make_shared<T>(); } }

		template <typename... TArgs>
		explicit Owner(TArgs&&... args) noexcept : UserPointer{
			std::make_shared<T>(std::forward<TArgs>(args)...)
		} {}

		Owner(Owner&) noexcept = delete;
		Owner(Owner&& other) noexcept = default;
		Owner& operator=(Owner&) noexcept = delete;
		Owner& operator=(Owner&& other) noexcept = default;

		explicit operator bool() const noexcept { return UserPointer != nullptr; }

		explicit operator ObjectUser<T>() const noexcept { return UserPointer; }

		explicit operator Credential<T>() const noexcept { return UserPointer; }

		T& operator*() const noexcept { return *UserPointer; }

		T* operator->() const noexcept { return UserPointer.get(); }
	};
}
