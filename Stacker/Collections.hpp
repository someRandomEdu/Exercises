#ifndef COLLECTIONS_DEFINED
#define COLLECTIONS_DEFINED

#pragma once

#include <array>
#include <vector>
#include <memory>
#include <algorithm>
#include <initializer_list>

#include "Lib.hpp"

namespace Lib::Collections::Helpers
{
	namespace
	{
		template <typename>
		struct ArrayLengthHelper;

		template <typename T, const usize Length>
		struct ArrayLengthHelper<T[Length]>
		{
		public:
			static constexpr usize GetLength() noexcept
			{
				return Length;
			}
		};
	}

	template <typename TCollection, typename TFunc>
	constexpr void Iterate(TCollection &&collection, TFunc &&func)
	{
		for (auto value : collection)
		{
			func(value);
		}
	}

	template <typename T, typename TCollection>
	constexpr bool Contains(const TCollection &collection, const T &value) noexcept
	{
		for (T temp : collection)
		{
			if (temp == value)
			{
				return true;
			}
		}

		return false;
	}

	template <typename TInt, const usize Length>
	constexpr TInt GetLength(const TInt(&lengths)[Length]) noexcept
	{
		std::remove_const_t<TInt> result = lengths[0];

		for (usize i = 1; i < Length; ++i)
		{
			result *= lengths[i];
		}

		return result;
	}

	template <typename TInt, const usize Length>
	constexpr TInt GetIndex(const TInt(&indices)[Length], const TInt(&lengths)[Length]) noexcept
	{
		std::remove_const_t<TInt> result = indices[0];

		for (usize i = 1; i < Length; ++i)
		{
			std::remove_const_t<TInt> temp = indices[i];

			//for (usize j = i - 1; j >= 0; --j)
			for (usize j = i - 1; j > 1; --j)
			{
				temp *= lengths[j];
			}

			result += temp;
		}

		return result;
	}

	template <typename T>
	constexpr void RemoveAtSwapBack(std::vector<T> &values, usize index) noexcept
	{
		std::iter_swap(values.begin() + index, values.end() - 1);
		values.pop_back();
	}

	template <typename T>
	constexpr void RemoveAtSwapBack(std::vector<T> &values, typename std::vector<T>::iterator iterator) noexcept
	{
		std::iter_swap(iterator, values.end() - 1);
		values.pop_back();
	}

	template <typename T, typename TAllocator>
	constexpr void RemoveAt(std::vector<T> &vector, usize index) noexcept(noexcept(vector.erase(vector.begin() + index)))
	{
		vector.erase(vector.begin() + index);
	}

	template <typename T, typename TIterator>
	constexpr std::vector<T> vector(TIterator values, usize length)
		noexcept(noexcept(std::vector<T>(values, values + length)))
	{
		return std::vector<T>(values, values + length);
	}

	template <typename TKey, typename TValue, const usize Length>
	std::unordered_map<TKey, TValue> unordered_map(std::pair<TKey, TValue>(&&values)[Length])
	{
		std::unordered_map<TKey, TValue> result = std::unordered_map<TKey, TValue>();
		result.reserve(Length);

		for (usize i = 0; i < Length; ++i)
		{
			result.emplace(std::piecewise_construct, std::forward_as_tuple(std::move(values[i].first)), std::forward_as_tuple(std::move(values[i].second)));
		}

		return result;
	}

	template <typename TArray>
	constexpr usize GetArrayLength() noexcept
	{
		return ArrayLengthHelper<TArray>::GetLength();
	}
}

namespace Lib::Collections
{
	using namespace Lib;
	using namespace Lib::Data;
	using namespace Lib::Collections::Helpers;

	template <typename T, const usize...>
	struct Span;

	template <typename T>
	struct Span<T> final
	{
	private:
		T *ptr;
		usize length;

	public:
		constexpr Span() noexcept : ptr(nullptr), length(0) {}
		constexpr Span(T *ptr, usize length) noexcept : ptr(ptr), length(length) {}

		constexpr usize size() const noexcept
		{
			return length;
		}

		constexpr const T &operator[](usize index) const noexcept
		{
			return ptr[index];
		}

		constexpr T &operator[](usize index) noexcept
		{
			return ptr[index];
		}
	};

	template <typename T, const usize Length>
	struct Span<T, Length> final
	{
	private:
		T *ptr;

	public:
		constexpr Span() noexcept : ptr(nullptr) {}
		constexpr Span(T *ptr) noexcept : ptr(ptr) {}
		constexpr Span(T *ptr, usize) noexcept : ptr(ptr) {}

