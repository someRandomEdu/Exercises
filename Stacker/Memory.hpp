#ifndef MEMORY_DEFINED
#define MEMORY_DEFINED

#pragma once

#include <type_traits>
#include <memory>
#include <exception>
#include <utility>

#include "Lib.hpp"

#define STACK_ALLOC(T, LENGTH) static_cast<T *>(alloca((LENGTH) * ::Lib::Memory::SizeOf<T>()))

namespace Lib::Memory // Temporary const T & parameters are valid until the end of their containing methods
{
	template <typename T>
	using ConstValue = std::conditional_t<(sizeof(T) < sizeof(void *)), const T, const T &>;

	template <typename T>
	constexpr usize SizeOf() noexcept
	{
		if constexpr (std::is_void_v<T>)
		{
			return static_cast<usize>(1);
		}
		else if constexpr (std::is_reference_v<T>)
		{
			return sizeof(void *);
		}
		else
		{
			return sizeof(T);
		}
	}

	constexpr bool IsNull(std::nullptr_t) noexcept
	{
		return true;
	}

	template <typename T>
	constexpr bool IsNull(T *value) noexcept
	{
		return value == nullptr;
	}

	template <typename T>
	constexpr bool IsNull(T &value) noexcept
	{
		return std::addressof(value) == nullptr;
	}

	template <typename TResult, typename TParam>
	constexpr TResult *PtrCast(TParam *ptr) noexcept
	{
		return static_cast<TResult *>(const_cast<void *>(static_cast<const volatile void *>(ptr)));
	}

	template <typename T>
	void Construct(T &destination, const T &value)
	{
		if constexpr (std::is_bounded_array_v<T>)
		{
			constexpr usize length = sizeof(T) / sizeof(std::remove_extent_t<T>);

			for (usize i = 0; i < length; ++i)
			{
				Construct(destination[i], value[i]);
			}
		}
		else
		{
			new (std::addressof(destination)) T(value);
		}
	}

	template <typename T>
	void Construct(T &destination, T &&value)
	{
		if constexpr (std::is_bounded_array_v<T>)
		{
			constexpr usize length = sizeof(T) / sizeof(std::remove_extent_t<T>);

			for (usize i = 0; i < length; ++i)
			{
				Construct(destination[i], value[i]);
			}
		}
		else
		{
			new (std::addressof(destination)) T(value);
		}
	}

	template <typename T>
	constexpr void Destruct(T &value) noexcept(noexcept(std::destroy_at(std::addressof(value))))
	{
		std::destroy_at(std::addressof(value));
	}

	template <typename T>
	void Copy(const T &source, std::remove_const_t<T> &destination) noexcept
	{
		const byte *src = PtrCast<const byte>(std::addressof(source));
		std::copy(src, src + sizeof(T), PtrCast<byte>(std::addressof(destination)));
	}

	template <typename T, auto...>
	class SharedPtr;

	template <typename T>
	class SharedPtr<T> final
	{
	private:
		std::remove_const_t<T> *ptr;
		mutable int *useCount;

		struct InPlaceTag {};
		
		template <typename ...TParams>
		constexpr SharedPtr(InPlaceTag, TParams &&...params) noexcept(noexcept(T(std::forward<TParams>(params)...))) :
			ptr(new T(std::forward<TParams>(params)...)), useCount(new int(1)) {}

	public:
		using value_type = T;

		constexpr SharedPtr() noexcept = default;
		constexpr SharedPtr(std::nullptr_t) noexcept : SharedPtr() {}
		constexpr SharedPtr(T *ptr) noexcept : ptr(ptr), useCount(ptr != nullptr ? new int(1) : nullptr) {}
		constexpr SharedPtr(T &&value) noexcept(noexcept(T(std::move(value)))) : ptr(new T(std::move(value))), useCount(new int(1)) {}

		constexpr SharedPtr(const SharedPtr &other) noexcept : ptr(other.ptr), useCount(other.useCount)
		{
			++(*useCount);
		}

		constexpr SharedPtr(SharedPtr &&other) noexcept : ptr(std::exchange(other.ptr, nullptr)), 
			useCount(std::exchange(other.useCount, nullptr)) {}

		constexpr int GetUseCount() const noexcept
		{
			return useCount != nullptr ? (*useCount) : 0;
		}

		constexpr const T *operator->() const noexcept
		{
			return ptr;
		}

