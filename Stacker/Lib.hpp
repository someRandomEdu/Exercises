#ifndef LIB_DEFINED
#define LIB_DEFINED

#pragma once

#include <type_traits>
#include <memory>
#include <exception>
#include <cstring>
#include <string>
#include <utility>
#include <functional>
#include <concepts>
#include <optional>
#include <bit>

#include "SDL.h"

using std::string;
using std::byte;
using Byte = std::byte;
using usize = std::size_t;
using isize = std::make_signed_t<usize>; // not named ssize because std::ssize already exists
using sbyte = enum struct SByte : std::make_signed_t<std::underlying_type_t<byte>> {};

namespace Lib::Traits
{
	template <template <typename...> typename TTemplated, typename ...TParams>
	concept CanInstantiate = requires
	{
		typename TTemplated<TParams...>;
	};
}

namespace Lib
{
	namespace
	{
		using VersionType = decltype(__cplusplus);
	}
	
	template <typename ...Ts>
	void Print(Ts &&...values)
	{
		((std::cout << std::forward<Ts>(values)), ...);
	}

	template <typename ...Ts>
	void PrintLine(Ts &&...values)
	{
		((std::cout << std::forward<Ts>(values) << '\n'), ...);
	}

	template <>
	void PrintLine<>()
	{
		std::cout << '\n';
	}

	template <typename ...Ts>
	void Read(Ts &...values)
	{
		((std::cin >> values), ...);
	}

	template <typename T>
	constexpr T SignOf(T value) noexcept
	{
		return value > T() ? static_cast<T>(1) : (value < T() ? static_cast<T>(-1) : T());
	}

	template <typename T>
	constexpr T Abs(T value) noexcept
	{
		return value < T() ? -value : value;
	}

	template <typename T>
	constexpr T Clamp(T value, T inclusiveMin, T inclusiveMax) noexcept
	{
		return value < inclusiveMin ? inclusiveMin : (value > inclusiveMax ? inclusiveMax : value);
	}

	template <typename T>
	constexpr bool IsInRange(const T &value, const T &inclusiveMin, const T &inclusiveMax) noexcept
	{
		return inclusiveMin <= value && inclusiveMax >= value;
	}

	template <typename TChar>
	constexpr usize StrLen(const TChar *str) noexcept
	{
		if (str == nullptr)
		{
			return 0;
		}
		
		if constexpr (std::is_same_v<decltype(str), const char *>)
		{
			return std::strlen(str);
		}
		else
		{
			usize result = 0;

			while (str[result] != TChar())
			{
				++result;
			}

			return result;
		}
	}

	struct Null
	{
	public:
		template <typename T>
		constexpr operator T() const noexcept 
		{
			if constexpr (std::is_pointer_v<T> || std::is_null_pointer_v<T>)
			{
				return nullptr;
			}
			else if constexpr (std::is_reference_v<T>)
			{
				return *static_cast<std::add_pointer_t<std::remove_reference_t<T>>>(nullptr);
			}
			else if constexpr (std::is_void_v<T>)
			{
				return void();
			}
			else
			{
				throw std::exception("Not a nullable type!");
			}
		}

		constexpr friend bool operator==(Null, Null) noexcept
		{
			return true;
		}

		constexpr friend bool operator!=(Null, Null) noexcept
		{
			return false;
		}

		constexpr friend std::istream &operator>>(std::istream &stream, Null &) noexcept
		{
			return stream;
		}

		constexpr friend std::ostream &operator<<(std::ostream &stream, const Null &) noexcept
		{
			return stream;
		}
	};

	constexpr inline Null null = {};

	// A way to sneak constants into type parameters
	template <auto Constant>
	struct Const
	{
	public:
		using value_type = decltype(Constant);

		static constexpr value_type value = Constant;

		constexpr operator decltype(Constant)() const noexcept
		{
			return Constant;
		}

		static constexpr decltype(Constant) Get() noexcept
		{
			return Constant;
		}
	};

	template <typename T, auto...>
	struct Literal;

	template <typename T>
	struct Literal<T> final
	{
	public:
		using value_type = T;

	private:
		T value;

	public:
		consteval Literal(T value) noexcept : value(value) {}