		constexpr usize size() const noexcept
		{
			return Length;
		}

		constexpr const T &operator[](usize index) const noexcept
		{
			return ptr[index];
		}

		constexpr T &operator[](usize index) noexcept
		{
			return ptr[index];
		}
	};

	template <typename T, const auto Dimensions = static_cast<usize>(1), typename TAllocator = std::allocator<T>>
	class Array final
	{
	public:
		using value_type = T;
		using size_type = decltype(Dimensions);
		using allocator_type = TAllocator;

	private:
		T *ptr;
		std::remove_const_t<decltype(Dimensions)> lengths[Dimensions];
		TAllocator allocator;

	public:
		constexpr Array() noexcept(noexcept(TAllocator())) = default;

		constexpr Array(const decltype(Dimensions)(&lengths)[Dimensions], const TAllocator &allocator = {}) 
			noexcept(noexcept(TAllocator(allocator)) && noexcept(this->allocator.allocate(static_cast<usize>(GetLength(lengths))))) : 
			allocator(allocator), ptr(this->allocator.allocate(static_cast<usize>(GetLength(lengths))))
		{
			std::copy(lengths, lengths + Dimensions, this->lengths);
		}

		constexpr size_type size() const noexcept
		{
			return GetLength(lengths);
		}

		constexpr const T &operator[](const decltype(Dimensions)(&indices)[Dimensions]) const noexcept
		{
			return ptr[GetIndex(indices, lengths)];
		}

		constexpr T &operator[](const decltype(Dimensions)(&indices)[Dimensions]) noexcept
		{
			return ptr[GetIndex(indices, lengths)];
		}

		constexpr ~Array() noexcept(noexcept(allocator.deallocate(ptr, static_cast<usize>(GetLength(lengths)))))
		{
			if (ptr != nullptr)
			{
				allocator.deallocate(ptr, static_cast<usize>(GetLength(lengths)));
			}
		}
	};

	template <typename T, typename TAllocator = std::allocator<T>, typename TSize = usize>
	class List final
	{
	private:
		T *ptr;
		TSize count;
		TSize actualCapacity;
		TAllocator allocator;

		void Resize()
		{
			if (ptr != nullptr)
			{
				T *oldPtr = ptr;
				TSize oldCapacity = actualCapacity;
				actualCapacity = actualCapacity * 3 / 2 + 1;
				ptr = allocator.allocate(actualCapacity);
				std::uninitialized_copy(oldPtr, oldPtr + count, ptr);
				allocator.deallocate(oldPtr, oldCapacity);
			}
			else
			{
				ptr = allocator.allocate(1);
			}
		}

	public:
		using value_type = T;
		using allocator_type = TAllocator;
		using size_type = TSize;
		using iterator = T *;
		using const_iterator = const T *;

		constexpr List() noexcept(noexcept(TAllocator())) : ptr(nullptr), count(TSize()), actualCapacity(TSize()), allocator() {}

		constexpr List(TSize capacity, const TAllocator &allocator = TAllocator()) 
			noexcept(noexcept(TAllocator(allocator)) && noexcept(this->allocator.allocate(capacity))) :
			count(TSize()), actualCapacity(capacity), allocator(allocator), ptr(this->allocator.allocate(capacity)) {}

		constexpr List(const TAllocator &allocator) noexcept(noexcept(TAllocator(allocator))) : 
			ptr(nullptr), count(TSize()), actualCapacity(TSize()), allocator(allocator) {}

		template <typename TIterator>
		constexpr List(TIterator values, TSize length, const TAllocator &allocator = TAllocator()) noexcept(noexcept(TAllocator(allocator)) && 
			noexcept(this->allocator.allocate(length)) && noexcept(std::uninitialized_copy(values, values + length, ptr))) :
			count(length), actualCapacity(length), allocator(allocator), ptr(this->allocator.allocate(length))
		{
			std::uninitialized_copy(values, values + length, ptr);
		}

		template <typename TIterator>
		constexpr List(TIterator first, TIterator last, const TAllocator &allocator = TAllocator()) 
			noexcept(noexcept(TAllocator(allocator)) && noexcept(this->allocator.allocate(actualCapacity)) &&
			std::copy(first, last, ptr)) : count(std::distance(first, last)), actualCapacity(count), allocator(allocator), 
			ptr(this->allocator.allocate(actualCapacity))
		{
			std::uninitialized_copy(first, last, ptr);
		}

