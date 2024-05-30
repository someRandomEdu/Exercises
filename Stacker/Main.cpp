#include <iostream>
#include <vector>
#include <span>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "Lib.hpp"
#include "Memory.hpp"
#include "Collections.hpp"
#include "IO.hpp"
#include "Debug.hpp"
#include "Time.hpp"
#include "SdlLib.hpp"
#include "Stacker.hpp"

using namespace Lib;
using namespace Lib::Sdl;
using namespace Lib::Sdl::Text;
using namespace Lib::Memory;
using namespace Lib::Time;
using namespace Lib::IO;

using namespace Stacker;

// Go to https://lazyfoo.net/tutorials/SDL/ and https://www.studyplan.dev/ for some SDL2 tutorials!
int main(int argc, char *argv[]) // main is now a macro!
{
	using enum Stacker::TetrominoType;

	// 125% scale on device...
	constexpr int width = 1920 * 4 / 5;
	constexpr int height = 1080 * 4 / 5;
	constexpr int boardWidth = 10;
	constexpr int boardHeight = 20;

	std::atexit([]() -> void
	{
		FreePathPtr();
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
	});
	
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	basePath = SDL_GetBasePath();
	assetPath = string(basePath) + "Assets\\Images\\";
	fontPath = string(basePath) + "Assets\\Fonts\\";
	SDL_Event event = {};
	Window window = {};
	Renderer renderer = {};
	CreateWindowAndRenderer(width, height, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_ALLOW_HIGHDPI, window, renderer);

	std::unordered_map<TetrominoType, Texture> tetrominoTextures = Lib::Collections::Helpers::unordered_map<TetrominoType, Texture>(
	{
		{ TetrominoType::I, Texture(renderer, GetPath("I.png")) },
		{ TetrominoType::J, Texture(renderer, GetPath("J.png")) },
		{ TetrominoType::L, Texture(renderer, GetPath("L.png")) },
		{ TetrominoType::O, Texture(renderer, GetPath("O.png")) },
		{ TetrominoType::S, Texture(renderer, GetPath("S.png")) },
		{ TetrominoType::T, Texture(renderer, GetPath("T.png")) },
		{ TetrominoType::Z, Texture(renderer, GetPath("Z.png")) },
	});

	std::unordered_map<TetrominoType, Texture> ghostTextures = Lib::Collections::Helpers::unordered_map<TetrominoType, Texture>(
	{
		{ TetrominoType::I, Texture(renderer, GetPath("Ghost I.png")) },
		{ TetrominoType::J, Texture(renderer, GetPath("Ghost J.png")) },
		{ TetrominoType::L, Texture(renderer, GetPath("Ghost L.png")) },
		{ TetrominoType::O, Texture(renderer, GetPath("Ghost O.png")) },
		{ TetrominoType::S, Texture(renderer, GetPath("Ghost S.png")) },
		{ TetrominoType::T, Texture(renderer, GetPath("Ghost T.png")) },
		{ TetrominoType::Z, Texture(renderer, GetPath("Ghost Z.png")) },
	});

	Texture gridTexture = Texture(renderer, GetPath("Grid.png"));
	Texture spawnTexture = Texture(renderer, GetPath("Spawn.png"));
	Texture clearedTexture = Texture(renderer, GetPath("Cleared.png"));
	Texture separatorTexture = Texture(renderer, GetPath("Separator.png"));
	Font spinFont = Font(GetFontPath("hun2.ttf"), 24);
	Font lineClearFont = Font(GetFontPath("hun2.ttf"), 36);
	Font b2bFont = Font(GetFontPath("hun2.ttf"), 24);
	Font comboFont = Font(GetFontPath("hun2.ttf"), 36);
	Font allClearFont = Font(GetFontPath("hun2.ttf"), 24);
	Font scoreFont = Font(GetFontPath("hun2.ttf"), 24);
	TextRenderGuide spinRenderGuide = TextRenderGuide(int2 { width / 2 - 192, height - 32 * 18 });
	TextRenderGuide lineClearRenderGuide = TextRenderGuide(int2 { width / 2 - 192, height - 32 * 17 });
	TextRenderGuide b2bRenderGuide = TextRenderGuide(int2 { width / 2 - 196, height - 32 * 15 - 16 });
	TextRenderGuide comboRenderGuide = TextRenderGuide(int2 { width / 2 - 192, height - 32 * 14 });
	TextRenderGuide allClearRenderGuide = TextRenderGuide(int2 { width / 2 - 192, height - 32 * 12 + 16 });
	TextRenderGuide scoreRenderGuide = TextRenderGuide(int2{ width / 2, height - 24 });

	TileMap tileMap = TileMap(RectSize { 32, 32 }, int2 { width / 2 - 160, height - 64 }, int2 { width / 2 - 32 * 10, height - 32 * 19 }, 
		int2 { width / 2 + 32 * 6, height - 32 * 19 });

	renderer.SetRenderDrawColor(0, 0, 0, 255);
	StdTimer timer = StdTimer();
	Board board = Board();
	bool looping = true;

	while (looping)
	{
		DeltaTime deltaTime = timer.GetDeltaTime();

		while (PollEvent(event) != 0)
		{
			if (event.type == SDL_EventType::SDL_QUIT || Held(event, SDL_KeyCode::SDLK_ESCAPE))
			{
				looping = false;
			}

			board.UpdateEvent(event);
		}

		board.Update(deltaTime);
		renderer.RenderClear();

		for (int i = 0; i < boardWidth; ++i)
		{
			for (int j = 0; j < boardHeight; ++j)
			{
				//tileMap.RenderTo(renderer, gridTexture, nullptr, ReversedY(int2{i, j}));
				tileMap.RenderTo(renderer, gridTexture, nullptr, int2 { i, -j });
			}
		}

		tileMap.RenderTo(renderer, board, tetrominoTextures, ghostTextures, spawnTexture, clearedTexture, separatorTexture);
		const LineClearData &lineClearData = board.GetLineClearData();
		string spinText = lineClearData.GetSpinText();
		string lineClearText = lineClearData.GetLineClearText();
		string allClearText = lineClearData.GetAllClearText();
		string b2bText = lineClearData.GetB2bText();
		string comboText = lineClearData.GetComboText();
		Uint8 alpha = board.GetTextAlpha();
		usize score = board.GetScore();
		
		if (!spinText.empty())
		{
			spinRenderGuide.RenderTopRightAligned(renderer, spinFont, spinText, Color(lineClearData.GetColor(), alpha));
		}

		if (!lineClearText.empty())
		{
			lineClearRenderGuide.RenderTopRightAligned(renderer, lineClearFont, lineClearText, SDL_Color { 255, 255, 255, alpha });
		}

		if (!b2bText.empty())
		{
			b2bRenderGuide.RenderTopRightAligned(renderer, b2bFont, b2bText, Color(lineClearData.GetB2bColor(), lineClearData.longB2bStreakBroken ? alpha : 255));
		}

		if (!comboText.empty())
		{
			comboRenderGuide.RenderTopRightAligned(renderer, comboFont, comboText, SDL_Color { 255, 255, 255, alpha });
		}

		if (!allClearText.empty())
		{
			allClearRenderGuide.RenderTopRightAligned(renderer, allClearFont, allClearText, SDL_Color { 206, 197, 82, alpha });
		}

		scoreRenderGuide.RenderTopCenterAligned(renderer, scoreFont, std::to_string(score), SDL_Color { 255, 255, 255, 255 });
		renderer.RenderPresent();
	}

	return 0;
}