		constexpr T Get() const noexcept
		{
			return value;
		}

		constexpr operator T() const noexcept
		{
			return value;
		}
	};

	template <typename T, auto Constraint>
	struct Literal<T, Constraint> final
	{
	private:
		T value;

	public:
		using value_type = T;

		consteval Literal(T value) noexcept : value(Constraint(value) ? value : throw std::exception()) {}
		
		constexpr decltype(Constraint) GetConstraint() const noexcept
		{
			return Constraint;
		}

		constexpr operator T() const noexcept
		{
			return value;
		}
	};

	template <typename TChar, usize...>
	struct StringLiteral;

	template <typename TChar>
	struct StringLiteral<TChar> final
	{
	private:
		const TChar *str;

	public:
		using value_type = const TChar *;

		consteval StringLiteral(const TChar *str) noexcept : str(str != nullptr ? str : throw std::exception("Not a string literal!")) {}

		constexpr const TChar *Get() const noexcept
		{
			return str;
		}	

		template <typename TOther>
		constexpr StringLiteral(const StringLiteral<TOther> &other) noexcept : str(other.Get()) {}

		template <typename TOther>
		constexpr StringLiteral &operator=(const StringLiteral<TOther> &other) noexcept
		{
			str = other.str;
			return *this;
		}

		constexpr usize size() const noexcept
		{
			return StrLen(str);
		}

		constexpr const TChar *begin() const noexcept
		{
			return str;
		}

		constexpr const TChar *end() const noexcept
		{
			return str + StrLen(str);
		}

		constexpr const TChar *cbegin() const noexcept
		{
			return str;
		}

		constexpr const TChar *cend() const noexcept
		{
			return str + StrLen(str);
		}

		constexpr TChar operator[](usize index) const noexcept
		{
			return str[index];
		}

		constexpr operator const TChar *() const noexcept
		{
			return str;
		}

		constexpr operator std::basic_string<std::remove_const_t<TChar>>() const noexcept
		{
			return std::basic_string<std::remove_const_t<TChar>>(str, size());
		}

		template <typename TOther> requires std::is_same_v<std::remove_const_t<TChar>, std::remove_const_t<TOther>>
		friend constexpr bool operator==(const StringLiteral &lhs, const StringLiteral<TOther> &rhs) noexcept
		{
			return lhs.Get() == rhs.Get();
		}

		template <typename TOther> requires std::is_same_v<std::remove_const_t<TChar>, std::remove_const_t<TOther>>
		friend constexpr bool operator!=(const StringLiteral &lhs, const StringLiteral<TOther> &rhs) noexcept
		{
			return lhs.Get() != rhs.Get();
		}
	};

	template <typename TChar, usize Length>
	struct StringLiteral<TChar, Length> final
	{
	private:
		TChar str[Length];

	public:
		constexpr StringLiteral(const TChar(&str)[Length]) noexcept
		{
			std::copy(str, str + Length, this->str);
		}

		constexpr usize size() const noexcept
		{
			return Length;
		}

		constexpr TChar operator[](usize index) const noexcept
		{
			return str[index];
		}
	};

	template <typename TEnum>
	struct Enum final
	{
	public:
		static_assert(std::is_enum_v<TEnum>, "TEnum must be an enum type!");

		using value_type = TEnum;
		using underlying_type = std::underlying_type_t<TEnum>;

		TEnum value;

		constexpr operator TEnum() const noexcept
		{
			return value;
		}

		constexpr explicit operator std::underlying_type_t<TEnum>() const noexcept
		{
			return static_cast<std::underlying_type_t<TEnum>>(value);
		}

		constexpr friend Enum<TEnum> operator |(Enum<TEnum> lhs, Enum<TEnum> rhs) noexcept
		{
			return Enum<TEnum>{ lhs.value | rhs.value };
		}

		constexpr friend Enum<TEnum> operator &(Enum<TEnum> lhs, Enum<TEnum> rhs) noexcept
		{
			return Enum<TEnum>{ lhs.value & rhs.value };
		}

		constexpr friend Enum<TEnum> operator ^(Enum<TEnum> lhs, Enum<TEnum> rhs) noexcept
		{
			return Enum<TEnum>{ lhs.value ^ rhs.value };
		}

