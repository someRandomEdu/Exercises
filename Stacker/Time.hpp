#ifndef TIME_DEFINED
#define TIME_DEFINED

#pragma once

#include <ctime>
#include <chrono>

#include "Lib.hpp"

namespace Lib
{
	using DeltaTime = double;
}

namespace Lib::Time
{
	using namespace Lib;

	struct StdTimer final
	{
	private:
		std::chrono::steady_clock::time_point previous;

	public:
		StdTimer() noexcept : previous(std::chrono::steady_clock::now()) {}

		void Start() noexcept
		{
			previous = std::chrono::steady_clock::now();
		}

		DeltaTime GetDeltaTime() noexcept
		{
			std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();
			DeltaTime result = std::chrono::duration_cast<std::chrono::duration<DeltaTime>>(current - previous).count();
			previous = current;
			return result;
		}
	};

	struct Timer final
	{
	private:
		DeltaTime currentTime;
		DeltaTime maxTime;

	public:
		using value_type = DeltaTime;

		constexpr Timer() noexcept = default;
		constexpr Timer(DeltaTime maxTime) noexcept : currentTime(maxTime), maxTime(maxTime) {}
		constexpr Timer(DeltaTime maxTime, DeltaTime initialTime) noexcept : currentTime(initialTime), maxTime(maxTime) {}

		constexpr DeltaTime GetMaxTime() const noexcept
		{
			return maxTime;
		}

		constexpr DeltaTime GetTime() const noexcept
		{
			return currentTime;
		}

		constexpr int Update(DeltaTime deltaTime) noexcept
		{
			DeltaTime addedTime = currentTime + deltaTime;
			DeltaTime temp = addedTime / maxTime;

			if (temp > static_cast<DeltaTime>(std::numeric_limits<int>::max()))
			{
				return std::numeric_limits<int>::max();
			}
			else
			{
				int result = static_cast<int>(temp);
				currentTime = addedTime - result * maxTime;
				return result;
			}
		}

		constexpr void Increment(DeltaTime deltaTime) noexcept
		{
			currentTime += deltaTime;
		}

		constexpr void SetTime(DeltaTime time) noexcept
		{
			maxTime = time;
			currentTime = time;
		}

		constexpr void SetToMax() noexcept
		{
			currentTime = maxTime;
		}

		constexpr void Start() noexcept
		{
			currentTime = maxTime;
		}

		constexpr void Reset() noexcept
		{
			currentTime = DeltaTime();
		}
	};

	struct DasArrTimer final
	{
	private:
		DeltaTime dasTime;
		DeltaTime arrTime;
		DeltaTime time;
		int dasTicks;

		static constexpr int arrTickThreshold = 2;

	public:
		constexpr DasArrTimer() noexcept = default;

		constexpr DasArrTimer(DeltaTime dasTime, DeltaTime arrTime) noexcept : dasTime(dasTime), arrTime(arrTime), 
			time(dasTime), dasTicks(0) {}

		constexpr DeltaTime GetDas() const noexcept
		{
			return dasTime;
		}

		constexpr DeltaTime GetArr() const noexcept
		{
			return arrTime;
		}

		constexpr DeltaTime GetTime() const noexcept
		{
			return time;
		}

		constexpr bool IsChargingArr() const noexcept
		{
			return dasTicks >= arrTickThreshold;
		}

		constexpr void SetToMax() noexcept
		{
			time = IsChargingArr() ? arrTime : dasTime;
		}

		constexpr void SetHandling(DeltaTime dasTime, DeltaTime arrTime) noexcept
		{
			this->dasTime = dasTime;
			this->arrTime = arrTime;
		}

		constexpr void Reset() noexcept
		{
			time = dasTime;
			dasTicks = 0;
		}

		constexpr int Update(DeltaTime deltaTime) noexcept
		{
			DeltaTime addedTime = time + deltaTime;

			if (IsChargingArr())
			{
				DeltaTime steps = addedTime / arrTime;

				if (steps > static_cast<DeltaTime>(std::numeric_limits<int>::max()))
				{
					return std::numeric_limits<int>::max();
				}
				else
				{
					int result = static_cast<int>(steps);
					time = addedTime - result * arrTime;
					return result;
				}
			}
			else
			{
				DeltaTime steps = addedTime / dasTime;

				if (steps > static_cast<DeltaTime>(std::numeric_limits<int>::max()))
				{
					return std::numeric_limits<int>::max();
				}
				else
				{
					int result = static_cast<int>(steps);
					time = addedTime - result * dasTime;

					if (result > 0 && dasTicks < arrTickThreshold)
					{
						++dasTicks;
					}

					return result;
				}
			}
		}
	};
}

#endif // !TIME_DEFINED
