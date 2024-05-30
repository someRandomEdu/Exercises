#ifndef STACKER_DEFINED
#define STACKER_DEFINED

#pragma once

#include <vector>
#include <deque>
#include <queue>
#include <unordered_map>
#include <utility>
#include <bit>
#include <memory>

#include "Lib.hpp"
#include "Time.hpp"
#include "Memory.hpp"
#include "Collections.hpp"
#include "Randomizers.hpp"
#include "SdlLib.hpp"

using namespace Lib;
using namespace Lib::Data;
using namespace Lib::Collections;
using namespace Lib::Memory;
using namespace Lib::Time;
using namespace Lib::Randomizers;
using namespace Lib::Sdl;
using namespace Lib::Sdl::Text;

namespace Stacker
{
	constexpr int orientationCount = 4; // Do I have a consistent naming convention for those constant values?

	constexpr int2 Vec(int x, int y) noexcept
	{
		return { x, y };
	}

	enum struct TetrominoType : char
	{
		None = char(), // Should be treated as TetrominoType::A. This exists to prevent undefined behaviors I guess...
		A = 'A', // A, for air, or absent.
		I = 'I',
		J = 'J',
		L = 'L',
		O = 'O',
		S = 'S',
		T = 'T',
		Z = 'Z',
	};

	constexpr bool IsValidTetrominoType(TetrominoType tetrominoType) noexcept
	{
		return tetrominoType == TetrominoType::I || tetrominoType == TetrominoType::J || tetrominoType == TetrominoType::L ||
			tetrominoType == TetrominoType::O || tetrominoType == TetrominoType::S || tetrominoType == TetrominoType::T ||
			tetrominoType == TetrominoType::Z;
	}

	enum struct Orientation : int
	{
		North = 0,
		East = 1,
		South = 2,
		West = 3,
	};

	enum struct RotateDirection : int
	{
		Counterclockwise180 = -2,
		Counterclockwise = -1,
		None = 0,
		Clockwise = 1,
		Clockwise180 = 2,
	};

	constexpr Orientation RotateOrientation(Orientation orientation, RotateDirection rotateDirection) noexcept
	{
		return static_cast<Orientation>(Mod(((ToUnderlying(orientation) + ToUnderlying(rotateDirection))), orientationCount));
	}

	struct RotationChange final
	{		
	public:
		Orientation orientation;
		RotateDirection rotateDirection;

		constexpr friend bool operator==(const RotationChange &lhs, const RotationChange &rhs) noexcept
		{
			return lhs.orientation == rhs.orientation && lhs.rotateDirection == rhs.rotateDirection;
		}

		constexpr friend bool operator!=(const RotationChange &lhs, const RotationChange &rhs) noexcept
		{
			return lhs.orientation != rhs.orientation || lhs.rotateDirection != rhs.rotateDirection;
		}
	};

	struct TetrominoState final
	{
	public:
		int2 positions[4];

		constexpr const int2 *cbegin() const noexcept
		{
			return std::cbegin(positions);
		}

		constexpr const int2 *cend() const noexcept
		{
			return std::cend(positions);
		}

		constexpr const int2 *begin() const noexcept
		{
			return std::cbegin(positions);
		}
		
		constexpr int2 *begin() noexcept
		{
			return std::begin(positions);
		}

		constexpr const int2 *end() const noexcept
		{
			return std::cend(positions);
		}

		constexpr int2 *end() noexcept
		{
			return std::end(positions);
		}

		constexpr bool Contains(int2 position) const noexcept
		{
			for (int2 pos : positions)
			{
				if (pos == position)
				{
					return true;
				}
			}

			return false;
		}

		constexpr int2 operator[](usize index) const noexcept
		{
			return positions[index];
		}

		constexpr int2 &operator[](usize index) noexcept
		{
			return positions[index];
		}

		constexpr friend TetrominoState operator+(const TetrominoState &state, int2 offset) noexcept
		{
			TetrominoState result = state;

			for (int2 &minoPos : result)
			{
				minoPos += offset;
			}

			return result;
		}

		constexpr friend TetrominoState operator-(const TetrominoState &state, int2 offset) noexcept
		{
			TetrominoState result = state;

			for (int2 &minoPos : result)
			{
				minoPos -= offset;
			}

			return result;
		}

		/// @brief Returns the offset of two states.
		constexpr friend int2 operator-(const TetrominoState &lhs, const TetrominoState &rhs) noexcept
		{
			return lhs.positions[0] - rhs.positions[0];
		}

		constexpr friend TetrominoState &operator+=(TetrominoState &state, int2 offset) noexcept
		{
			for (int2 &minoPos : state)
			{
				minoPos += offset;
			}

			return state;
		}

		constexpr friend TetrominoState &operator-=(TetrominoState &state, int2 offset) noexcept
		{
			for (int2 &minoPos : state)
			{
				minoPos -= offset;
			}

			return state;
		}

		friend std::istream &operator>>(std::istream &stream, TetrominoState &value)
		{
			for (int2 &minoPos : value)
			{
				stream >> minoPos;
			}

			return stream;
		}

		friend std::ostream &operator<<(std::ostream &stream, const TetrominoState &value)
		{
			for (int2 minoPos : value)
			{
				stream << minoPos << ' ';
			}

			return stream;
		}
	};

	enum struct SpinType : char
	{
		None = 0,
		Spin = 1,
		MiniSpin = 2,
	};

	struct LineClearData final
	{
	public:
		int combo;
		int b2b;
		int linesCleared;
		bool isAllClear;
		bool longB2bStreakBroken;
		SpinType spinType;
		TetrominoType tetrominoType;

		static constexpr int longB2bStreakThreshold = 2;

		constexpr bool IsValid() const noexcept
		{
			return IsValidTetrominoType(tetrominoType);
		}

		constexpr bool IsB2bClear() const noexcept
		{
			return spinType != SpinType::None || linesCleared >= 4;
		}

		SDL_Color GetColor() const noexcept;

		constexpr SDL_Color GetB2bColor() const noexcept
		{
			return longB2bStreakBroken ? SDL_Color { 206, 82, 90, 255 } : SDL_Color { 206, 197, 82, 255 };
		}

		constexpr string GetComboText() const noexcept
		{
			if (combo > 0)
			{
				return std::to_string(combo) + " COMBO";
			}
			else
			{
				return string();
			}
		}

		constexpr string GetB2bText() const noexcept
		{
			if (b2b > 0)
			{
				return "B2B x" + std::to_string(b2b);
			}
			else if (longB2bStreakBroken)
			{
				return string("B2B x0");
			}
			else
			{
				return string();
			}
		}
		
		constexpr string GetLineClearText() const noexcept
		{
			if (linesCleared > 0 && linesCleared < 5)
			{
				constexpr const char *texts[4] = { "SINGLE", "DOUBLE", "TRIPLE", "QUAD" };
				return string(texts[linesCleared - 1]);
			}
			else if (linesCleared >= 5)
			{
				return string("QUAD+");
			}
			else
			{
				return string();
			}
		}

		constexpr string GetAllClearText() const noexcept
		{
			return isAllClear ? string("ALL CLEAR") : string();
		}

		constexpr string GetSpinText() const noexcept
		{
			if (IsValidTetrominoType(tetrominoType) && spinType != SpinType::None)
			{
				return spinType == SpinType::Spin ? (string(1, static_cast<char>(tetrominoType)) + "-SPIN") : 
					("MINI " + string(1, static_cast<char>(tetrominoType)) + "-SPIN");
			}
			else
			{
				return string();
			}
		}
	
		static constexpr LineClearData New(TetrominoType tetrominoType) noexcept
		{
			return { -1, -1, 0, false, false, SpinType::None, tetrominoType };
		}

		static constexpr LineClearData New(const LineClearData &previous, TetrominoType tetrominoType) noexcept
		{
			return { previous.combo, previous.b2b, 0, false, false, SpinType::None, tetrominoType };
		}