		constexpr friend bool operator==(Enum<TEnum> lhs, Enum<TEnum> rhs) noexcept
		{
			return lhs.value == rhs.value;
		}

		constexpr friend bool operator!=(Enum<TEnum> lhs, Enum<TEnum> rhs) noexcept
		{
			return lhs.value != rhs.value;
		}

		constexpr friend bool operator<(Enum<TEnum> lhs, Enum<TEnum> rhs) noexcept
		{
			return lhs.value < rhs.value;
		}

		constexpr friend bool operator>(Enum<TEnum> lhs, Enum<TEnum> rhs) noexcept
		{
			return lhs.value > rhs.value;
		}

		constexpr friend bool operator<=(Enum<TEnum> lhs, Enum<TEnum> rhs) noexcept
		{
			return lhs.value <= rhs.value;
		}

		constexpr friend bool operator>=(Enum<TEnum> lhs, Enum<TEnum> rhs) noexcept
		{
			return lhs.value >= rhs.value;
		}
	};

	template <typename T>
	struct Range final
	{
	private:

	public:
	};

	template <auto Constant>
	constexpr Const<Constant> MakeConst() noexcept
	{
		return Const<Constant>();
	}

	template <typename TConstant, TConstant Constant>
	constexpr Const<Constant> MakeConst() noexcept
	{
		return Const<Constant>();
	}

	template <typename T, typename TConstraint>
	consteval Literal<T> MakeLiteral(T value, TConstraint constraint) noexcept
	{
		return constraint(value) ? Literal<T>(value) : throw std::exception("Constraint not satisfied!");
	}

	template <typename T>
	struct Nullable final
	{
	private:
		struct InPlaceTag {};

		union 
		{
		public:
			std::remove_const_t<T> value;
			//byte defaultInitBlocker;
		};

		bool hasValue;

		template <typename ...TParams>
		constexpr Nullable(InPlaceTag, TParams &&...params) noexcept(noexcept(T(std::forward<TParams>(params)...))) :
			value(T(std::forward<TParams>(params)...)), hasValue(true) {}

	public:
		constexpr Nullable() noexcept : hasValue(false) {} // TODO: Default constructed value support?

#pragma warning(disable: 26495)
		constexpr Nullable(decltype(std::nullopt)) noexcept : hasValue(false) {}
		constexpr Nullable(Null) noexcept : hasValue(false) {}
#pragma warning(restore: 26495)

		constexpr Nullable(const T &value) noexcept(noexcept(T(value))) : value(value), hasValue(true) {}
		constexpr Nullable(std::remove_const_t<T> &&value) noexcept(noexcept(T(std::move(value)))) : value(std::move(value)), hasValue(true) {}

		constexpr Nullable(const Nullable &other) noexcept(noexcept(T(value))) : hasValue(other.hasValue)
		{
			if (other.hasValue)
			{
				new (std::addressof(value)) T(other.value);
			}
		}

		constexpr Nullable(Nullable &&other) noexcept(noexcept(T(std::move(value)))) : hasValue(other.hasValue)
		{
			if (other.hasValue)
			{
				new (std::addressof(value)) T(std::move(other.value));
			}
		}

		constexpr Nullable &operator=(const Nullable &other) noexcept(noexcept(T(value)) && noexcept(value = other.value))
		{
			if (hasValue)
			{
				if (other.hasValue)
				{
					value = other.value;
				}
				else
				{
					std::destroy_at(std::addressof(value));
				}
			}
			else if (other.hasValue)
			{
				new (std::addressof(value)) T(other.value);
			}

			hasValue = other.hasValue;
			return *this;
		}

		constexpr Nullable &operator=(Nullable &&other) noexcept(noexcept(T(value)) && noexcept(value = std::move(other.value)))
		{
			if (hasValue)
			{
				if (other.hasValue)
				{
					value = std::move(other.value);
				}
				else
				{
					std::destroy_at(std::addressof(value));
					hasValue = false;
				}
			}
			else if (other.hasValue)
			{
				new (std::addressof(value)) T(std::move(other.value));
			}

			hasValue = other.hasValue;
			return *this;
		}

		constexpr const T &Get() const &noexcept
		{
			return value;
		}