		constexpr List(std::initializer_list<T> values, const TAllocator &allocator = TAllocator()) 
			noexcept(noexcept(TAllocator(allocator)) && noexcept(this->allocator.allocate(values.size())) && 
			noexcept(std::uninitialized_copy(values.begin(), values.end(), ptr))) :
			count(values.size()), actualCapacity(values.size()), allocator(allocator), ptr(this->allocator.allocate(values.size()))
		{
			std::uninitialized_copy(values.begin(), values.end(), ptr);
		}

		constexpr List(const List &other) : count(other.count), actualCapacity(other.actualCapacity), 
			allocator(other.allocator), ptr(this->allocator.allocate(other.actualCapacity))
		{
			std::uninitialized_copy(other.ptr, other.ptr + other.count, ptr);
		}

		constexpr List(List &&other) noexcept(noexcept(TAllocator(std::move(other.allocator)))) :
			ptr(std::exchange(other.ptr, nullptr)), count(other.count), actualCapacity(other.actualCapacity), 
			allocator(std::move(other.allocator)) {}

		constexpr List &operator=(const List &other)
		{
			if (ptr != nullptr)
			{
				std::destroy(ptr, ptr + count);
				allocator.deallocate(ptr, actualCapacity);
			}
			
			allocator = other.allocator;
			count = other.count;
			actualCapacity = other.actualCapacity;
			ptr = allocator.allocate(other.actualCapacity);
			std::uninitialized_copy(other.ptr, other.ptr + other.actualCapacity, ptr);
			return *this;
		}

		constexpr List &operator=(List &&other)
		{
			if (ptr != nullptr && ptr != other.ptr)
			{
				std::destroy(ptr, ptr + count);
				allocator.deallocate(ptr, actualCapacity);
			}

			allocator = std::move(other.allocator);
			count = other.count;
			actualCapacity = other.actualCapacity;
			ptr = std::exchange(other.ptr, nullptr);
			return *this;
		}

		constexpr TSize size() const noexcept
		{
			return count;
		}
		
		constexpr TSize capacity() const noexcept
		{
			return actualCapacity;
		}

		constexpr const T *cbegin() const noexcept
		{
			return ptr;
		}

		constexpr const T *cend() const noexcept
		{
			return ptr + count;
		}

		constexpr const T *begin() const noexcept
		{
			return ptr;
		}

		constexpr T *begin() noexcept
		{
			return ptr;
		}

		constexpr const T *end() const noexcept
		{
			return ptr + count;
		}

		constexpr T *end() noexcept
		{
			return ptr + count;
		}

		constexpr void Resize(TSize newCapacity)
		{
			if (ptr != nullptr)
			{
				T *oldPtr = ptr;
				TSize oldCapacity = actualCapacity;
				ptr = allocator.allocate(newCapacity);

				if (newCapacity < count)
				{
					count = newCapacity;
				}

				std::uninitialized_copy(oldPtr, oldPtr + count, ptr);
				allocator.deallocate(oldPtr, oldCapacity);
			}
			else
			{
				ptr = allocator.allocate(newCapacity);
			}
		}

		template <typename ...TParams>
		constexpr void Add(TParams &&...params)
		{
			if (size() >= capacity())
			{
				Resize();
			}
			
			new (ptr + count) T(std::forward<TParams>(params)...);
			++count;
		}

		template <typename TIterator>
		constexpr void AddRange(TIterator begin, TIterator end)
		{
			TSize length = static_cast<TSize>(std::distance(begin, end));

			if (size() + length < capacity())
			{
				Resize(size() + length);
			}

			std::uninitialized_copy(begin, end, this->end());
			count += length;
		}

		template <typename TIterator>
		constexpr void AddRange(TIterator begin, TSize length)
		{
			AddRange(begin, std::advance(begin, length));
		}

		template <typename TIterable>
		constexpr void AddRange(const TIterable &iterable)
		{
			AddRange(std::begin(iterable), std::end(iterable));
		}

		template <typename TIterable>
		constexpr void AddRange(TIterable &&iterable)
		{
			AddRange(std::make_move_iterator(std::begin(iterable)), std::make_move_iterator(std::end(iterable)));
		}

		template <const TSize Length>
		constexpr void AddRange(const T(&values)[Length])
		{
			AddRange(std::begin(values), std::end(values));
		}

		template <const TSize Length>
		constexpr void AddRange(std::remove_const_t<T>(&&values)[Length])
		{
			AddRange(std::make_move_iterator(std::begin(values)), std::make_move_iterator(std::end(values)));
		}