		static constexpr LineClearData Default() noexcept
		{
			return { -1, -1, 0, false, false, SpinType::None, TetrominoType::None };
		}

		friend std::ostream &operator<<(std::ostream &stream, const LineClearData &value)
		{
			return stream << "Combo: " << value.combo << ", B2B: " << value.b2b << ", Lines cleared: " << value.linesCleared << ", Is all clear: " << 
				(value.isAllClear ? "True" : "False")  << ", Spin type: " <<
				(value.spinType == SpinType::Spin ? "Spin" : (value.spinType == SpinType::MiniSpin ? "Mini spin" : "None")) << 
				", Tetromino type: " << static_cast<char>(value.tetrominoType);
		}
	};
}

template <> struct std::hash<Stacker::RotationChange>
{
public:
	constexpr usize operator()(const Stacker::RotationChange &value) const noexcept
	{
		return std::bit_cast<usize>(value);
	}
};

namespace Stacker
{
	struct KickTable final
	{
	public:
		TetrominoState states[4];
		std::unordered_map<RotationChange, std::vector<int2>> kicks;

		const TetrominoState &GetSpawnState() const noexcept
		{
			return states[0];
		}

		const TetrominoState &GetState(Orientation orientation) const noexcept
		{
			return states[static_cast<usize>(orientation)];
		}

		TetrominoState &GetState(Orientation orientation) noexcept
		{
			return states[static_cast<usize>(orientation)];
		}

		const std::vector<int2> &GetKicks(RotationChange rotationChange) const noexcept
		{
			return kicks.at(rotationChange);
		}

		std::vector<int2> &GetKicks(RotationChange rotationChange) noexcept
		{
			return kicks.at(rotationChange);
		}
	};

	struct Tetromino final
	{
	public:
		KickTable kickTable;
		int2 spawnOffset;
		TetrominoType tetrominoType;
		SDL_Color color;

		TetrominoState GetSpawnState() const noexcept
		{
			return kickTable.GetSpawnState() + spawnOffset;
		}

		constexpr friend bool operator==(const Tetromino &lhs, const Tetromino &rhs) noexcept
		{
			return lhs.tetrominoType == rhs.tetrominoType;
		}

		constexpr friend bool operator!=(const Tetromino &lhs, const Tetromino &rhs) noexcept
		{
			return lhs.tetrominoType != rhs.tetrominoType;
		}
	};

	struct Box final
	{
	public:
		int2 mins;
		int2 maxes;

		constexpr RectSize GetSize() const noexcept
		{
			return { maxes.x - mins.x, maxes.y - mins.y };
		}

		constexpr int2 GetBottomLeft() const noexcept
		{
			return mins;
		}

		constexpr int2 GetTopRight() const noexcept
		{
			return maxes;
		}

		constexpr int2 GetBottomRight() const noexcept
		{
			return { maxes.x, mins.y };
		}

		constexpr int2 GetTopLeft() const noexcept
		{
			return { mins.x, maxes.y };
		}

		static constexpr Box New(int2 mins, int2 maxes) noexcept
		{
			return { mins, maxes };
		}

		static constexpr Box Bounding(const TetrominoState(&baseStates)[4], int2 offset) noexcept
		{
			Box result = { int2 { std::numeric_limits<int>::max(), std::numeric_limits<int>::max() }, 
				int2 { std::numeric_limits<int>::min(), std::numeric_limits<int>::min() }};

			for (const TetrominoState &state : baseStates)
			{
				for (int2 position : state)
				{
					if (position.x < result.mins.x)
					{
						result.mins.x = position.x;
					}

					if (position.y < result.mins.y)
					{
						result.mins.y = position.y;
					}

					if (position.x > result.maxes.x)
					{
						result.maxes.x = position.x;
					}

					if (position.y > result.maxes.y)
					{
						result.maxes.y = position.y;
					}
				}
			}

			result.mins += offset;
			result.maxes += offset;
			return result;
		}

		static constexpr Box Bounding(const Tetromino &tetromino, const TetrominoState &tetrominoState, Orientation orientation)
		{
			return Bounding(tetromino.kickTable.states, tetrominoState - tetromino.kickTable.GetState(orientation));
		}

		constexpr friend bool operator==(const Box &lhs, const Box &rhs) noexcept
		{
			return lhs.mins == rhs.mins && lhs.maxes == rhs.maxes;
		}

		constexpr friend bool operator!=(const Box &lhs, const Box &rhs) noexcept
		{
			return lhs.mins != rhs.mins || lhs.maxes != rhs.maxes;
		}

		constexpr friend Box operator+(const Box &lhs, int2 offset) noexcept
		{
			return { lhs.mins + offset, lhs.maxes + offset };
		}

		constexpr friend Box operator-(const Box &lhs, int2 offset) noexcept
		{
			return { lhs.mins - offset, lhs.maxes - offset };
		}

		constexpr friend Box &operator+=(Box &value, int2 offset) noexcept
		{
			value.mins += offset;
			value.maxes += offset;
			return value;
		}

		constexpr friend Box &operator-=(Box &value, int2 offset) noexcept
		{
			value.mins -= offset;
			value.maxes -= offset;
			return value;
		}

		friend std::istream &operator>>(std::istream &stream, Box &value)
		{
			return stream >> value.mins >> value.maxes;
		}

		friend std::ostream &operator<<(std::ostream &stream, const Box &value)
		{
			return stream << "Mins: " << value.mins << ", Maxes: " << value.maxes;
		}
	};

	struct HoldQueue final // TODO: How to render the hold and the next queue?
	{
	private:
		Nullable<Tetromino> heldPiece;

	public:
		usize size() const noexcept
		{
			return 1;
		}

		bool HasValue() const noexcept
		{
			return heldPiece.HasValue();
		}

		Nullable<Tetromino> Get() const noexcept
		{
			return heldPiece;
		}

		Nullable<Tetromino> PopAndPush(const Tetromino &tetromino) noexcept // To handle the first hold...
		{
			Nullable<Tetromino> result = heldPiece;
			heldPiece = MakeNullable<Tetromino>(tetromino);
			return result;
		}

		void Push(const Tetromino &tetromino) noexcept
		{
			heldPiece = MakeNullable<Tetromino>(tetromino);
		}

		void Reset() noexcept
		{
			heldPiece = Nullable<Tetromino>();
		}
	};

	struct NextQueue final
	{
	private:
		std::deque<Tetromino> tetrominoes;

	public:
		NextQueue(usize size) : tetrominoes(std::deque<Tetromino>(size)) {}

		NextQueue(usize size, BagRandomizer<Tetromino> &bagRandomizer) : tetrominoes(std::deque<Tetromino>(size)) 
		{
			for (Tetromino &tetromino : tetrominoes)
			{
				tetromino = bagRandomizer.GetNext();
			}
		}

		void Fill(BagRandomizer<Tetromino> &bagRandomizer)
		{
			for (Tetromino &tetromino : tetrominoes)
			{
				tetromino = bagRandomizer.GetNext();
			}
		}

		usize size() const noexcept
		{
			return tetrominoes.size();
		}

		Tetromino PopAndPush(const Tetromino &tetromino)
		{
			Tetromino result = tetrominoes.front();
			tetrominoes.pop_front();
			tetrominoes.push_back(tetromino);
			return result;
		}

		Tetromino PopAndPush(Tetromino &&tetromino)
		{
			Tetromino result = tetrominoes.front();
			tetrominoes.pop_front();
			tetrominoes.push_back(tetromino);
			return result;
		}

		std::deque<Tetromino>::const_iterator cbegin() const noexcept
		{
			return tetrominoes.cbegin();
		}

		std::deque<Tetromino>::const_iterator cend() const noexcept
		{
			return tetrominoes.cend();
		}

		std::deque<Tetromino>::const_iterator begin() const noexcept
		{
			return tetrominoes.begin();
		}

		std::deque<Tetromino>::const_iterator end() const noexcept
		{
			return tetrominoes.end();
		}