		constexpr T &Get() &noexcept
		{
			return value;
		}

		constexpr const T &&Get() const &&noexcept
		{
			return std::move(value);
		}

		constexpr T &&Get() &&noexcept
		{
			return std::move(value);
		}

		constexpr bool HasValue() const noexcept
		{
			return hasValue;
		}

		constexpr const T *operator->() const noexcept
		{
			return hasValue ? std::addressof(value) : nullptr;
		}

		constexpr T *operator->() noexcept
		{
			return hasValue ? std::addressof(value) : nullptr;
		}
		
		constexpr const T &operator*() const &noexcept
		{
			return *operator->();
		}

		constexpr T &operator*() &noexcept
		{
			return *operator->();
		}

		constexpr const T &&operator*() const &&noexcept
		{
			return std::move(*operator->());
		}

		constexpr T &&operator*() &&noexcept
		{
			return std::move(*operator->());
		}

		constexpr explicit operator bool() const noexcept
		{
			return hasValue;
		}

		template <typename ...TParams>
		static constexpr Nullable New(TParams &&...params) noexcept(noexcept(Nullable(InPlaceTag{}, std::forward<TParams>(params)...)))
		{
			return Nullable(InPlaceTag{}, std::forward<TParams>(params)...);
		}

		constexpr operator Nullable<const T>() const noexcept
		{
			return hasValue ? Nullable<const T>::New(value) : Nullable<const T>();
		}

		constexpr ~Nullable() noexcept(noexcept(std::destroy_at(std::addressof(value))))
		{
			if (hasValue)
			{
				std::destroy_at(std::addressof(value));
			}
		}
	};

	template <typename T, typename ...TParams>
	constexpr Nullable<T> MakeNullable(TParams &&...params) noexcept(noexcept(Nullable<T>::New(std::forward<TParams>(params)...)))
	{
		return Nullable<T>::New(std::forward<TParams>(params)...);
	}

	using CStrLiteral = StringLiteral<char>;

	struct Bool final // because std::vector<bool> is more like a std::vector<Bool8>...
	{
	public:
		using value_type = bool;

		bool value;

		static const Bool True;
		static const Bool False;

		constexpr Bool() noexcept = default;
		constexpr Bool(bool value) noexcept : value(value) {}

		constexpr Bool &operator=(bool value) noexcept
		{
			this->value = value;
			return *this;
		}

		constexpr operator bool() const noexcept // bool or const bool &?
		{
			return value;
		}

		constexpr operator bool &() noexcept
		{
			return value;
		}
	};

	constexpr inline Bool Bool::True = { true };
	constexpr inline Bool Bool::False = { false };

	enum struct EndPointType : bool
	{
		Inclusive = false,
		Exclusive = true,
	};

	template <typename T>
	struct Hash
	{
	public:
		usize operator()(const T &value) const
		{
			return std::hash<T>()(value);
		}
	};

	template <typename T>
	usize GetHashCode(const T &value) noexcept
	{
		return std::hash<T>()(value);
	}

	enum struct CppVersion : VersionType
	{
		Current = __cplusplus,
		Cpp98 = 199711L,
		Cpp11 = 201103L,
		Cpp14 = 201402L,
		Cpp17 = 201703L,
		Cpp20 = 202002L,
	};

	template <const CppVersion Version>
	constexpr bool IsSupported() noexcept
	{
		return __cplusplus >= static_cast<std::underlying_type_t<CppVersion>>(Version);
	}

	constexpr bool IsSupported(CppVersion version) noexcept
	{
		return __cplusplus >= static_cast<std::underlying_type_t<CppVersion>>(version);
	}

	template <typename TEnum> requires (std::is_enum_v<TEnum>)
	std::istream &operator>>(std::istream &stream, TEnum &value)
	{
		std::underlying_type_t<TEnum> underlyingValue;
		stream >> underlyingValue;
		value = static_cast<TEnum>(underlyingValue);
		return stream;
	}

	template <typename TEnum> requires (std::is_enum_v<TEnum>)
	std::ostream &operator<<(std::ostream &stream, const TEnum &value)
	{
		return stream << static_cast<std::underlying_type_t<TEnum>>(value);
	}

