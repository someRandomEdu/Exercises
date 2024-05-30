#ifndef RANDOMIZERS_DEFINED
#define RANDOMIZERS_DEFINED

#pragma once

#include <random>

#include "Lib.hpp"

namespace Lib::Randomizers
{
	class Randomizer final
	{
	private:
		std::mt19937_64 rng;
		std::uniform_int_distribution<usize> distribution;

	public:
		Randomizer() : rng(std::mt19937_64(std::random_device()())), distribution(std::uniform_int_distribution<usize>()) {}

		Randomizer(usize inclusiveMin, usize inclusiveMax) : rng(std::mt19937_64(std::random_device()())),
			distribution(std::uniform_int_distribution<usize>(inclusiveMin, inclusiveMax)) {}

		usize operator()()
		{
			return distribution(rng);
		}
	};

	template <typename T>
	class BagRandomizer final
	{
	private:
		std::vector<T> bag;
		usize index;
		std::mt19937_64 randomizer;

		void Shuffle()
		{
			std::shuffle(std::begin(bag), std::end(bag), randomizer);
		}

	public:
		using value_type = T;

		BagRandomizer(const T values[], usize length) : bag(std::vector<T>(values, values + length)), index(0), 
			randomizer(std::mt19937_64(std::random_device()()))
		{
			Shuffle();
		}

		BagRandomizer(const std::vector<T> &bag) : bag(std::vector<T>(bag)), index(0), randomizer(std::mt19937_64((std::random_device()())))
		{
			Shuffle();
		}

		BagRandomizer(std::vector<T> &&bag) : bag(std::vector<T>(std::move(bag))), index(0), randomizer(std::mt19937_64((std::random_device()())))
		{
			Shuffle();
		}

		BagRandomizer(std::initializer_list<T> bag) : bag(std::vector<T>(bag)), index(0), randomizer(std::mt19937_64(std::random_device()())) {}

		usize size() const noexcept
		{
			return bag.size();
		}

		usize GetIndex() const noexcept
		{
			return index;
		}

		std::vector<T> GetBag() const noexcept
		{
			return bag;
		}

		void Reset()
		{
			index = 0;
			Shuffle();
		}

		T GetNext()
		{
			T result = bag[index];
			++index;

			if (index >= bag.size())
			{
				Shuffle();
				index = 0;
			}

			return result;
		}
	};
}

#endif // RANDOMIZERS_DEFINED