		std::deque<Tetromino>::iterator begin() noexcept
		{
			return tetrominoes.begin();
		}

		std::deque<Tetromino>::iterator end() noexcept
		{
			return tetrominoes.end();
		}
	};

	enum struct MoveButton : unsigned char
	{
		None = 0,
		Left = 1,
		Right = 2,
		Both = Left | Right,
	};

	enum struct SoftDropButton : unsigned char
	{
		None = 0,
		Primary = 1,
		Secondary = 2,
		Both = Primary | Secondary,
	};

	struct ControllerBinding final
	{
	public:
		SDL_KeyCode moveLeftKey;
		SDL_KeyCode moveRightKey;
		SDL_KeyCode primarySoftDropKey;
		SDL_KeyCode secondarySoftDropKey;
		SDL_KeyCode hardDropKey;
		SDL_KeyCode holdKey;
		SDL_KeyCode rotateClockwiseKey;
		SDL_KeyCode rotateCounterclockwiseKey;
		SDL_KeyCode rotateClockwise180Key;
		SDL_KeyCode rotateCounterclockwise180Key;
		SDL_KeyCode restartKey;

		static const ControllerBinding defaultBinding;
	};

	constexpr inline ControllerBinding ControllerBinding::defaultBinding =
	{
		.moveLeftKey = SDL_KeyCode::SDLK_LEFT,
		.moveRightKey = SDL_KeyCode::SDLK_RIGHT,
		.primarySoftDropKey = SDL_KeyCode::SDLK_DOWN,
		.secondarySoftDropKey = SDL_KeyCode::SDLK_UP,
		.hardDropKey = SDL_KeyCode::SDLK_SPACE,
		.holdKey = SDL_KeyCode::SDLK_LSHIFT,
		.rotateClockwiseKey = SDL_KeyCode::SDLK_x,
		.rotateCounterclockwiseKey = SDL_KeyCode::SDLK_z,
		.rotateClockwise180Key = SDL_KeyCode::SDLK_a,
		.rotateCounterclockwise180Key = SDL_KeyCode::SDLK_s,
		.restartKey = SDL_KeyCode::SDLK_r,
	};

	struct Handling final
	{
	public:
		DeltaTime das;
		DeltaTime arr;
	};

	struct HandlingData final
	{
	public:
		/*double das;
		double arr;
		double primarySoftDropDas;
		double primarySoftDropFactor;
		double secondarySoftDropDas;
		double secondarySoftDropFactor;*/
		Handling movement;
		Handling primarySoftDrop;
		Handling secondarySoftDrop;
		bool cancelDasOnDirectionChange;

		static const HandlingData defaultHandling;
	};

	constexpr inline HandlingData HandlingData::defaultHandling =
	{
		Handling { 0.15, 1.0 / 60.0 },
		Handling { 0.0, 1.0 / 80.0 },
		Handling { 0.0, 1.0 / 5.0 },
		false
	};

	std::unordered_map<RotationChange, std::vector<int2>> jlszKicks =
	{
		{ { Orientation::North, RotateDirection::Clockwise }, { {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2} }},
		{ { Orientation::East, RotateDirection::Clockwise }, { {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2} } },
		{ { Orientation::South, RotateDirection::Clockwise }, { {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2} } },
		{ { Orientation::West, RotateDirection::Clockwise }, { {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2} } },
		{ { Orientation::North, RotateDirection::Counterclockwise }, { {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2} } },
		{ { Orientation::East, RotateDirection::Counterclockwise }, { {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2} } },
		{ { Orientation::South, RotateDirection::Counterclockwise }, { {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2} } },
		{ { Orientation::West, RotateDirection::Counterclockwise }, { {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2} } },
		{ { Orientation::North, RotateDirection::Clockwise180 }, { {0, 0}, {0, 1}, {1, 1}, {-1, 1}, {1, 0}, {-1, 0} } },
		{ { Orientation::East, RotateDirection::Clockwise180 }, { {0, 0}, {1, 0}, {1, 2}, {1, 1}, {0, 2}, {0, 1} } },
		{ { Orientation::South, RotateDirection::Clockwise180 }, { {0, 0}, {0, -1}, {-1, -1}, {1, -1}, {-1, 0}, {1, 0} } },
		{ { Orientation::West, RotateDirection::Clockwise180 }, { {0, 0}, {-1, 0}, {-1, 2}, {-1, 1}, {0, 2}, {0, 1} } },
		{ { Orientation::North, RotateDirection::Counterclockwise180 }, { {0, 0}, {0, 1}, {-1, 1}, {1, 1}, {-1, 0}, {1, 0} } },
		{ { Orientation::East, RotateDirection::Counterclockwise180 }, { {0, 0}, {1, 0}, {1, 2}, {1, 1}, {0, 2}, {0, 1} } },
		{ { Orientation::South, RotateDirection::Counterclockwise180 }, { {0, 0}, {0, -1}, {1, -1}, {-1, -1}, {1, 0}, {-1, 0} } },
		{ { Orientation::West, RotateDirection::Counterclockwise180 }, { {0, 0}, {-1, 0}, {-1, 2}, {-1, 1}, {0, 2}, {0, 1} } }
	};

	std::unordered_map<RotationChange, std::vector<int2>> iKicks =
	{
		{ { Orientation::North, RotateDirection::Clockwise }, { {0, 0}, {1, 0}, {-2, 0}, {-2, -1}, {1, 2} } },
		{ { Orientation::East, RotateDirection::Clockwise }, { {0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1} } },
		{ { Orientation::South, RotateDirection::Clockwise }, { {0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2} } },
		{ { Orientation::West, RotateDirection::Clockwise }, { {0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1} } },
		{ { Orientation::North, RotateDirection::Counterclockwise }, { {0, 0}, {-1, 0}, {2, 0}, {2, -1}, {-1, 2} } },
		{ { Orientation::East, RotateDirection::Counterclockwise }, { {0, 0}, {-1, 0}, {2, 0}, {-1, -2}, {2, 1} } },
		{ { Orientation::South, RotateDirection::Counterclockwise }, { {0, 0}, {-2, 0}, {1, 0}, {-2, 1}, {1, -2} } },
		{ { Orientation::West, RotateDirection::Counterclockwise }, { {0, 0}, {1, 0}, {-2, 0}, {1, 2}, {-2, -1} } },
		{ { Orientation::North, RotateDirection::Clockwise180 }, { {0, 0}, {0, 1}, {1, 1}, {-1, 1}, {1, 0}, {-1, 0} } },
		{ { Orientation::East, RotateDirection::Clockwise180 }, { {0, 0}, {1, 0}, {1, 2}, {1, 1}, {0, 2}, {0, 1} } },
		{ { Orientation::South, RotateDirection::Clockwise180 }, { {0, 0}, {0, -1}, {-1, -1}, {1, -1}, {-1, 0}, {1, 0} } },
		{ { Orientation::West, RotateDirection::Clockwise180 }, { {0, 0}, {-1, 0}, {-1, 2}, {-1, 1}, {0, 2}, {0, 1} } },
		{ { Orientation::North, RotateDirection::Counterclockwise180 }, { {0, 0}, {0, 1}, {-1, 1}, {1, 1}, {-1, 0}, {1, 0} } },
		{ { Orientation::East, RotateDirection::Counterclockwise180 }, { {0, 0}, {1, 0}, {1, 2}, {1, 1}, {0, 2}, {0, 1} } },
		{ { Orientation::South, RotateDirection::Counterclockwise180 }, { {0, 0}, {0, -1}, {1, -1}, {-1, -1}, {1, 0}, {-1, 0} } },
		{ { Orientation::West, RotateDirection::Counterclockwise180 }, { {0, 0}, {-1, 0}, {-1, 2}, {-1, 1}, {0, 2}, {0, 1} } }
	};