	template <typename TEnum> requires (std::is_enum_v<TEnum>)
	constexpr std::underlying_type_t<TEnum> ToUnderlying(TEnum value) noexcept
	{
		return static_cast<std::underlying_type_t<TEnum>>(value);
	}
	template <typename TEnum> requires (std::is_enum_v<TEnum>)
	constexpr TEnum operator~(TEnum value) noexcept
	{
		return static_cast<TEnum>(~ToUnderlying(value));
	}

	template <typename TEnum> requires (std::is_enum_v<TEnum>)
	constexpr TEnum operator|(TEnum lhs, TEnum rhs) noexcept
	{
		return static_cast<TEnum>(ToUnderlying(lhs) | ToUnderlying(rhs));
	}

	template <typename TEnum> requires (std::is_enum_v<TEnum>)
	constexpr TEnum operator&(TEnum lhs, TEnum rhs) noexcept
	{
		return static_cast<TEnum>(ToUnderlying(lhs) & ToUnderlying(rhs));
	}

	template <typename TEnum> requires (std::is_enum_v<TEnum>)
	constexpr TEnum operator^(TEnum lhs, TEnum rhs) noexcept
	{
		return static_cast<TEnum>(ToUnderlying(lhs) ^ ToUnderlying(rhs));
	}

	template <typename TEnum> requires (std::is_enum_v<TEnum>)
	constexpr TEnum &operator|=(TEnum &lhs, TEnum rhs) noexcept
	{
		lhs = lhs | rhs;
		return lhs;
	}

	template <typename TEnum> requires (std::is_enum_v<TEnum>)
	constexpr TEnum &operator&=(TEnum &lhs, TEnum rhs) noexcept
	{
		lhs = lhs & rhs;
		return lhs;
	}

	template <typename TEnum> requires (std::is_enum_v<TEnum>)
	constexpr TEnum &operator^=(TEnum &lhs, TEnum rhs) noexcept
	{
		lhs = lhs ^ rhs;
		return lhs;
	}

	template <typename TEnum> requires (std::is_enum_v<TEnum>)
	constexpr void SetFlag(TEnum &value, TEnum flag) noexcept
	{
		value |= flag;
	}

	template <typename TEnum> requires (std::is_enum_v<TEnum>)
	constexpr void ClearFlag(TEnum &value, TEnum flag) noexcept
	{
		value &= ~flag;
	}

	template <typename TEnum> requires (std::is_enum_v<TEnum>)
	constexpr bool HasFlag(TEnum value, TEnum flag) noexcept
	{
		return (value & flag) == flag;
	}

	template <typename TInt>
	constexpr TInt Mod(TInt lhs, TInt rhs) noexcept
	{
		TInt temp = lhs % rhs;
		return temp < TInt() ? temp + rhs : temp;
	}

	template <typename TEnum> requires (std::is_enum_v<TEnum>)
	struct EnumNames
	{
	public:
		static const std::unordered_map<TEnum, const char *> names;
	};

	template <typename TEnum>
	const char *GetEnumName(TEnum value)
	{
		return EnumNames<TEnum>::names[value];
	}

	template <typename TOut, typename TIn>
	constexpr TOut As(TIn &&value) noexcept(noexcept(static_cast<TOut>(value)))
	{
		return static_cast<TOut>(std::forward<TIn>(value));
	}
}

namespace Lib::Data
{
	template <typename T>
	struct Value2 final
	{
	public:
		using value_type = T;

		template <typename TOther>
		using rebind = Value2<TOther>;

		T x;
		T y;

		constexpr usize size() const noexcept
		{
			return 2;
		}

		constexpr const T *cbegin() const noexcept
		{
			return std::addressof(x);
		}

		constexpr const T *cend() const noexcept
		{
			return cbegin() + size();
		}
		
		constexpr T *begin() noexcept
		{
			return std::addressof(x);
		}

		constexpr T *end() noexcept
		{
			return begin() + size();
		}

		constexpr const T &operator[](usize index) const noexcept
		{
			return std::addressof(x)[index];
		}

		constexpr T &operator[](usize index) noexcept
		{
			return std::addressof(x)[index];
		}

		constexpr operator bool() const noexcept requires std::same_as<std::remove_cvref_t<T>, bool>
		{
			return x && y;
		}

