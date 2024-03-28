#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <random>
#include <unordered_set>

using std::string;

using usize = std::size_t;

constexpr const char *words[] = { "angle", "ant", "apple", "arch", "arm", "army",
"baby", "bag", "ball", "band", "basin", "basket", "bath", "bed", "bee", "bell", "berry",
"bird", "blade", "board", "boat", "bone", "book", "boot", "bottle", "box", "boy",
"brain", "brake", "branch", "brick", "bridge", "brush", "bucket", "bulb", "button",
"cake", "camera", "card", "cart", "carriage", "cat", "chain", "cheese", "chest",
"chin", "church", "circle", "clock", "cloud", "coat", "collar", "comb", "cord",
"cow", "cup", "curtain", "cushion",
"dog", "door", "drain", "drawer", "dress", "drop", "ear", "egg", "engine", "eye",
"face", "farm", "feather", "finger", "fish", "flag", "floor", "fly",
"foot", "fork", "fowl", "frame", "garden", "girl", "glove", "goat", "gun",
"hair", "hammer", "hand", "hat", "head", "heart", "hook", "horn", "horse",
"hospital", "house", "island", "jewel", "kettle", "key", "knee", "knife", "knot",
"leaf", "leg", "library", "line", "lip", "lock",
"map", "match", "monkey", "moon", "mouth", "muscle",
"nail", "neck", "needle", "nerve", "net", "nose", "nut",
"office", "orange", "oven", "parcel", "pen", "pencil", "picture", "pig", "pin",
"pipe", "plane", "plate", "plow", "pocket", "pot", "potato", "prison", "pump",
"rail", "rat", "receipt", "ring", "rod", "roof", "root",
"sail", "school", "scissors", "screw", "seed", "sheep", "shelf", "ship", "shirt",
"shoe", "skin", "skirt", "snake", "sock", "spade", "sponge", "spoon", "spring",
"square", "stamp", "star", "station", "stem", "stick", "stocking", "stomach",
"store", "street", "sun", "table", "tail", "thread", "throat", "thumb", "ticket",
"toe", "tongue", "tooth", "town", "train", "tray", "tree", "trousers", "umbrella",
"wall", "watch", "wheel", "whip", "whistle", "window", "wire", "wing", "worm", };

constexpr const char *hangmanFigures[] = 
{ 
" ------------- \n"
" | \n"
" | \n"
" | \n"
" | \n"
" | \n"
" ----- \n",
" ------------- \n"
" | | \n"
" | \n"
" | \n"
" | \n"
" | \n"
" ----- \n",
" ------------- \n"
" | | \n"
" | O \n"
" | \n"
" | \n"
" | \n"
" ----- \n",

" ------------- \n"
" | | \n"
" | O \n"
" | | \n"
" | \n"
" | \n"
" ----- \n",
" ------------- \n"
" | | \n"
" | O \n"
" | /| \n"
" | \n"
" | \n"
" ----- \n",
" ------------- \n"
" | | \n"
" | O \n"
" | /|\\ \n"
" | \n"
" | \n"
" ----- \n",

" ------------- \n"
" | | \n"
" | O \n"
" | /|\\ \n"
" | / \n"
" | \n"
" ----- \n",
" ------------- \n"
" | | \n"
" | O \n"
" | /|\\ \n"
" | / \\ \n"
" | \n"
" ----- \n",
};

constexpr char placeholderChar = '-';
constexpr usize failedGuessCountToEnd = 8;

class Randomizer final
{
private:
	std::random_device device;
	std::mt19937_64 rng;
	std::uniform_int_distribution<usize> distribution;

public:
	Randomizer() : device(std::random_device()), rng(std::mt19937_64(device())), distribution(std::uniform_int_distribution<usize>()) {}

	Randomizer(usize inclusiveMin, usize inclusiveMax) : device(std::random_device()), rng(std::mt19937_64(device())),
		distribution(std::uniform_int_distribution<usize>(inclusiveMin, inclusiveMax)) {}

	usize operator()()
	{
		return distribution(rng);
	}
};

auto contains = [](const string &str, char character) -> bool
{
	for (char c : str)
	{
		if (c == character)
		{
			return true;
		}
	}

	return false;
};

auto replace = [](string &str, const string &original, char newChar) -> void
{
	for (usize i = 0; i < original.length(); ++i)
	{
		if (original[i] == newChar)
		{
			str[i] = newChar;
		}
	}
};

int main()
{
	string pickedWord = string(words[Randomizer(0, std::size(words) - 1)()]);
	string word = string(pickedWord.size(), placeholderChar);
	std::unordered_set<char> pickedCharacters = std::unordered_set<char>();
	usize failedGuessCount = 0;

	while (true)
	{
		std::cout << pickedWord << ' ' << word << '\n' << hangmanFigures[failedGuessCount] << '\n';
		char guessedCharacter;
		std::cout << word << "\n\n";
		std::cin >> guessedCharacter;

		if (pickedCharacters.contains(guessedCharacter))
		{
			std::cout << "Character already picked! Pick again!\n";
		}
		else
		{
			pickedCharacters.insert(guessedCharacter);

			if (contains(pickedWord, guessedCharacter))
			{
				replace(word, pickedWord, guessedCharacter);

				if (word == pickedWord)
				{
					std::cout << "You win!\n";
					break;
				}
			}
			else
			{
				++failedGuessCount;

				if (failedGuessCount >= failedGuessCountToEnd)
				{
					std::cout << "Game Over!\n";
					break;
				}
			}
		}
	}

	return 0;
}