	std::unordered_map<RotationChange, std::vector<int2>> oKicks = std::unordered_map<RotationChange, std::vector<int2>>
	{
		{ { Orientation::North, RotateDirection::Clockwise }, { {0, 0} } },
		{ { Orientation::East, RotateDirection::Clockwise }, { {0, 0} } },
		{ { Orientation::South, RotateDirection::Clockwise }, { {0, 0} } },
		{ { Orientation::West, RotateDirection::Clockwise }, { {0, 0} } },
		{ { Orientation::North, RotateDirection::Counterclockwise }, { {0, 0} } },
		{ { Orientation::East, RotateDirection::Counterclockwise }, { {0, 0} } },
		{ { Orientation::South, RotateDirection::Counterclockwise }, { {0, 0} } },
		{ { Orientation::West, RotateDirection::Counterclockwise }, { {0, 0} } },
		{ { Orientation::North, RotateDirection::Clockwise180 }, { {0, 0} } },
		{ { Orientation::East, RotateDirection::Clockwise180 }, { {0, 0} } },
		{ { Orientation::South, RotateDirection::Clockwise180 }, { {0, 0} } },
		{ { Orientation::West, RotateDirection::Clockwise180 }, { {0, 0} } },
		{ { Orientation::North, RotateDirection::Counterclockwise180 }, { {0, 0} } },
		{ { Orientation::East, RotateDirection::Counterclockwise180 }, { {0, 0} } },
		{ { Orientation::South, RotateDirection::Counterclockwise180 }, { {0, 0} } },
		{ { Orientation::West, RotateDirection::Counterclockwise180 }, { {0, 0} } }
	};

	std::unordered_map<RotationChange, std::vector<int2>> tKicks =
	{
		{ { Orientation::North, RotateDirection::Clockwise }, { {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2} }},
		{ { Orientation::East, RotateDirection::Clockwise }, { {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2} } },
		{ { Orientation::South, RotateDirection::Clockwise }, { {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2} } },
		{ { Orientation::West, RotateDirection::Clockwise }, { {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2} } },
		{ { Orientation::North, RotateDirection::Counterclockwise }, { {0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2} } },
		{ { Orientation::East, RotateDirection::Counterclockwise }, { {0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2} } },
		{ { Orientation::South, RotateDirection::Counterclockwise }, { {0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2} } },
		{ { Orientation::West, RotateDirection::Counterclockwise }, { {0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2} } },
		{ { Orientation::North, RotateDirection::Clockwise180 }, { {0, 0}, {0, 1}, {1, 1}, {-1, 1}, {1, 0}, {-1, 0} } },
		{ { Orientation::East, RotateDirection::Clockwise180 }, { {0, 0}, {1, 0}, {1, 2}, {1, 1}, {0, 2}, {0, 1} } },
		{ { Orientation::South, RotateDirection::Clockwise180 }, { {0, 0}, {0, -1}, {-1, -1}, {1, -1}, {-1, 0}, {1, 0} } },
		{ { Orientation::West, RotateDirection::Clockwise180 }, { {0, 0}, {-1, 0}, {-1, 2}, {-1, 1}, {0, 2}, {0, 1} } },
		{ { Orientation::North, RotateDirection::Counterclockwise180 }, { {0, 0}, {0, 1}, {-1, 1}, {1, 1}, {-1, 0}, {1, 0} } },
		{ { Orientation::East, RotateDirection::Counterclockwise180 }, { {0, 0}, {1, 0}, {1, 2}, {1, 1}, {0, 2}, {0, 1} } },
		{ { Orientation::South, RotateDirection::Counterclockwise180 }, { {0, 0}, {0, -1}, {1, -1}, {-1, -1}, {1, 0}, {-1, 0} } },
		{ { Orientation::West, RotateDirection::Counterclockwise180 }, { {0, 0}, {-1, 0}, {-1, 2}, {-1, 1}, {0, 2}, {0, 1} } }
	};

	KickTable iKickTable = KickTable
	{
		{ 
			TetrominoState{{{0, 2}, {1, 2}, {2, 2}, {3, 2}}},
			TetrominoState{{{2, 0}, {2, 1}, {2, 2}, {2, 3}}},
			TetrominoState{{{0, 1}, {1, 1}, {2, 1}, {3, 1}}},
			TetrominoState{{{1, 0}, {1, 1}, {1, 2}, {1, 3}}}
		}, 
		iKicks 
	};

	KickTable jKickTable = KickTable
	{
		{
			TetrominoState{{{0, 1}, {0, 2}, {1, 1}, {2, 1}}},
			TetrominoState{{{1, 0}, {1, 1}, {1, 2}, {2, 2}}},
			TetrominoState{{{0, 1}, {1, 1}, {2, 0}, {2, 1}}},
			TetrominoState{{{0, 0}, {1, 0}, {1, 1}, {1, 2}}}
		},
		jlszKicks
	};

	KickTable lKickTable = KickTable
	{
		{
			TetrominoState{{{0, 1}, {1, 1}, {2, 1}, {2, 2}}},
			TetrominoState{{{1, 0}, {1, 1}, {1, 2}, {2, 0}}},
			TetrominoState{{{0, 0}, {0, 1}, {1, 1}, {2, 1}}},
			TetrominoState{{{0, 2}, {1, 0}, {1, 1}, {1, 2}}}
		},
		jlszKicks
	};

	KickTable oKickTable = KickTable
	{
		{
			TetrominoState{{{1, 1}, {2, 1}, {1, 2}, {2, 2}}},
			TetrominoState{{{1, 1}, {2, 1}, {1, 2}, {2, 2}}},
			TetrominoState{{{1, 1}, {2, 1}, {1, 2}, {2, 2}}},
			TetrominoState{{{1, 1}, {2, 1}, {1, 2}, {2, 2}}}
			//TetrominoState{{{0, 0}, {1, 0}, {0, 1}, {1, 1}}}
		},
		oKicks
	};

	KickTable sKickTable = KickTable
	{
		{
			TetrominoState{{{0, 1}, {1, 1}, {1, 2}, {2, 2}}},
			TetrominoState{{{1, 1}, {1, 2}, {2, 0}, {2, 1}}},
			TetrominoState{{{0, 0}, {1, 0}, {1, 1}, {2, 1}}},
			TetrominoState{{{0, 1}, {0, 2}, {1, 0}, {1, 1}}}
		},
		jlszKicks
	};

	KickTable tKickTable = KickTable
	{
		{
			TetrominoState{{{0, 1}, {1, 1}, {1, 2}, {2, 1}}},
			TetrominoState{{{1, 0}, {1, 1}, {1, 2}, {2, 1}}},
			TetrominoState{{{0, 1}, {1, 0}, {1, 1}, {2, 1}}},
			TetrominoState{{{0, 1}, {1, 0}, {1, 1}, {1, 2}}}
		},
		tKicks
	};

	KickTable zKickTable = KickTable
	{
		{
			TetrominoState{{{0, 2}, {1, 1}, {1, 2}, {2, 1}}},
			TetrominoState{{{1, 0}, {1, 1}, {2, 1}, {2, 2}}},
			TetrominoState{{{0, 1}, {1, 0}, {1, 1}, {2, 0}}},
			TetrominoState{{{0, 0}, {0, 1}, {1, 1}, {1, 2}}}
		},
		jlszKicks
	};

	std::vector<Tetromino> tetrominoVector = 
	{
		{ iKickTable, int2 { 3, 19 }, TetrominoType::I, SDL_Color { 82, 207, 173, 255 } },
		{ jKickTable, int2 { 3, 20 }, TetrominoType::J, SDL_Color { 103, 81, 206, 255 } },
		{ lKickTable, int2 { 3, 20 }, TetrominoType::L, SDL_Color { 206, 129, 82, 255 } },
		{ oKickTable, int2 { 3, 20 }, TetrominoType::O, SDL_Color { 206, 197, 82, 255 } },
		{ sKickTable, int2 { 3, 20 }, TetrominoType::S, SDL_Color { 129, 207, 82, 255 } },
		{ tKickTable, int2 { 3, 20 }, TetrominoType::T, SDL_Color { 195, 82, 206, 255 } },
		{ zKickTable, int2 { 3, 20 }, TetrominoType::Z, SDL_Color { 206, 82, 90, 255 } }
	};