		constexpr operator Value2<const T>() const noexcept requires (!std::is_const_v<T>)
		{
			return { x, y };
		}

		constexpr operator Value2<std::remove_const_t<T>>() const noexcept requires std::is_const_v<T>
		{
			return { x, y };
		}

		static constexpr Value2 Scale(const Value2 &lhs, const Value2 &rhs) noexcept
		{
			return { lhs.x * rhs.x, lhs.y * rhs.y };
		}

		constexpr friend Value2 operator+(const Value2 &value) noexcept
		{
			return value;
		}

		constexpr friend Value2 operator-(const Value2 &value) noexcept
		{
			return { -value.x, -value.y };
		}

		constexpr friend Value2<bool> operator==(const Value2 &lhs, const Value2 &rhs) noexcept
		{
			return { lhs.x == rhs.x, lhs.y == rhs.y };
		}

		constexpr friend Value2<bool> operator!=(const Value2 &lhs, const Value2 &rhs) noexcept
		{
			return { lhs.x != rhs.x, lhs.y != rhs.y };
		}

		constexpr friend Value2<bool> operator<(const Value2 &lhs, const Value2 &rhs) noexcept requires std::totally_ordered<T>
		{
			return { lhs.x < rhs.x, lhs.y < rhs.y };
		}

		constexpr friend Value2<bool> operator>(const Value2 &lhs, const Value2 &rhs) noexcept requires std::totally_ordered<T>
		{
			return { lhs.x > rhs.x, lhs.y > rhs.y };
		}

		constexpr friend Value2<bool> operator<=(const Value2 &lhs, const Value2 &rhs) noexcept requires std::totally_ordered<T>
		{
			return { lhs.x <= rhs.x, lhs.y <= rhs.y };
		}

		constexpr friend Value2<bool> operator>=(const Value2 &lhs, const Value2 &rhs) noexcept requires std::totally_ordered<T>
		{
			return { lhs.x >= rhs.x, lhs.y >= rhs.y };
		}

		constexpr friend Value2 operator+(const Value2 &lhs, const Value2 &rhs) noexcept
		{
			return { lhs.x + rhs.x, lhs.y + rhs.y };
		}

		constexpr friend Value2 operator-(const Value2 &lhs, const Value2 &rhs) noexcept
		{
			return { lhs.x - rhs.x, lhs.y - rhs.y };
		}

		constexpr friend Value2 operator*(const Value2 &value, T scale) noexcept
		{
			return { value.x * scale, value.y * scale };
		}

		constexpr friend Value2 operator*(T scale, const Value2 &value) noexcept
		{
			return { value.x * scale, value.y * scale };
		}

		constexpr friend Value2 operator/(const Value2 &value, T scale) noexcept
		{
			return { value.x / scale, value.y / scale };
		}

		constexpr friend Value2 &operator+=(Value2 &value, const Value2 &other) noexcept
		{
			value.x += other.x;
			value.y += other.y;
			return value;
		}

		constexpr friend Value2 &operator-=(Value2 &value, const Value2 &other) noexcept
		{
			value.x -= other.x;
			value.y -= other.y;
			return value;
		}

		friend std::istream &operator>>(std::istream &stream, Value2 &value)
		{
			return stream >> value.x >> value.y;
		}

		friend std::ostream &operator<<(std::ostream &stream, const Value2 &value)
		{
			return stream << '(' << value.x << ", " << value.y << ')';
		}
	};

	// Cartesian coordinates and SDL coordinates are a bit different: the Y axis points down in SDL instead.
	template <typename TNumeric>
	constexpr Value2<TNumeric> ReversedY(const Value2<TNumeric> &value) noexcept
	{
		return { value.x, -value.y };
	}

	using short2 = Value2<short>;
	using int2 = Value2<int>;
	using long2 = Value2<long>;
	using llong2 = Value2<long long>;
	using usize2 = Value2<usize>;
	using float2 = Value2<float>;
	using double2 = Value2<double>;
	using bool2 = Value2<bool>;

	struct RectSize final
	{
	public:
		int width;
		int height;

		constexpr int GetArea() const noexcept
		{
			return width * height;
		}

		constexpr operator int2() const noexcept
		{
			return { width, height };
		}