		constexpr bool Remove(TSize index)
		{
			if (IsInRange(index, TSize(), count))
			{
				std::move(ptr + index + 1, end(), ptr + index);
				std::destroy_at(end() - 1);
				/*std::destroy_at(ptr + index);
				std::uninitialized_copy(ptr + index + 1, ptr + count, ptr + index);*/
				--count;
				return true;
			}
			else
			{
				return false;
			}
		}

		constexpr bool Remove(const T *position)
		{
			return Remove(static_cast<TSize>(position - begin()));
		}

		constexpr bool Remove(TSize firstIndex, TSize lastIndex)
		{
			if (IsInRange(firstIndex, TSize(), count) && IsInRange(lastIndex, TSize(), count))
			{
				TSize length = lastIndex - firstIndex;
				std::move(ptr + lastIndex, end(), ptr + firstIndex);
				count -= length;
				return true;
			}
			else
			{
				return false;
			}
		}

		constexpr bool RemoveAtSwapBack(TSize index)
		{
			if (index < count && index >= 0)
			{
				std::destroy_at(ptr + index);
				std::uninitialized_copy(ptr + index + 1, ptr + count, ptr + index); // only one element is actually uninitialized though...
				--count;
				return true;
			}
			else
			{
				return false;
			}
		}

		constexpr bool RemoveAtSwapBack(const T *position)
		{
			return RemoveAtSwapBack(static_cast<TSize>(position - begin()));
		}
		
		constexpr void Clear()
		{
			std::destroy(begin(), end());
			count = TSize();
		}

		constexpr std::reverse_iterator<const T *> rbegin() const noexcept
		{
			return std::reverse_iterator<const T *>(ptr + count - 1);
		}
		
		constexpr std::reverse_iterator<T *> rbegin() noexcept
		{
			return std::reverse_iterator<T *>(ptr + count - 1);
		}

		constexpr std::reverse_iterator<const T *> rend() const noexcept
		{
			return std::reverse_iterator<const T *>(ptr - 1);
		}

		constexpr std::reverse_iterator<T *> rend() noexcept
		{
			return std::reverse_iterator<T *>(ptr - 1);
		}

		constexpr const T &operator[](TSize index) const noexcept
		{
			return ptr[index];
		}

		constexpr T &operator[](TSize index) noexcept
		{
			return ptr[index];
		}

		friend std::istream &operator>>(std::istream &stream, List &values)
		{
			if (values.ptr != nullptr)
			{
				std::destroy(values.ptr, values.ptr + count);
				allocator.deallocate(values.ptr, values.actualCapacity);
			}

			stream >> values.count;
			values.actualCapacity = values.count;
			
			for (T &value : values)
			{
				T temp = {};
				stream >> temp;
				new (std::addressof(value)) T(std::move(temp));
			}

			return stream;
		}

		friend std::ostream &operator<<(std::ostream &stream, const List &values)
		{
			for (const T &value : values)
			{
				stream << value << ' ';
			}

			return stream;
		}

		constexpr ~List() noexcept(noexcept(allocator.deallocate(ptr, actualCapacity)))
		{
			if (ptr != nullptr)
			{
				std::destroy(ptr, ptr + count);
				allocator.deallocate(ptr, actualCapacity);
			}
		}
	};

	template <typename T>
	class Matrix final
	{
	private:
		std::vector<std::vector<T>> values; // fuck std::vector<bool>...

	public:
		using value_type = T;

		constexpr Matrix() noexcept = default;

		constexpr Matrix(usize rows, usize columns) : values(std::vector<std::vector<T>>(rows, std::vector<T>(columns))) {}

		constexpr Matrix(usize rows, usize columns, const T &value) : values(std::vector<std::vector<T>>(rows, std::vector<T>(columns, value))) {}

		template <const usize Rows, const usize Columns>
		constexpr Matrix(const T(&values)[Rows][Columns]) : Matrix(Rows, Columns)
		{
			for (usize i = 0; i < Rows; ++i)
			{
				for (usize j = 0; j < Columns; ++j)
				{
					/*new (std::addressof(this->values[i][j])) T(values[i][j]);*/
					this->values[i][j] = values[i][j];
				}
			}
		}