	SDL_Color LineClearData::GetColor() const noexcept
	{
		if (IsValidTetrominoType(tetrominoType))
		{
			for (const Tetromino &tetromino : tetrominoVector)
			{
				if (tetrominoType == tetromino.tetrominoType)
				{
					return tetromino.color;
				}
			}
		}

		return SDL_Color{};
	}
}

namespace Stacker
{
	class Board; 

	class Controller final // TODO: Merge with Stacker::Board?
	{
	private:
		Input moveLeftInput;
		Input moveRightInput;
		Input rotateClockwiseInput;
		Input rotateCounterclockwiseInput;
		Input rotateClockwise180Input;
		Input rotateCounterclockwise180Input;
		Input primarySoftDropInput;
		Input secondarySoftDropInput;
		Input hardDropInput;
		Input holdInput;
		Input restartInput;
		DasArrTimer movementTimer;
		Timer softDropTimer;
		Handling primarySoftDropHandling;
		Handling secondarySoftDropHandling;
		int direction;
		SoftDropButton pressedSoftDropButton;
		MoveButton pressedMoveButton;
		bool hardDropPressed;
		bool rotateClockwisePressed;
		bool rotateCounterclockwisePressed;
		bool rotateClockwise180Pressed;
		bool rotateCounterclockwise180Pressed;
		bool holdPressed;
		bool restartPressed;
			
		/*Timer primarySoftDropDasTimer;
		Timer secondarySoftDropDasTimer;*/

	public:
		Controller(ControllerBinding controllerBinding, HandlingData handlingData) noexcept : moveLeftInput(Input(controllerBinding.moveLeftKey)),
			moveRightInput(Input(controllerBinding.moveRightKey)), rotateClockwiseInput(Input(controllerBinding.rotateClockwiseKey)), 
			rotateCounterclockwiseInput(Input(controllerBinding.rotateCounterclockwiseKey)), rotateClockwise180Input(Input(controllerBinding.rotateClockwise180Key)),
		    rotateCounterclockwise180Input(Input(controllerBinding.rotateCounterclockwise180Key)), primarySoftDropInput(Input(controllerBinding.primarySoftDropKey)),
			secondarySoftDropInput(Input(controllerBinding.secondarySoftDropKey)), hardDropInput(Input(controllerBinding.hardDropKey)),
			holdInput(Input(controllerBinding.holdKey)), restartInput(controllerBinding.restartKey), movementTimer(DasArrTimer(handlingData.movement.das, handlingData.movement.arr)),
			softDropTimer(Timer(handlingData.primarySoftDrop.arr)), primarySoftDropHandling({ handlingData.primarySoftDrop.das, handlingData.primarySoftDrop.arr }),
			secondarySoftDropHandling({ handlingData.secondarySoftDrop.das, handlingData.secondarySoftDrop.arr }),
			direction(0), pressedSoftDropButton(SoftDropButton::None), pressedMoveButton(MoveButton::None), hardDropPressed(false), 
			rotateClockwisePressed(false), rotateCounterclockwisePressed(false), rotateClockwise180Pressed(false), rotateCounterclockwise180Pressed(false),
			holdPressed(false), restartPressed(false)
		{
			movementTimer.Reset();
		}

		Controller() noexcept : Controller(ControllerBinding::defaultBinding, HandlingData::defaultHandling) {}

		int GetDirection() const noexcept
		{
			return direction;
		}

		SoftDropButton GetSoftDropButton() const noexcept
		{
			return pressedSoftDropButton;
		}

		void UpdateEvent(const SDL_Event &event, Board &board) noexcept;
		void Update(DeltaTime deltaTime, Board &board) noexcept;
	};

	class Board final
	{
	private:
		BagRandomizer<Tetromino> randomizer;
		HoldQueue holdQueue;
		NextQueue nextQueue;
		Matrix<TetrominoType> boardState;
		Controller controller;
		RectSize boardSize;
		Tetromino currentTetromino;
		TetrominoState currentTetrominoPositions;
		TetrominoState currentGhostPositions;
		Orientation currentOrientation;
		Timer gravityTimer;
		bool gravityState;
		std::vector<int> clearedRows;
		LineClearData previousLineClearData; // the one that's actually used for rendering...
		LineClearData currentLineClearData;
		double textFadeTimer; // for text fading purposes
		usize score;

		Tetromino GetNext()
		{
			return nextQueue.PopAndPush(randomizer.GetNext());
		}

		TetrominoState CalculateGhostPositions() const
		{
			int2 offsets = { 0, 0 };

			if (CanMove(offsets))
			{
				while (CanMove(offsets))
				{
					--offsets.y;
				}

				return currentTetrominoPositions + offsets + int2{ 0, 1 };
			}
			else
			{
				return currentTetrominoPositions;
			}
		}

		int CalculateClearedLineCount() const
		{
			TetrominoState ghost = CalculateGhostPositions();
			int result = 0;

			for (int row = 0; row < 40; ++row)
			{
				bool cleared = true;

				for (int column = 0; column < 10; ++column)
				{
					if (!(IsValidTetrominoType(boardState[int2{ row, column }]) || ghost.Contains(int2{ column, row })))
					{
						cleared = false;
						break;
					}
				}

				if (cleared)
				{
					++result;
				}
			}

			return result;
		}

		std::vector<int> CalculateClearedLines() const
		{
			std::vector<int> result = std::vector<int>();
			TetrominoState ghost = CalculateGhostPositions();

			for (int row = 0; row < 40; ++row)
			{
				bool cleared = true;

				for (int column = 0; column < 10; ++column)
				{
					if (!(IsValidTetrominoType(boardState[int2{ row, column }]) || ghost.Contains(int2{ column, row })))
					{
						cleared = false;
						break;
					}
				}

				if (cleared)
				{
					result.push_back(row);
				}
			}

			return result;
		}

		int SoftDropOnly(int steps)
		{
			int actualSteps = 0;

			while (actualSteps != steps)
			{
				++actualSteps;

				if (IsOccupied(currentTetrominoPositions - int2{ 0, actualSteps }))
				{
					--actualSteps;
					break;
				}
			}

			if (actualSteps != 0)
			{
				currentTetrominoPositions -= int2{ 0, actualSteps };
			}

			return actualSteps;
		}

	public:
		static constexpr double startFadeThreshold = 1.0;
		static constexpr double fullyFadedThreshold = 3.0;

		Board() : randomizer(BagRandomizer<Tetromino>(tetrominoVector)), holdQueue(HoldQueue()), nextQueue(NextQueue(5)),
			boardState(Matrix<TetrominoType>(40, 10, TetrominoType::None)), controller(Controller()), boardSize(RectSize{ 10, 40 }),
			gravityTimer(Timer(1)), gravityState(true), clearedRows(std::vector<int>()), previousLineClearData(LineClearData::Default()), 
			currentLineClearData(LineClearData::Default()), textFadeTimer(0.0), score(0)
		{
			nextQueue.Fill(randomizer);
			currentTetromino = GetNext();
			currentTetrominoPositions = currentTetromino.GetSpawnState();
			currentGhostPositions = CalculateGhostPositions();
			clearedRows = CalculateClearedLines();
			currentOrientation = Orientation::North;
			gravityTimer.SetToMax();
			currentLineClearData = LineClearData::New(GetTetrominoType());
		}

		Uint8 GetTextAlpha() const
		{
			if (textFadeTimer <= startFadeThreshold)
			{
				return 255;	
			}
			else if (textFadeTimer >= fullyFadedThreshold)
			{
				return 0;
			}
			else
			{
				return static_cast<Uint8>((fullyFadedThreshold - textFadeTimer) / (fullyFadedThreshold - startFadeThreshold) * 255.0);
			}
		}