		constexpr T *operator->() noexcept
		{
			return ptr;
		}

		constexpr const T &operator*() const noexcept
		{
			return *operator->();
		}

		constexpr T &operator*() noexcept
		{
			return *operator->();
		}

		constexpr explicit operator bool() const noexcept
		{
			return ptr != nullptr;
		}

		template <typename ...TParams>
		static constexpr SharedPtr New(TParams &&...params) noexcept(noexcept(T(std::forward<TParams>(params)...)))
		{
			return SharedPtr(InPlaceTag{}, std::forward<TParams>(params)...);
		}

		~SharedPtr()
		{
			if (useCount != nullptr && (--(*useCount)) <= 0)
			{
				int &count = *useCount;
				--count;

				if (count <= 0)
				{
					delete useCount;
					delete ptr;
				}
			}
		}
	};

	template <typename T, auto Deleter>
	class SharedPtr<T, Deleter> final
	{
	private:
		T *ptr;
		int *useCount;

	public:
		SharedPtr() noexcept = default;

		SharedPtr(T *ptr) noexcept : ptr(ptr), useCount(new int(1)) {}
		SharedPtr(T *ptr, decltype(Deleter)) noexcept : SharedPtr(ptr) {}

		SharedPtr(const SharedPtr &other) noexcept : ptr(other.ptr), useCount(other.useCount)
		{
			++(*useCount);
		}

		int GetUseCount() const noexcept
		{
			return useCount != nullptr ? (*useCount) : 0;
		}

		constexpr const T *operator->() const noexcept
		{
			return ptr;
		}

		constexpr T *operator->() noexcept
		{
			return ptr;
		}

		constexpr const T &operator*() const noexcept
		{
			return *operator->();
		}

		constexpr T &operator*() noexcept
		{
			return *operator->();
		}

		~SharedPtr() noexcept(noexcept(Deleter(ptr)))
		{
			if (useCount != nullptr)
			{
				int &count = useCount;
				--count;

				if (count <= 0)
				{
					if (ptr != nullptr)
					{
						Deleter(ptr);
					}

					delete useCount;
				}
			}
		}
	};

	template <typename T, typename ...TParams>
	constexpr SharedPtr<T> MakeShared(TParams &&...params)
	{
		return SharedPtr<T>::New(std::forward<TParams>(params)...);
	}

	template <typename T>
	class LazyPtr final
	{
	private:
		T *ptr;
		bool isDetached;

	public:
		constexpr LazyPtr() noexcept = default;
		constexpr LazyPtr(T *ptr) noexcept : ptr(ptr), isDetached(false) {}
		constexpr LazyPtr(T &ref) noexcept : ptr(std::addressof(ref)), isDetached(false) {}

		constexpr LazyPtr(LazyPtr &&other) noexcept : ptr(other.ptr), isDetached(other.isDetached)
		{
			other.ptr = nullptr;
			other.isDetached = true;
		}

		constexpr LazyPtr &operator=(LazyPtr &&other) noexcept
		{
			T *temp = other.ptr;
			bool detached = other.isDetached;
			other.ptr = nullptr;
			other.isDetached = true;
			ptr = temp;
			isDetached = detached;
			return *this;
		}

		constexpr bool HasValue() const noexcept
		{
			return ptr != nullptr;
		}

		constexpr bool IsDetached() const noexcept
		{
			return isDetached;
		}

		constexpr const T *operator->() const noexcept
		{
			return ptr;
		}

		constexpr T *operator->() noexcept(noexcept(T(*ptr)))
		{
			if (!isDetached)
			{
				T *old = ptr;
				ptr = new T(*old);
				isDetached = true;
			}

			return ptr;
		}

		constexpr const T &operator*() const noexcept
		{
			return *operator->();
		}

		constexpr T &operator*() noexcept
		{
			return *operator->();
		}

		constexpr friend bool operator==(const LazyPtr &lhs, const LazyPtr &rhs) noexcept
		{
			return lhs.ptr == rhs.ptr;
		}

		constexpr friend bool operator!=(const LazyPtr &lhs, const LazyPtr &rhs) noexcept
		{
			return lhs.ptr != rhs.ptr;
		}

		constexpr ~LazyPtr() noexcept(noexcept(delete ptr))
		{
			if (isDetached)
			{
				delete ptr;
			}
		}
	};

	template <typename T>
	struct NonNull final // TODO: Move semantics?
	{
	private:
		T *ptr;