		static constexpr RectSize Scale(RectSize value, int2 scale) noexcept
		{
			return { value.width * scale.x, value.height * scale.y };
		}

		constexpr friend RectSize operator*(const RectSize &value, int scale) noexcept
		{
			return { value.width * scale, value.height * scale };
		}

		constexpr friend RectSize operator*(int scale, const RectSize &value) noexcept
		{
			return { value.width * scale, value.height * scale };
		}

		constexpr friend RectSize operator/(const RectSize &value, int scale) noexcept
		{
			return { value.width / scale, value.height / scale };
		}

		constexpr friend RectSize operator*(const RectSize &value, double scale) noexcept
		{
			return { static_cast<int>(value.width * scale), static_cast<int>(value.height * scale) };
		}

		constexpr friend RectSize operator*(double scale, const RectSize &value) noexcept
		{
			return { static_cast<int>(value.width * scale), static_cast<int>(value.height * scale) };
		}

		constexpr friend RectSize operator/(const RectSize &value, double scale) noexcept
		{
			return { static_cast<int>(value.width / scale), static_cast<int>(value.height / scale) };
		}

		constexpr friend RectSize &operator*=(RectSize &value, int scale) noexcept
		{
			value.width *= scale;
			value.height *= scale;
			return value;
		}

		constexpr friend RectSize &operator/=(RectSize &value, int scale) noexcept
		{
			value.width /= scale;
			value.height /= scale;
			return value;
		}

		constexpr friend RectSize &operator*=(RectSize &value, double scale) noexcept
		{
			value.width = static_cast<int>(value.width * scale);
			value.height = static_cast<int>(value.height * scale);
			return value;
		}

		constexpr friend RectSize &operator/=(RectSize &value, double scale) noexcept
		{
			value.width = static_cast<int>(value.width / scale);
			value.height = static_cast<int>(value.height / scale);
			return value;
		}

		constexpr friend bool operator==(const RectSize &lhs, const RectSize &rhs) noexcept
		{
			return lhs.width == rhs.width && lhs.height == rhs.height;
		}

		constexpr friend bool operator!=(const RectSize &lhs, const RectSize &rhs) noexcept
		{
			return lhs.width != rhs.width || lhs.height != rhs.height;
		}

		friend std::istream &operator>>(std::istream &stream, RectSize &value)
		{
			return stream >> value.width >> value.height;
		}

		friend std::ostream &operator<<(std::ostream &stream, const RectSize &value)
		{
			return stream << "Width: " << value.width << ", Height: " << value.height;
		}
	};

	constexpr int2 Scale(int2 value, RectSize scale) noexcept
	{
		return { value.x * scale.width, value.y * scale.height };
	}
}

template <typename T>
struct std::hash<Lib::Data::Value2<T>>
{
public:
	constexpr usize operator()(const Lib::Data::Value2<T> &value) const noexcept
	{
		if constexpr (sizeof(Lib::Data::Value2<T>) == sizeof(usize))
		{
			return std::bit_cast<usize>(value);
		}
		else // https://stackoverflow.com/a/2595226 TODO: Better implementation?
		{
			std::hash<T> hasher = {};
			usize result = hasher(value.x);
			result ^= hasher(value.y) + static_cast<usize>(0x9e3779b9) + (result << 6) + (result >> 2);
			return result;
		}
	}
};

namespace Lib::Sdl
{
	// Pressed as long as it is held down!
	constexpr bool Held(const SDL_Event &event, SDL_KeyCode keyCode) noexcept
	{
		return event.type == SDL_EventType::SDL_KEYDOWN && event.key.keysym.sym == static_cast<SDL_Keycode>(keyCode);
	}

	// This action indeed lasts only for one frame!
	constexpr bool Released(const SDL_Event &event, SDL_KeyCode keyCode) noexcept
	{
		return event.type == SDL_EventType::SDL_KEYUP && event.key.keysym.sym == static_cast<SDL_Keycode>(keyCode);
	}

	constexpr bool Triggered(const SDL_Event &event, SDL_KeyCode keyCode) noexcept
	{
		return event.key.keysym.sym == static_cast<SDL_Keycode>(keyCode);
	}
}

#endif // !LIB_DEFINED