		usize GetBagIndex() const noexcept
		{
			return randomizer.GetIndex();
		}

		usize GetBagSize() const noexcept
		{
			return randomizer.size();
		}

		usize GetNextSize() const noexcept
		{
			return nextQueue.size();
		}

		RectSize GetBoardSize() const noexcept
		{
			return boardSize;
		}

		const Matrix<TetrominoType> &GetBoardState() const noexcept
		{
			return boardState;
		}

		const TetrominoState &GetTetrominoState() const noexcept
		{
			return currentTetrominoPositions;
		}

		const TetrominoState &GetGhostState() const noexcept
		{
			return currentGhostPositions;
		}

		const HoldQueue &GetHoldQueue() const noexcept
		{
			return holdQueue;
		}

		const NextQueue &GetNextQueue() const noexcept
		{
			return nextQueue;
		}

		const std::vector<int> &GetClearedRows() const noexcept
		{
			return clearedRows;
		}

		TetrominoType GetTetrominoType() const noexcept
		{
			return currentTetromino.tetrominoType;
		}

		const LineClearData &GetLineClearData() const noexcept
		{
			return previousLineClearData;
		}

		usize GetScore() const noexcept
		{
			return score;
		}

		void SetGravityState(bool gravityState) noexcept
		{
			this->gravityState = gravityState;
		}

		bool IsOccupied(int2 position) const noexcept
		{
			return position.x < 0 || position.x >= GetBoardSize().width || position.y < 0 || 
				IsValidTetrominoType(boardState[int2{ position.y, position.x }]);
		}

		bool IsOccupied(const TetrominoState &tetrominoPositions) const noexcept
		{
			for (int2 pos : tetrominoPositions)
			{
				if (IsOccupied(pos))
				{
					return true;
				}
			}

			return false;
		}

		bool CanMove(int2 offset) const noexcept // positive Y here means up...
		{
			return !IsOccupied(currentTetrominoPositions + offset);
		}

		int SoftDropPiece(int steps) // positive Y steps here means down instead...
		{
			int actualSteps = 0;

			while (actualSteps != steps)
			{
				++actualSteps;

				if (IsOccupied(currentTetrominoPositions - int2 { 0, actualSteps }))
				{
					--actualSteps;
					break;
				}
			}

			if (actualSteps != 0)
			{
				currentTetrominoPositions -= int2 { 0, actualSteps };
			}

			score += static_cast<usize>(actualSteps);
			return actualSteps;
		}

		void LockAndMoveNext()
		{
			for (int2 pos : currentTetrominoPositions)
			{
				boardState[int2{ pos.y, pos.x }] = GetTetrominoType();
			}

			currentLineClearData.longB2bStreakBroken = false;
			currentLineClearData.linesCleared = static_cast<int>(clearedRows.size());

			if (currentLineClearData.linesCleared > 0)
			{
				++currentLineClearData.combo;

				if (currentLineClearData.IsB2bClear())
				{
					++currentLineClearData.b2b;
				}
				else
				{
					if (currentLineClearData.b2b >= LineClearData::longB2bStreakThreshold)
					{
						currentLineClearData.longB2bStreakBroken = true;
					}

					currentLineClearData.b2b = -1;
				}
			}
			else
			{
				currentLineClearData.combo = -1;
			}

			for (usize row = 0; row < 40; ++row)
			{
				bool cleared = true;

				for (usize column = 0; column < 10; ++column)
				{
					if (!IsValidTetrominoType(boardState[usize2 { row, column }]))
					{
						cleared = false;
						break;
					}
				}

				if (cleared)
				{
					boardState.ClearRow(row);
					boardState.MoveRowToLast(row);
					--row;
				}
			}

			currentLineClearData.isAllClear = boardState.IsCleared();

			if (currentLineClearData.linesCleared > 0 || currentLineClearData.spinType != SpinType::None)
			{
				textFadeTimer = 0.0;
			}

			if (currentLineClearData.IsB2bClear())
			{
				usize score = 0;

				if (currentLineClearData.spinType == SpinType::Spin) // Spins
				{
					if (currentLineClearData.linesCleared == 0)
					{
						score += 400;
					}
					else if (currentLineClearData.linesCleared == 1)
					{
						score += 800;
					}
					else if (currentLineClearData.linesCleared == 2)
					{
						score += 1200;
					}
					else if (currentLineClearData.linesCleared == 3)
					{
						score += 1600;
					}
					else
					{
						score += 2600;
					}
				}
				else if (currentLineClearData.spinType == SpinType::MiniSpin) // Mini T-spins
				{
					if (currentLineClearData.linesCleared == 0)
					{
						score += 100;
					}
					else if (currentLineClearData.linesCleared == 1)
					{
						score += 200;
					}
					else
					{
						score += 400;
					}
				}
				else // Quads
				{
					score += 800;
				}

				if (currentLineClearData.b2b > 0)
				{
					score *= 3;
					score /= 2;
				}

				this->score += score;
			}
			else
			{
				if (currentLineClearData.linesCleared == 1) // Singles
				{
					score += 100;
				}
				else if (currentLineClearData.linesCleared == 2) // Doubles
				{
					score += 300;
				}
				else // Triples
				{
					score += 500;
				}
			}

			if (currentLineClearData.combo > 0)
			{
				score += static_cast<usize>(currentLineClearData.combo) * 50;
			}

			if (currentLineClearData.isAllClear)
			{
				score += 3500;
			}

			currentTetromino = GetNext();
			currentTetrominoPositions = currentTetromino.GetSpawnState();
			currentGhostPositions = CalculateGhostPositions();
			clearedRows = CalculateClearedLines();
			currentOrientation = Orientation::North;
			previousLineClearData = currentLineClearData;
			currentLineClearData = LineClearData::New(previousLineClearData, GetTetrominoType());
			gravityTimer.SetToMax();
		}

		void HardDropPiece() // A bit ironically, locking happens here
		{
			int dropSteps = SoftDropOnly(std::numeric_limits<int>::max());

			if (dropSteps > 0)
			{
				currentLineClearData.spinType = SpinType::None;
			}

			score += static_cast<usize>(dropSteps) * 2;
			LockAndMoveNext();
		}

		void MovePiece(int maxMovement)
		{
			int step = SignOf(maxMovement);
			int actualMovement = 0;

			while (actualMovement != maxMovement && CanMove(int2 { actualMovement + step, 0 }))
			{
				actualMovement += step;
			}

			currentTetrominoPositions += int2 { actualMovement, 0 };
			currentGhostPositions = CalculateGhostPositions();
			clearedRows = CalculateClearedLines();
		}

