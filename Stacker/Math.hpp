#ifndef MATH_DEFINED
#define MATH_DEFINED

#pragma once

#define _USE_MATH_DEFINES

#include <cmath>
#include <limits>
#include <algorithm>

using usize = std::size_t;

namespace Lib::Math
{
	constexpr double pi = M_PI;
	constexpr double euler = M_E;

	template <typename TNumeric>
	constexpr TNumeric min = std::numeric_limits<TNumeric>::min();

	template <typename TNumeric>
	constexpr TNumeric max = std::numeric_limits<TNumeric>::max();

	template <typename TNumber>
	constexpr TNumber epsilon = []() constexpr noexcept -> TNumber
	{
		if constexpr (std::is_integral_v<TNumber>)
		{
			return static_cast<TNumber>(1);
		}
		else
		{
			return std::numeric_limits<TNumber>::epsilon();
		}
	}();

	template <typename TFloat>
	constexpr TFloat positiveInfinity = std::numeric_limits<TFloat>::infinity();

	template <typename TFloat>
	constexpr TFloat negativeInfinity = -std::numeric_limits<TFloat>::infinity();

	template <typename TNumeric>
	constexpr TNumeric Max() noexcept
	{
		return std::numeric_limits<TNumeric>::max();
	}

	template <typename TNumeric>
	constexpr TNumeric Min() noexcept
	{
		return std::numeric_limits<TNumeric>::min();
	}

	template <typename TCollection>
	auto Max(const TCollection &collection) noexcept -> decltype(*std::cbegin(collection))
	{
		auto result = *std::begin(collection);

		for (auto value : collection)
		{
			if (result < value)
			{
				result = value;
			}
		}

		return result;
	}

	template <typename TCollection>
	auto Min(const TCollection &collection) noexcept -> decltype(*std::cbegin(collection))
	{
		auto result = *std::begin(collection);

		for (auto value : collection)
		{
			if (result > value)
			{
				result = value;
			}
		}

		return result;
	}

	template <typename TInt>
	constexpr TInt Gcd(std::remove_const_t<TInt> lhs, std::remove_const_t<TInt> rhs) noexcept
	{
		while (rhs != 0)
		{
			usize temp = rhs;
			rhs = lhs % rhs;
			lhs = temp;
		}

		return lhs;
	}

	template <typename TInt>
	constexpr TInt Lcm(std::remove_const_t<TInt> lhs, std::remove_const_t<TInt> rhs) noexcept
	{
		return lhs / Gcd(lhs, rhs) * rhs;
	}
}

#endif // !MATH_DEFINED