	public:
		constexpr NonNull(T *ptr) noexcept : ptr(ptr)
		{
			if (ptr == nullptr)
			{
				throw std::exception("The pointer passed in is null!");
			}
		}

		constexpr NonNull(T &ref) noexcept : ptr(std::addressof(ref)) {}

		constexpr const T *operator->() const noexcept
		{
			return ptr;
		}

		constexpr T *operator->() noexcept
		{
			return ptr;
		}

		constexpr const T &operator*() const noexcept
		{
			return *ptr;
		}

		constexpr T &operator*() noexcept
		{
			return *ptr;
		}

		constexpr operator const T *() const noexcept
		{
			return ptr;
		}

		constexpr operator T *() noexcept
		{
			return ptr;
		}
	};

	/// @brief Ref<T> allows a nullptr or a non temporary std::remove_const_t<T> &, but not a temporary const T &&!
	/// @tparam T 
	template <typename T>
	struct Ref final
	{
	private:
		T *value;

	public:
		using value_type = T;

		constexpr Ref() noexcept = default;
		constexpr Ref(std::nullptr_t) noexcept : value(nullptr) {}
		constexpr Ref(T &ref) noexcept : value(std::addressof(ref)) {}
		Ref(const T &&) = delete;

		constexpr bool HasValue() const noexcept
		{
			return value != nullptr;
		}

		constexpr const T *operator->() const noexcept
		{
			return value;
		}

		constexpr T *operator->() noexcept
		{
			return value;
		}

		constexpr operator const T &() const noexcept
		{
			return *value;
		}

		constexpr operator T &() noexcept
		{
			return *value;
		}

		constexpr operator Ref<const T>() const noexcept
		{
			return Ref<const T>(*value);
		}

		constexpr friend bool operator==(const Ref &ref, std::nullptr_t) noexcept
		{
			return ref.value == nullptr;
		}

		constexpr friend bool operator==(std::nullptr_t, const Ref &ref) noexcept
		{
			return ref.value == nullptr;
		}

		constexpr friend bool operator!=(const Ref &ref, std::nullptr_t) noexcept
		{
			return ref.value != nullptr;
		}

		constexpr friend bool operator!=(std::nullptr_t, const Ref &ref) noexcept
		{
			return ref.value != nullptr;
		}

		constexpr friend bool operator==(const Ref &ref, Null) noexcept
		{
			return ref.value == nullptr;
		}

		constexpr friend bool operator==(Null, const Ref &ref) noexcept
		{
			return ref.value == nullptr;
		}

		constexpr friend bool operator!=(const Ref &ref, Null) noexcept
		{
			return ref.value != nullptr;
		}

		constexpr friend bool operator!=(Null, const Ref &ref) noexcept
		{
			return ref.value != nullptr;
		}
	};
}

namespace Lib::Memory::Allocation
{
	template <typename T>
	struct Allocator
	{
	public:
		constexpr Allocator() noexcept = default;

		template <typename TOther>
		constexpr Allocator(const Allocator<TOther> &) noexcept {}

		[[nodiscard]]
		constexpr std::remove_const_t<T> *allocate(usize size) const
		{
			return std::allocator<std::remove_const_t<T>>().allocate(size);
		}

		constexpr void deallocate(std::remove_const_t<T> *ptr, usize length) const
		{
			std::allocator<std::remove_const_t<T>>().deallocate(ptr, length);
		}

		template <typename TOther>
		constexpr friend bool operator==(const Allocator &, const Allocator<TOther> &) noexcept
		{
			return true;
		}

		template <typename TOther>
		constexpr friend bool operator!=(const Allocator &, const Allocator<TOther> &) noexcept
		{
			return false;
		}
	};

	template <typename TAllocator>
	struct AllocatorPtr final
	{
	public:
		using value_type = typename TAllocator::value_type;

	private:
		std::remove_const_t<value_type> *ptr;

	public:
		constexpr AllocatorPtr(TAllocator &allocator, usize length) noexcept(noexcept(allocator.allocate(ptr, length))) :
			ptr(allocator.allocate(ptr, length)) {}

		// TODO: Forwarding TAllocator to automatically pick the correct one?
		constexpr void Deallocate(TAllocator &allocator, usize length) const noexcept(noexcept(allocator.deallocate(ptr, length)))
		{
			allocator.allocate(ptr, length);
		}
	};
}