		void RotatePiece(RotateDirection rotateDirection)
		{
			Orientation newOrientation = RotateOrientation(currentOrientation, rotateDirection);
			const std::vector<int2> &kicks = currentTetromino.kickTable.GetKicks(RotationChange { currentOrientation, rotateDirection });
			int2 offset = currentTetrominoPositions - currentTetromino.kickTable.GetState(currentOrientation);
			TetrominoState newPositions = currentTetromino.kickTable.GetState(newOrientation) + offset;

			for (int2 kickOffset : kicks)
			{
				if (!IsOccupied(newPositions + kickOffset))
				{
					currentTetrominoPositions = newPositions + kickOffset;
					currentGhostPositions = CalculateGhostPositions();
					clearedRows = CalculateClearedLines();
					currentOrientation = newOrientation;

					if ((!CanMove(int2{ 0, -1 })) && IsValidTetrominoType(GetTetrominoType()))
					{
						if (GetTetrominoType() == TetrominoType::T) // T-spins are funny... https://tetris.wiki/T-Spin#Current_rules
						{
							Box box = Box::Bounding(currentTetromino, currentTetrominoPositions, currentOrientation);

							bool cornered[4] = { IsOccupied(box.GetTopLeft()), IsOccupied(box.GetTopRight()), 
								IsOccupied(box.GetBottomRight()), IsOccupied(box.GetBottomLeft()) }; // This assumes SRS-like T states smh...

							int occupiedCount = 0;

							for (bool cornerOccupied : cornered)
							{
								if (cornerOccupied)
								{
									++occupiedCount;
								}
							}

							if (occupiedCount >= 3)
							{
								// last kick check is simple because all the last kicks that can trigger this are all 1 tile sideway, 2 tiles downward
								if (Abs(kickOffset.x) >= 1 && kickOffset.y <= -2)
								{
									currentLineClearData.spinType = SpinType::Spin;
								}
								else // long ass front corners checks...
								{
									if (currentOrientation == Orientation::North && (cornered[0] && cornered[1]))
									{
										currentLineClearData.spinType = SpinType::Spin;
									}
									else if (currentOrientation == Orientation::East && (cornered[1] && cornered[2]))
									{
										currentLineClearData.spinType = SpinType::Spin;
									}
									else if (currentOrientation == Orientation::South && (cornered[2] && cornered[3]))
									{
										currentLineClearData.spinType = SpinType::Spin;
									}
									else if (currentOrientation == Orientation::West && (cornered[3] && cornered[0]))
									{
										currentLineClearData.spinType = SpinType::Spin;
									}
									else
									{
										currentLineClearData.spinType = SpinType::MiniSpin;
									}
								}
							}
							else
							{
								currentLineClearData.spinType = SpinType::None;
							}
						}
						else if ((!CanMove({ -1, 0 })) && (!CanMove({ 1, 0 })) && (!CanMove({ 0, 1 }))) // (Non T)-spins are much simpler...
						{
							currentLineClearData.spinType = SpinType::Spin;
						}
						else
						{
							currentLineClearData.spinType = SpinType::None;
						}
					}
					else
					{
						currentLineClearData.spinType = SpinType::None;
					}

					return;
				}
			}
		}

		void HoldPiece()
		{
			Nullable<Tetromino> heldTetromino = holdQueue.PopAndPush(currentTetromino);

			if (heldTetromino.HasValue())
			{
				currentTetromino = heldTetromino.Get();
			}
			else
			{
				currentTetromino = GetNext();
			}

			currentTetrominoPositions = currentTetromino.GetSpawnState();
			currentGhostPositions = CalculateGhostPositions();
			clearedRows = CalculateClearedLines();
			currentOrientation = Orientation::North;
			currentLineClearData.spinType = SpinType::None;
			currentLineClearData.tetrominoType = GetTetrominoType();
			gravityTimer.SetToMax();
		}

		void Reset()
		{
			boardState.Clear();
			randomizer.Reset();
			holdQueue.Reset();
			nextQueue.Fill(randomizer);
			currentTetromino = GetNext();
			currentTetrominoPositions = currentTetromino.GetSpawnState();
			currentGhostPositions = CalculateGhostPositions();
			clearedRows = CalculateClearedLines();
			currentOrientation = Orientation::North;
			gravityTimer.SetToMax();
			previousLineClearData = LineClearData::Default();
			currentLineClearData = LineClearData::New(GetTetrominoType());
			score = 0;
			textFadeTimer = 0.0;
		}

		void UpdateEvent(const SDL_Event &event) noexcept
		{
			controller.UpdateEvent(event, *this);
		}

		void Update(DeltaTime deltaTime) noexcept
		{
			if (gravityState)
			{
				if (CanMove(int2 { 0, -1 }))
				{
					int step = gravityTimer.Update(deltaTime);

					if (step > 0)
					{
						SoftDropOnly(step);
					}
				}
				else
				{
					gravityTimer.Reset();
				}
			}

			controller.Update(deltaTime, *this);

			if (textFadeTimer < fullyFadedThreshold)
			{
				textFadeTimer += deltaTime;
			}
		}
	};

	void Controller::UpdateEvent(const SDL_Event &event, Board &board) noexcept // TODO: Rewrite gravity support?
	{
		moveLeftInput.Update(event);
		moveRightInput.Update(event);
		rotateClockwiseInput.Update(event);
		rotateCounterclockwiseInput.Update(event);
		rotateClockwise180Input.Update(event);
		rotateCounterclockwise180Input.Update(event);
		primarySoftDropInput.Update(event);
		secondarySoftDropInput.Update(event);
		hardDropInput.Update(event);
		holdInput.Update(event);
		restartInput.Update(event);

		if (pressedMoveButton != MoveButton::Both)
		{
			if (moveLeftInput.IsHeld() && (!HasFlag(pressedMoveButton, MoveButton::Left)))
			{
				SetFlag(pressedMoveButton, MoveButton::Left);
				direction = -1;
			}

			if (moveRightInput.IsHeld() && (!HasFlag(pressedMoveButton, MoveButton::Right)))
			{
				SetFlag(pressedMoveButton, MoveButton::Right);
				direction = 1;
			}
		}
		
		if (pressedMoveButton != MoveButton::None)
		{
			if (moveLeftInput.IsReleased())
			{
				ClearFlag(pressedMoveButton, MoveButton::Left);
				direction = HasFlag(pressedMoveButton, MoveButton::Right) ? 1 : 0;
			}

			if (moveRightInput.IsReleased())
			{
				ClearFlag(pressedMoveButton, MoveButton::Right);
				direction = HasFlag(pressedMoveButton, MoveButton::Left) ? -1 : 0;
			}
		}

		if (pressedMoveButton == MoveButton::None)
		{
			movementTimer.Reset();
		}

		if (pressedSoftDropButton != SoftDropButton::Both)
		{
			if (primarySoftDropInput.IsHeld() && (!HasFlag(pressedSoftDropButton, SoftDropButton::Primary)))
			{
				SetFlag(pressedSoftDropButton, SoftDropButton::Primary);
				softDropTimer.SetTime(primarySoftDropHandling.arr);
				board.SetGravityState(false);
			}

			if (secondarySoftDropInput.IsHeld() && (!HasFlag(pressedSoftDropButton, SoftDropButton::Secondary)))
			{
				SetFlag(pressedSoftDropButton, SoftDropButton::Secondary);
				softDropTimer.SetTime(secondarySoftDropHandling.arr);
				board.SetGravityState(false);
			}
		}

		if (pressedSoftDropButton != SoftDropButton::None)
		{
			if (primarySoftDropInput.IsReleased())
			{
				ClearFlag(pressedSoftDropButton, SoftDropButton::Primary);

				if (HasFlag(pressedSoftDropButton, SoftDropButton::Secondary))
				{
					softDropTimer.SetTime(secondarySoftDropHandling.arr);
				}
			}

			if (secondarySoftDropInput.IsReleased())
			{
				ClearFlag(pressedSoftDropButton, SoftDropButton::Secondary);

				if (HasFlag(pressedSoftDropButton, SoftDropButton::Primary))
				{
					softDropTimer.SetTime(primarySoftDropHandling.arr);
				}
			}
		}

		if (pressedSoftDropButton == SoftDropButton::None)
		{
			//softDropTimer.Reset();
			board.SetGravityState(true);
		}

		if (hardDropInput.IsHeld() && (!hardDropPressed))
		{
			hardDropPressed = true;
			board.HardDropPiece();
		}

		if (hardDropInput.IsReleased())
		{
			hardDropPressed = false;
		}

		if (rotateClockwiseInput.IsHeld() && (!rotateClockwisePressed))
		{
			rotateClockwisePressed = true;
			board.RotatePiece(RotateDirection::Clockwise);
		}

		if (rotateClockwiseInput.IsReleased())
		{
			rotateClockwisePressed = false;
		}

		if (rotateCounterclockwiseInput.IsHeld() && (!rotateCounterclockwisePressed))
		{
			rotateCounterclockwisePressed = true;
			board.RotatePiece(RotateDirection::Counterclockwise);
		}

		if (rotateCounterclockwiseInput.IsReleased())
		{
			rotateCounterclockwisePressed = false;
		}

		if (rotateClockwise180Input.IsHeld() && (!rotateClockwise180Pressed))
		{
			rotateClockwise180Pressed = true;
			board.RotatePiece(RotateDirection::Clockwise180);
		}

		if (rotateClockwise180Input.IsReleased())
		{
			rotateClockwise180Pressed = false;
		}

		if (rotateCounterclockwise180Input.IsHeld() && (!rotateCounterclockwise180Pressed))
		{
			rotateCounterclockwise180Pressed = true;
			board.RotatePiece(RotateDirection::Counterclockwise180);
		}

		if (rotateCounterclockwise180Input.IsReleased())
		{
			rotateCounterclockwise180Pressed = false;
		}

		if (holdInput.IsHeld() && (!holdPressed))
		{
			holdPressed = true;
			board.HoldPiece();
		}

		if (holdInput.IsReleased())
		{
			holdPressed = false;
		}

		if (restartInput.IsHeld() && (!restartPressed))
		{
			restartPressed = true;
			board.Reset();
		}

		if (restartInput.IsReleased())
		{
			restartPressed = false;
		}
	}

