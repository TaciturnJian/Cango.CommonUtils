#pragma once

#include <concepts>
#include <memory>

#include <Cango/CommonUtils/ScopeNotifier.hpp>

#ifndef CANGO_COMMON_UTILS_INTERNAL_DETAILS
#define CANGO_COMMON_UTILS_INTERNAL_DETAILS
namespace Cango::InternalDetails {
	struct EmptyStruct {};

	static constexpr EmptyStruct EmptyStructInstance{};
}
#endif

namespace Cango:: inline CommonUtils :: inline ObjectOwnership {
	template <typename T>
	using Credential = std::weak_ptr<T>;

	template <typename T>
	using ObjectUser = std::shared_ptr<T>;

	template <typename T>
	class Owner final
#ifdef _DEBUG
		: EnableLogLifetime<Owner<T>>
#endif
	{
		std::shared_ptr<T> UserPointer;

		explicit Owner(InternalDetails::EmptyStruct) noexcept : UserPointer{} {}

	public:
		using element_type = T;

		/// @brief 确保创建一个空 Owner
		static Owner CreateEmpty() noexcept { return Owner{InternalDetails::EmptyStructInstance}; }

		/// @brief 让 Owner 支持默认构造，Owner 会尽可能地创建一个 T 类型的对象，如果不支持，则创建空 Owner
		Owner() noexcept { if constexpr (std::default_initializable<T>) { UserPointer = std::make_shared<T>(); } }

		template <typename... TArgs>
		explicit Owner(TArgs&&... args) noexcept : UserPointer{
			std::make_shared<T>(std::forward<TArgs>(args)...)
		} {}

		Owner(Owner&) noexcept = delete;
		Owner& operator=(Owner&) noexcept = delete;

		Owner(Owner&& other) noexcept : UserPointer(std::move(other.UserPointer)) {}

		Owner& operator=(Owner&& other) noexcept {
			if (this != &other) { UserPointer = std::move(other.UserPointer); }
			return *this;
		}

		/// 检查 Owner 是否有效
		explicit operator bool() const noexcept { return UserPointer != nullptr; }

		// ReSharper disable once CppNonExplicitConversionOperator
		operator ObjectUser<T>() const noexcept { return UserPointer; }

		// ReSharper disable once CppNonExplicitConversionOperator
		operator Credential<T>() const noexcept { return UserPointer; }

		T& operator*() const noexcept { return *UserPointer; }

		T* operator->() const noexcept { return UserPointer.get(); }
	};
}