namespace Lib::Memory::Unsafe
{
	template <typename T>
	struct Out final
	{
	private:
		T &ref;
		bool isWritten;

	public:
		using value_type = T;

		constexpr Out(T &ref) noexcept : ref(ref), isWritten(false) {}

		constexpr Out &operator=(T &ref) noexcept
		{
			this->ref = ref;
			isWritten = true;
			return *this;
		}

		constexpr Out &operator=(T &&ref) noexcept
		{
			this->ref = ref;
			isWritten = true;
			return *this;
		}

		constexpr bool IsWritten() const noexcept
		{
			return isWritten;
		}

		constexpr explicit operator bool() const noexcept
		{
			return isWritten;
		}

		constexpr ~Out() noexcept(false)
		{
			if (!isWritten)
			{
				throw std::exception("Reference is not written!");
			}
		}
	};

	template <typename T>
	union Manual
	{
	public:
		using value_type = T;

		std::remove_const_t<T> value;

	private:
		struct InPlaceTag {};

		template <typename ...TParams>
		constexpr Manual(InPlaceTag, TParams &&...params) noexcept(noexcept(T(std::forward<TParams>(params)...))) :
			value(T(std::forward<TParams>(params)...)) {}

	public:
		constexpr Manual() noexcept {}

		constexpr Manual(const Manual &other) noexcept
		{
			Copy(other.value, value);
		}

		constexpr Manual &operator=(const Manual &other) noexcept
		{
			Copy(other.value, value);
			return *this;
		}

		template <typename ...TParams>
		constexpr void Construct(TParams &&...params) noexcept(noexcept(T(std::forward<TParams>(params)...)))
		{
			new (std::addressof(value)) T(std::forward<TParams>(params)...);
		}

		constexpr void CopyFrom(const T &value) noexcept
		{
			Copy(value, this->value);
		}

		constexpr void CopyFrom(const Manual &other) noexcept
		{
			CopyFrom(other.value);
		}

		constexpr void Destruct() noexcept(noexcept(Lib::Memory::Destruct(value)))
		{
			Lib::Memory::Destruct(value);
		}

		static constexpr Manual Uninitialized() noexcept
		{
			return Manual(InPlaceTag{});
		}

		template <typename ...TParams>
		static constexpr Manual New(TParams &&...params) noexcept(noexcept(T(std::forward<TParams>(params)...)))
		{
			return Manual(InPlaceTag{}, std::forward<TParams>(params)...);
		}

		constexpr ~Manual() noexcept {}
	};

	template <typename T>
	struct SharedValue final
	{
	private:
		struct InPlaceTag {};

		Manual<T> value;
		mutable int *useCount;

		template <typename ...TParams>
		SharedValue(InPlaceTag, TParams &&...params) noexcept(noexcept(T(std::forward<TParams>(params)...))) : useCount(new int(1))
		{
			value.Construct(std::forward<TParams>(params)...);
		}

	public:
		SharedValue() noexcept = default;

		SharedValue(T &&value) noexcept(noexcept(T(std::move(value)))) : useCount(new int(1))
		{
			this->value.Construct(std::move(value));
		}

		SharedValue(const SharedValue &other) noexcept : value(other.value), useCount(other.useCount)
		{
			++(*useCount);
		}

		SharedValue(SharedValue &&other) noexcept : value(other.value), useCount(std::exchange(other.useCount, nullptr)) {}

		int GetUseCount() const noexcept
		{
			return useCount != nullptr ? *useCount : 0;
		}

		const T *operator->() const noexcept
		{
			return std::addressof(value.value);
		}

		T *operator->() noexcept
		{
			return std::addressof(value.value);
		}

		const T &operator*() const noexcept
		{
			return value.value;
		}

		T &operator*() noexcept
		{
			return value.value;
		}

		explicit operator bool() const noexcept
		{
			return useCount != nullptr;
		}

		template <typename ...TParams>
		static SharedValue New(TParams &&...params) noexcept(noexcept(T(std::forward<TParams>(params)...)))
		{
			return SharedValue(InPlaceTag{}, std::forward<TParams>(params)...);
		}

		~SharedValue()
		{
			if (useCount != nullptr)
			{
				int &count = *useCount;
				--count;

				if (count <= 0)
				{
					delete useCount;
					value.Destruct();
				}
			}
		}
	};
}

#endif // !MEMORY_DEFINED