	void Controller::Update(DeltaTime deltaTime, Board &board) noexcept
	{
		if (pressedSoftDropButton != SoftDropButton::None)
		{
			int steps = softDropTimer.Update(deltaTime);

			if (steps > 0)
			{
				board.SoftDropPiece(steps);
			}
		}

		if (pressedMoveButton != MoveButton::None)
		{
			int steps = movementTimer.Update(deltaTime);

			if (steps > 0)
			{
				board.MovePiece(steps * direction);
			}
		}
	}
}

namespace Lib::Sdl
{
	using namespace Stacker;

	struct TileMap final // BoardRenderGuide seems to be a better name
	{
	private:
		RectSize tileSize;
		int2 offset;
		int2 holdQueueOffset;
		int2 nextQueueOffset;

	public:
		constexpr TileMap() noexcept = default;
		constexpr TileMap(RectSize tileSize, int2 offset) noexcept : tileSize(tileSize), offset(offset) {}

		constexpr TileMap(RectSize tileSize, int2 offset, int2 holdQueueOffset, int2 nextQueueOffset) noexcept : tileSize(tileSize),
			offset(offset), holdQueueOffset(holdQueueOffset), nextQueueOffset(nextQueueOffset) {}

		RectSize GetTileSize() const noexcept
		{
			return tileSize;
		}

		void RenderTo(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_Rect *srcRect, int2 position) const
		{
			SDL_Rect rect = Rect(Scale(position, tileSize) + offset, tileSize);
			SDL_RenderCopy(renderer, texture, srcRect, &rect);
		}

		void RenderTo(SDL_Renderer *renderer, SDL_Texture *texture, Nullable<const SDL_Rect> srcRect, int2 position) const
		{
			SDL_Rect rect = Rect(Scale(position, tileSize) + offset, tileSize);
			SDL_RenderCopy(renderer, texture, srcRect.operator->(), &rect);
		}

		void RenderTo(SDL_Renderer *renderer, const Matrix<TetrominoType> &matrix,
			const std::unordered_map<TetrominoType, Texture> &tileTextures) const
		{
			usize rowCount = matrix.size(0);
			usize columnCount = matrix.size(1);

			for (usize row = 0; row < rowCount; ++row)
			{
				for (usize column = 0; column < columnCount; ++column)
				{
					TetrominoType tetrominoType = matrix[{ row, column }];

					if (IsValidTetrominoType(tetrominoType))
					{
						// position is the same as ReversedY({ column, row })
						RenderTo(renderer, tileTextures.at(tetrominoType), nullptr, int2{ static_cast<int>(column), -static_cast<int>(row) });
					}
				}
			}
		}

		void RenderTo(SDL_Renderer *renderer, const Board &board, const std::unordered_map<TetrominoType, Texture> &tileTextures,
			const std::unordered_map<TetrominoType, Texture> &ghostTextures, const Texture &spawnTexture, const Texture &clearedTexture, 
			const Texture &separatorTexture) const
		{
			RenderTo(renderer, board.GetBoardState(), tileTextures);
			const TetrominoState &tetrominoState = board.GetTetrominoState();
			const TetrominoState &ghostState = board.GetGhostState();
			const Texture &tileTexture = tileTextures.at(board.GetTetrominoType());
			const Texture &ghostTexture = ghostTextures.at(board.GetTetrominoType());
			int2 nextOffset = nextQueueOffset;
			int columns = board.GetBoardSize().width;
			Nullable<Tetromino> heldPiece = board.GetHoldQueue().Get();
			TetrominoState nextState = board.GetNextQueue().cbegin()->GetSpawnState();
			
			for (int2 position : ghostState)
			{
				RenderTo(renderer, ghostTexture, nullptr, ReversedY(position));
			}

			for (int2 position : tetrominoState)
			{
				RenderTo(renderer, tileTexture, nullptr, ReversedY(position));
			}

			for (int row : board.GetClearedRows())
			{
				for (int column = 0; column < columns; ++column)
				{
					RenderTo(renderer, clearedTexture, nullptr, int2{ static_cast<int>(column), -static_cast<int>(row) });
				}
			}

			for (int2 position : nextState)
			{
				RenderTo(renderer, spawnTexture, nullptr, ReversedY(position));
			}

			if (heldPiece.HasValue())
			{
				const Texture &heldPieceTexture = tileTextures.at(heldPiece->tetrominoType);

				for (int2 position : heldPiece.Get().kickTable.GetSpawnState())
				{
					SDL_Rect rect = Rect(Scale(ReversedY(position), tileSize) + holdQueueOffset, tileSize);
					SDL_RenderCopy(renderer, heldPieceTexture, nullptr, &rect);
					//RenderTo(renderer, tileTexture, nullptr, ReversedY(position));
				}
			}

			int nextIndex = Mod(static_cast<int>(board.GetBagIndex()) - static_cast<int>(board.GetNextSize()), static_cast<int>(board.GetBagSize()));

			for (const Tetromino &tetromino : board.GetNextQueue())
			{
				TetrominoState state = tetromino.kickTable.GetSpawnState();
				const Texture &texture = tileTextures.at(tetromino.tetrominoType);

				for (int2 position : state)
				{
					SDL_Rect rect = Rect(Scale(ReversedY(position), tileSize) + nextOffset, tileSize);
					SDL_RenderCopy(renderer, texture, nullptr, &rect);
				}

				if (nextIndex + 1 >= static_cast<int>(board.GetBagSize()))
				{
					SDL_Rect separatorRect = Rect(nextOffset + int2 { 0, 32 }, separatorTexture.GetSize());
					SDL_RenderCopy(renderer, separatorTexture, nullptr, &separatorRect);
					nextIndex = 0;
				}

				++nextIndex;
				nextOffset.y += tileSize.height * 4;
			}
		}
	};

	struct TextRenderGuide final
	{
	private:
		int2 origin;

	public:
		TextRenderGuide() noexcept = default;
		TextRenderGuide(int2 origin) noexcept : origin(origin) {}
		
		int2 GetOrigin() const noexcept
		{
			return origin;
		}

		void RenderTopRightAligned(SDL_Renderer *renderer, const Font &font, const string &text, SDL_Color color) const
		{
			if (color.a > 0)
			{
				SDL_Rect rect = RectWithTopRightPosition(origin, font.SizeUtf8(text));
				font.RenderUtf8(text, color, renderer, nullptr, &rect);
			}
		}

		void RenderTopCenterAligned(SDL_Renderer *renderer, const Font &font, const string &text, SDL_Color color) const
		{
			if (color.a > 0)
			{
				SDL_Rect rect = RectWithTopMiddlePosition(origin, font.SizeUtf8(text));
				font.RenderUtf8(text, color, renderer, nullptr, &rect);
			}
		}
	};
}

#endif // !STACKER_DEFINED