		template <const usize Rows, const usize Columns>
		constexpr Matrix(T(&&values)[Rows][Columns]) : Matrix(Rows, Columns)
		{
			for (usize i = 0; i < Rows; ++i)
			{
				for (usize j = 0; j < Columns; ++j)
				{
					/*new (std::addressof(this->values[i][j])) T(std::move(values[i][j]));*/
					this->values[i][j] = std::move(values[i][j]);
				}
			}
		}

		constexpr usize size() const noexcept
		{
			return values.size() > 0 ? values.size() * values.front().size() : 0;
		}

		constexpr usize size(usize dimension) const noexcept
		{
			if (dimension == 0)
			{
				return values.size();
			}
			else if (dimension == 1)
			{
				return values.size() > 0 ? values.front().size() : 0;
			}
			else
			{
				return static_cast<usize>(-1);
			}
		}

		constexpr std::vector<std::vector<T>>::const_iterator begin() const noexcept
		{
			return values.begin();
		}

		constexpr std::vector<std::vector<T>>::iterator begin() noexcept
		{
			return values.begin();
		}

		constexpr std::vector<std::vector<T>>::const_iterator end() const noexcept
		{
			return values.end();
		}

		constexpr std::vector<std::vector<T>>::iterator end() noexcept
		{
			return values.end();
		}

		constexpr void ClearRow(usize row) noexcept
		{
			for (T &value : values[row])
			{
				value = T();
			}
		}

		constexpr void MoveRowToLast(usize row) noexcept
		{
			std::vector<T> temp = values[row];
			values.erase(values.begin() + row);
			values.push_back(std::move(temp));
		}

		constexpr void InsertRow(usize row, T values[]) noexcept
		{
			this->values[row].assign(values, values + size(1));
		}

		constexpr void RemoveRow(usize row) noexcept
		{
			values.erase(values.begin() + row);
		}

		constexpr void SwapRowToLast(usize row) noexcept
		{
			std::swap(values[row], values[size(0) - 1]);
		}

		constexpr void Reverse()
		{
			std::reverse(values.begin(), values.end());
		}

		constexpr void Fill(const T &value)
		{
			for (std::vector<T> &row : values)
			{
				for (T &val : row)
				{
					val = value;
				}
			}
		}

		constexpr void Clear()
		{
			Fill(T());
		}

		constexpr bool IsCleared() const noexcept
		{
			for (const std::vector<T> &row : values)
			{
				for (const T &value : row)
				{
					if (value != T())
					{
						return false;
					}
				}
			}

			return true;
		}

		template <typename TInt> requires std::is_integral_v<TInt>
		constexpr const T &operator[](const Value2<TInt> &indices) const noexcept
		{
			return values[static_cast<usize>(indices.x)][static_cast<usize>(indices.y)];
		}

		template <typename TInt> requires std::is_integral_v<TInt>
		constexpr T &operator[](const Value2<TInt> &indices) noexcept
		{
			return values[static_cast<usize>(indices.x)][static_cast<usize>(indices.y)];
		}

		constexpr const T &operator[](const usize(&indices)[2]) const noexcept
		{
			return values[indices[0]][indices[1]];
		}

		constexpr T &operator[](const usize(&indices)[2]) noexcept
		{
			return values[indices[0]][indices[1]];
		}

		friend std::istream &operator>>(std::istream &stream, Matrix &values)
		{
			usize rows;
			usize columns;
			stream >> rows >> columns;
			values.values = std::vector<std::vector<T>>(rows, std::vector<T>(columns));

			for (usize i = 0; i < rows; ++i)
			{
				for (usize j = 0; j < columns; ++j)
				{
					stream >> values[{i, j}];
				}
			}

			return stream;
		}

		friend std::ostream &operator<<(std::ostream &stream, const Matrix &values)
		{
			usize rows = values.size(0);
			usize columns = values.size(1);

			for (usize i = 0; i < rows; ++i)
			{
				for (usize j = 0; j < columns; ++j)
				{
					stream << values[{i, j}] << ' ';
				}

				stream << '\n';
			}

			return stream;
		}
	};

	template <typename T>
	struct SinglyLinkedNode final
	{
	public:
		T value;
		SinglyLinkedNode *next;
	};

	template <typename T>
	struct DoublyLinkedNode final
	{
	public:
		T value;
		DoublyLinkedNode *previous;
		DoublyLinkedNode *next;
	};

	template <typename T, typename TAllocator = std::allocator<SinglyLinkedNode<T>>>
	struct SinglyLinkedList final
	{
	private:
		SinglyLinkedNode<T> *node;
		TAllocator allocator;

	public:
	};
}

#endif // !COLLECTIONS_DEFINED
