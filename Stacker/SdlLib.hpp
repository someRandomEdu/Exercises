#ifndef SDL_LIB_DEFINED
#define SDL_LIB_DEFINED

#pragma once

#include <vector>
#include <functional>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "Lib.hpp"
#include "Memory.hpp"
//#include "Stacker.hpp"

using namespace Lib;
using namespace Lib::Data;
using namespace Lib::Memory;
using namespace Lib::Memory::Unsafe;

//using namespace Stacker;

namespace Lib::Data
{
	enum struct BlendMode : std::underlying_type_t<SDL_BlendMode>
	{
		None = SDL_BLENDMODE_NONE,
		Blend = SDL_BLENDMODE_BLEND,
		Add = SDL_BLENDMODE_ADD,
		Mod = SDL_BLENDMODE_MOD,
		Mul = SDL_BLENDMODE_MUL,
		Invalid = SDL_BLENDMODE_INVALID,
	};
	
	constexpr SDL_Point operator+(const SDL_Point &value) noexcept
	{
		return value;
	}

	constexpr SDL_Point operator-(const SDL_Point &value) noexcept
	{
		return { -value.x, -value.y };
	}

	constexpr SDL_Point operator+(const SDL_Point &lhs, const SDL_Point &rhs) noexcept
	{
		return { lhs.x + rhs.x, lhs.y + rhs.y };
	}

	constexpr SDL_Point operator-(const SDL_Point &lhs, const SDL_Point &rhs) noexcept
	{
		return { lhs.x - rhs.x, lhs.y - rhs.y };
	}

	constexpr SDL_Point operator*(const SDL_Point &value, int scale) noexcept
	{
		return { value.x * scale, value.y * scale };
	}

	constexpr SDL_Point operator*(int scale, const SDL_Point &value) noexcept
	{
		return { value.x * scale, value.y * scale };
	}

	constexpr SDL_Point operator/(const SDL_Point &value, int scale) noexcept
	{
		return { value.x / scale, value.y / scale };
	}

	constexpr SDL_Point &operator+=(SDL_Point &value, const SDL_Point &other) noexcept
	{
		value.x += other.x;
		value.y += other.y;
		return value;
	}

	constexpr SDL_Point &operator-=(SDL_Point &value, const SDL_Point &other) noexcept
	{
		value.x -= other.x;
		value.y -= other.y;
		return value;
	}

	constexpr SDL_Point &operator*=(SDL_Point &value, int scale) noexcept
	{
		value.x *= scale;
		value.y *= scale;
		return value;
	}

	constexpr SDL_Point &operator/=(SDL_Point &value, int scale) noexcept
	{
		value.x /= scale;
		value.y /= scale;
		return value;
	}

	constexpr bool operator==(const SDL_Point &lhs, const SDL_Point &rhs) noexcept
	{
		return lhs.x == rhs.x && lhs.y == rhs.y;
	}

	constexpr bool operator!=(const SDL_Point &lhs, const SDL_Point &rhs) noexcept
	{
		return lhs.x != rhs.x || lhs.y != rhs.y;
	}

	constexpr SDL_Rect operator+(const SDL_Rect &rect, int2 offset) noexcept
	{
		return SDL_Rect { rect.x + offset.x, rect.y + offset.y, rect.w, rect.h, };
	}

	constexpr SDL_Rect operator-(const SDL_Rect &rect, int2 offset) noexcept
	{
		return SDL_Rect{ rect.x - offset.x, rect.y - offset.y, rect.w, rect.h, };
	}

	constexpr SDL_Rect &operator+=(SDL_Rect &value, int2 offset) noexcept
	{
		value.x += offset.x;
		value.y += offset.y;
		return value;
	}

	constexpr SDL_Rect &operator-=(SDL_Rect &value, int2 offset) noexcept
	{
		value.x -= offset.x;
		value.y -= offset.y;
		return value;
	}

	constexpr bool operator==(const SDL_Rect &lhs, const SDL_Rect &rhs) noexcept
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.w == rhs.w && lhs.h == rhs.h;
	}

	constexpr bool operator!=(const SDL_Rect &lhs, const SDL_Rect &rhs) noexcept
	{
		return lhs.x != rhs.x || lhs.y != rhs.y || lhs.w != rhs.w || lhs.h != rhs.h;
	}

	constexpr SDL_Rect Rect(int2 topLeftCornerPosition, RectSize size) noexcept
	{
		return { topLeftCornerPosition.x, topLeftCornerPosition.y, size.width, size.height };
	}

	constexpr SDL_Rect Rect(int2 topLeftCornerPosition, int width, int height) noexcept
	{
		return { topLeftCornerPosition.x, topLeftCornerPosition.y, width, height };
	}

	constexpr SDL_Rect RectWithTopRightPosition(int2 topRightPosition, RectSize size) noexcept
	{
		return { topRightPosition.x - size.width, topRightPosition.y, size.width, size.height };
	}

	constexpr SDL_Rect RectWithTopMiddlePosition(int2 topMiddlePosition, RectSize size) noexcept
	{
		return { topMiddlePosition.x - size.width / 2, topMiddlePosition.y, size.width, size.height };
	}

	constexpr SDL_Rect Rect(int x, int y, int width, int height) noexcept
	{
		return { x, y, width, height };
	}

	constexpr SDL_Rect Rect(int x, int y, RectSize size) noexcept
	{
		return { x, y, size.width, size.height };
	}

	constexpr SDL_Color Color(SDL_Color color, Uint8 alpha) noexcept
	{
		return { color.r, color.g, color.b, alpha };
	}

	std::istream &operator>>(std::istream &stream, SDL_Rect &value)
	{
		return stream >> value.x >> value.y >> value.w >> value.h;
	}

	std::ostream &operator<<(std::ostream &stream, const SDL_Rect &value)
	{
		return stream << "Top left position: (" << value.x << ", " << value.y << "); Size: (" << value.w << ", " << value.h << ')';
	}
}

namespace Lib::Sdl
{
	namespace
	{
		SDL_Window *CreateWindow(const char title[], int x, int y, int width, int height, Uint32 flags)
		{
			return SDL_CreateWindow(title, x, y, width, height, flags);
		}

		SDL_Surface *GetWindowSurface(SDL_Window *window)
		{
			return SDL_GetWindowSurface(window);
		}

		SDL_Surface *LoadSurface(const string &path)
		{
			SDL_Surface *loadedSurface = IMG_Load(path.c_str());

			if (loadedSurface != nullptr)
			{
				SDL_Surface *optimizedSurface = SDL_ConvertSurface(loadedSurface, loadedSurface->format, 0);
				SDL_FreeSurface(loadedSurface);
				return optimizedSurface;
			}
			else
			{
				return nullptr;
			}
		}
	}

	struct Window;
	struct Renderer;

	inline char *basePath = SDL_GetBasePath();
	/*inline string assetPath = "D:\\Projects\\SdlProject\\Images\\";
	inline string fontPath = "D:\\Projects\\SdlProject\\Fonts\\";*/
	inline string assetPath = "Assets\\Images";
	inline string fontPath = "Assets\\Fonts";
		
	int CreateWindowAndRenderer(int width, int height, Uint32 windowFlags, Window &window, Renderer &renderer);

	void FreePathPtr()
	{
		SDL_free(basePath);
		basePath = nullptr;
	}

	string GetPath(const string &path) noexcept
	{
		return assetPath + path;
	}

	string GetPath(const char path[]) noexcept
	{
		return assetPath + path;
	}

	string GetFontPath(const string &path)
	{
		return fontPath + path;
	}

	string GetFontPath(const char path[])
	{
		return fontPath + path;
	}

	int PollEvent(SDL_Event *event)
	{
		return SDL_PollEvent(event);
	}

	int PollEvent(SDL_Event &event)
	{
		return SDL_PollEvent(std::addressof(event));
	}

	const char *GetBasePath()
	{
		return SDL_GetBasePath();
	}

	struct SdlTimer final // https://lazyfoo.net/tutorials/SDL/23_advanced_timers/index.php
	{
	private:
		Uint64 startTicks;
		Uint64 pausedTicks;
		bool paused;
		bool started;

	public:
		bool IsPaused() const noexcept
		{
			return paused;
		}

		bool IsStarted() const noexcept
		{
			return started;
		}

		void Start()
		{
			started = true;
			paused = false;
			startTicks = SDL_GetTicks64();
			pausedTicks = 0;
		}

		void Stop()
		{
			started = false;
			paused = false;
			startTicks = 0;
			pausedTicks = 0;
		}

		void Pause()
		{
			if (started && !paused)
			{
				paused = true;
				startTicks = 0;
				pausedTicks = SDL_GetTicks64() - startTicks;
			}
		}

		void Unpause()
		{
			if (started && paused)
			{
				paused = false;
				startTicks = SDL_GetTicks64() - pausedTicks;
				pausedTicks = 0;
			}
		}

		Uint64 GetMiliseconds() const
		{
			return started ? (paused ? pausedTicks : (SDL_GetTicks64() - startTicks)) : 0;
		}

		DeltaTime GetDeltaTime() const
		{
			return static_cast<DeltaTime>(GetMiliseconds()) / static_cast<DeltaTime>(1000.0);
		}
	};

	struct Surface final
	{
	private:
		SDL_Surface *surface;

	public:
		Surface() noexcept = default;
		Surface(SDL_Surface *surface) noexcept : surface(surface) {}
		Surface(const string &fileName) : surface(IMG_Load(fileName.c_str())) {}
		Surface(SDL_Window *window) : surface(SDL_GetWindowSurface(window)) {}
		Surface(Surface &&other) noexcept : surface(std::exchange(other.surface, nullptr)) {}

		Surface &operator=(Surface &&other) noexcept
		{
			SDL_Surface *temp = other.surface;
			other.surface = nullptr;
			surface = temp;
			return *this;
		}

		SDL_Surface *GetSurface() const noexcept
		{
			return surface;
		}

		SDL_Surface *operator->() const noexcept
		{
			return surface;
		}

		operator SDL_Surface *() const noexcept
		{
			return surface;
		}

		explicit operator bool() const noexcept
		{
			return surface != nullptr;
		}

		~Surface() noexcept
		{
			SDL_FreeSurface(surface);
		}
	};

	struct Window final
	{
	private:
		friend int CreateWindowAndRenderer(int width, int height, Uint32 windowFlags, Window &window, Renderer &renderer);

		SDL_Window *window;

	public:
		Window() noexcept = default;
		Window(SDL_Window *window) noexcept : window(window) {}

		Window(const char title[], int x, int y, int width, int height, Uint32 flags) : 
			window(SDL_CreateWindow(title, x, y, width, height, flags)) {}

		Window(const char title[], int x, int y, int width, int height, Uint32 flags, Uint8 r, Uint8 g, Uint8 b) :
			Window(title, x, y, width, height, flags)
		{
			SDL_Surface *surface = GetSurface();
			SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, r, g, b));
		}

		Window(const char title[], int x, int y, int width, int height, Uint32 flags, Uint8 r, Uint8 g, Uint8 b, Uint8 a) :
			Window(title, x, y, width, height, flags)
		{
			SDL_Surface *surface = GetSurface();
			SDL_FillRect(surface, nullptr, SDL_MapRGBA(surface->format, r, g, b, a));
		}

		Window(Window &&other) noexcept : window(std::exchange(other.window, nullptr)) {}

		Window &operator=(Window &&other) noexcept
		{
			window = std::exchange(other.window, nullptr);
			return *this;
		}

		SDL_Window *GetWindow() const noexcept
		{
			return window;
		}

		SDL_Surface *GetSurface() const noexcept
		{
			return SDL_GetWindowSurface(window);
		}

		SDL_Renderer *GetRenderer() const noexcept
		{
			return SDL_GetRenderer(window);
		}

		int FillRect(const SDL_Rect *rect, Uint32 color) const
		{
			SDL_Surface *surface = GetSurface();
			return SDL_FillRect(surface, rect, color);
		}

		int FillRect(Nullable<const SDL_Rect> rect, Uint32 color) const
		{
			SDL_Surface *surface = GetSurface();
			return SDL_FillRect(surface, rect.operator->(), color);
		}

		int	SetWindowFullScreen(Uint32 flags) const
		{
			return SDL_SetWindowFullscreen(window, flags);
		}

		RectSize GetWindowSize() const
		{
			RectSize result = {};
			SDL_GetWindowSize(window, &result.width, &result.height);
			return result;
		}

		void GetWindowSize(int *width, int *height) const
		{
			SDL_GetWindowSize(window, width, height);
		}

		void SetWindowSize(int width, int height) const
		{
			SDL_SetWindowSize(window, width, height);
		}

		void SetWindowPosition(int x, int y) const
		{
			SDL_SetWindowPosition(window, x, y);
		}

		void Render() const
		{
			SDL_UpdateWindowSurface(window);
		}

		operator SDL_Window *() const noexcept
		{
			return window;
		}

		explicit operator bool() const noexcept
		{
			return window != nullptr;
		}

		~Window()
		{
			if (window != nullptr)
			{
				SDL_DestroyWindow(window);
			}
		}
	};

	/*struct Image final
	{
	private:
		SDL_Surface *surface;

	public:
		Image() noexcept = default;

		Image(const string &fileName, SDL_Surface *surface)
		{
			SDL_Surface *temp = IMG_Load(fileName.c_str());
			this->surface = SDL_ConvertSurface(temp, surface->format, 0);
			SDL_FreeSurface(temp);
			SDL_BlitSurface(this->surface, nullptr, surface, nullptr);
		}

		Image(Image &&other) noexcept : surface(std::exchange(other.surface, nullptr)) {}

		Image &operator=(Image &&other) noexcept
		{
			surface = std::exchange(other.surface, nullptr);
			return *this;
		}

		explicit operator bool() const noexcept
		{
			return surface != nullptr;
		}

		~Image()
		{
			SDL_FreeSurface(surface);
		}
	};*/

	struct Texture final
	{
	private:
		SDL_Texture *texture;

	public:
		Texture() noexcept = default;
		Texture(SDL_Texture *texture) noexcept : texture(texture) {}
		Texture(SDL_Renderer *renderer, SDL_Surface *surface) : texture(SDL_CreateTextureFromSurface(renderer, surface)) {}
		Texture(SDL_Renderer *renderer, const string &fileName) : texture(IMG_LoadTexture(renderer, fileName.c_str())) {}

		Texture(SDL_Renderer *renderer, Uint32 format, int access, int width, int height) : 
			texture(SDL_CreateTexture(renderer, format, access, width, height)) {}

		Texture(Texture &&other) noexcept : texture(std::exchange(other.texture, nullptr)) {}

		Texture &operator=(Texture &&other) noexcept
		{
			if (texture != nullptr)
			{
				SDL_DestroyTexture(texture);
			}

			texture = std::exchange(other.texture, nullptr);
			return *this;
		}

		SDL_Texture *GetTexture() const noexcept
		{
			return texture;
		}

		/// @brief Returns the size of this texture. Only valid if called with a valid texture
		/// @return The size of this texture
		RectSize GetSize() const
		{
			RectSize result = {};
			SDL_QueryTexture(texture, nullptr, nullptr, &result.width, &result.height);
			return result;
		}

		int SetTextureScaleMode(SDL_ScaleMode scaleMode) const
		{
			return SDL_SetTextureScaleMode(texture, scaleMode);
		}

		int SetTextureBlendMode(SDL_BlendMode blendMode) const
		{
			return SDL_SetTextureBlendMode(texture, blendMode);
		}

		int SetTextureAlphaMod(Uint8 alpha) const
		{
			return SDL_SetTextureAlphaMod(texture, alpha);
		}

		int QueryTexture(Uint32 *format, int *access, int *width, int *height) const
		{
			return SDL_QueryTexture(texture, format, access, width, height);
		}

		int QueryTexture(Ref<Uint32> format, Ref<int> access, Ref<int> width, Ref<int> height) const
		{
			return SDL_QueryTexture(texture, format.operator->(), access.operator->(), width.operator->(), height.operator->());
		}

		operator SDL_Texture *() const noexcept
		{
			return texture;
		}

		explicit operator bool() const noexcept
		{
			return texture != nullptr;
		}

		~Texture()
		{
			if (texture != nullptr)
			{
				SDL_DestroyTexture(texture);
			}
		}
	};

	struct Renderer final
	{
	private:
		friend int CreateWindowAndRenderer(int width, int height, Uint32 windowFlags, Window &window, Renderer &renderer);

		SDL_Renderer *renderer;

	public:
		Renderer() noexcept = default;
		Renderer(SDL_Renderer *renderer) noexcept : renderer(renderer) {}
		Renderer(SDL_Window *window) : renderer(SDL_GetRenderer(window)) {}
		Renderer(SDL_Window *window, int index, Uint32 flags) : renderer(SDL_CreateRenderer(window, index, flags)) {}
		Renderer(Renderer &&other) noexcept : renderer(std::exchange(other.renderer, nullptr)) {}

		Renderer &operator=(Renderer &&other) noexcept
		{
			if (renderer != nullptr)
			{
				SDL_DestroyRenderer(renderer);
			}

			renderer = std::exchange(other.renderer, nullptr);
			return *this;
		}

		SDL_Renderer *GetRenderer() const noexcept
		{
			return renderer;
		}

		operator SDL_Renderer *() const noexcept 
		{
			return renderer;
		}

		explicit operator bool() const noexcept
		{
			return renderer != nullptr;
		}

		int RenderCopy(SDL_Texture *texture, const SDL_Rect *rect, const SDL_Rect *disRect)
		{
			return SDL_RenderCopy(renderer, texture, rect, disRect);
		}

		int RenderCopy(SDL_Texture *texture, Nullable<const SDL_Rect> rect, Nullable<const SDL_Rect> disRect)
		{
			return SDL_RenderCopy(renderer, texture, rect.operator->(), disRect.operator->());
		}

		int SetRenderDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
		{
			return SDL_SetRenderDrawColor(renderer, r, g, b, a);
		}

		int SetRenderDrawBlendMode(SDL_BlendMode blendMode)
		{
			return SDL_SetRenderDrawBlendMode(renderer, blendMode);
		}

		void RenderClear()
		{
			SDL_RenderClear(renderer);
		}

		void RenderPresent()
		{
			SDL_RenderPresent(renderer);
		}

		~Renderer()
		{
			if (renderer != nullptr)
			{
				SDL_DestroyRenderer(renderer);
			}
		}
	};

	struct Sprite final
	{
	private:
		SharedPtr<Texture> texture;
		RectSize imageSize;

	public:
		Sprite() noexcept = default;
		Sprite(SDL_Texture *texture) : texture(MakeShared<Texture>(texture)), imageSize(this->texture->GetSize()) {}
		Sprite(Texture &&texture) : texture(SharedPtr<Texture>(std::move(texture))), imageSize(this->texture->GetSize()) {}

		RectSize GetSize() const noexcept
		{
			return imageSize;
		}

		void Resize(RectSize value) noexcept
		{
			imageSize = value;
		}

		void ScaleBy(int scale) noexcept
		{
			imageSize *= scale;
		}

		void ScaleBy(double scale) noexcept
		{
			imageSize *= scale;
		}

		void DownscaleBy(int scale) noexcept
		{
			imageSize /= scale;
		}

		void DownscaleBy(double scale) noexcept
		{
			imageSize /= scale;
		}

		void RenderTo(SDL_Renderer *renderer, const SDL_Rect *srcRect, int2 position) const
		{
			SDL_Rect rect = Rect(position, imageSize);
			SDL_RenderCopy(renderer, texture->GetTexture(), srcRect, &rect);
		}
	};

	struct Input final
	{
	private:
		enum struct InputState : char
		{
			Untriggered = 0,
			Held = 1,
			Released = 2,
		};

		SDL_KeyCode key;
		InputState state;

	public:
		constexpr Input() noexcept = default;
		constexpr Input(SDL_KeyCode key) noexcept : key(key), state(InputState::Untriggered) {}

		constexpr SDL_KeyCode GetKey() const noexcept
		{
			return key;
		}

		constexpr bool IsHeld() const noexcept
		{
			return state == InputState::Held;
		}

	 	constexpr bool IsReleased() const noexcept
		{
			return state == InputState::Released;
		}

		constexpr bool IsUntriggered() const noexcept
		{
			return (!IsHeld()) && (!IsReleased());
		}

		constexpr void Update(const SDL_Event &event) noexcept
		{
			if (Held(event, key))
			{
				state = InputState::Held;
			}
			else if (Released(event, key))
			{
				state = InputState::Released;
			}
			else
			{
				state = InputState::Untriggered;
			}
		}
	};

	struct AxisInput final
	{
	private:
		SDL_KeyCode positiveKey;
		SDL_KeyCode negativeKey;
		int value;

	public:
		constexpr AxisInput() noexcept = default;

		constexpr AxisInput(SDL_KeyCode positiveKey, SDL_KeyCode negativeKey) noexcept : positiveKey(positiveKey), negativeKey(negativeKey),
			value(0) {}

		constexpr void Update(const SDL_Event &event) noexcept
		{

		}
	};

	struct InputManager final
	{
	private:

	public:
		void Update(const SDL_Event &event) noexcept
		{

		}
	};

	enum struct KeyCode : std::underlying_type_t<SDL_KeyCode>
	{
		Esc = SDLK_ESCAPE,
		Space = SDLK_SPACE,
		Zero = SDLK_0,
		One = SDLK_1,
		Two = SDLK_2,
		Three = SDLK_3,
		Four = SDLK_4,
		Five = SDLK_5,
		Six = SDLK_6,
		Seven = SDLK_7,
		Eight = SDLK_8,
		Nine = SDLK_9,
		A = SDLK_a,
		B = SDLK_b,
		C = SDLK_c,
		D = SDLK_d,
		E = SDLK_e,
		F = SDLK_f,
		G = SDLK_g,
		H = SDLK_h,
		I = SDLK_i,
		J = SDLK_j,
		K = SDLK_k,
		L = SDLK_l,
		M = SDLK_m,
		N = SDLK_n,
		O = SDLK_o,
		P = SDLK_p,
		Q = SDLK_q,
		R = SDLK_r,
		S = SDLK_s,
		T = SDLK_t,
		U = SDLK_u,
		V = SDLK_v,
		W = SDLK_w,
		X = SDLK_x,
		Y = SDLK_y,
		Z = SDLK_z,
		F1 = SDLK_F1,
		F2 = SDLK_F2,
		F3 = SDLK_F3,
		F4 = SDLK_F4,
		F5 = SDLK_F5,
		F6 = SDLK_F6,
		F7 = SDLK_F7,
		F8 = SDLK_F8,
		F9 = SDLK_F9,
		F10 = SDLK_F10,
		F11 = SDLK_F11,
		F12 = SDLK_F12,
		Right = SDLK_RIGHT,
		Left = SDLK_LEFT,
		Down = SDLK_DOWN,
		Up = SDLK_UP,
		F13 = SDLK_F13,
		F14 = SDLK_F14,
		F15 = SDLK_F15,
		F16 = SDLK_F16,
		F17 = SDLK_F17,
		F18 = SDLK_F18,
		F19 = SDLK_F19,
		F20 = SDLK_F20,
		F21 = SDLK_F21,
		F22 = SDLK_F22,
		F23 = SDLK_F23,
		F24 = SDLK_F24,
		LeftShift = SDLK_LSHIFT,
		RightShift = SDLK_RSHIFT,
	};
	
	bool Held(const SDL_Event &event, KeyCode keyCode) noexcept
	{
		return event.type == SDL_KEYDOWN && event.key.keysym.sym == static_cast<SDL_Keycode>(keyCode);
	}

	bool Released(const SDL_Event &event, KeyCode keyCode) noexcept
	{
		return event.type == SDL_KEYUP && event.key.keysym.sym == static_cast<SDL_Keycode>(keyCode);
	}

	int CreateWindowAndRenderer(int width, int height, Uint32 windowFlags, Window &window, Renderer &renderer)
	{
		return SDL_CreateWindowAndRenderer(width, height, windowFlags, &window.window, &renderer.renderer);
	}
}

namespace Lib::Sdl::Text
{
	using namespace Lib::Sdl;

	struct Font final
	{
	private:
		TTF_Font *font;

	public:
		Font() noexcept = default;
		Font(TTF_Font *font) noexcept : font(font) {}
		Font(const string &fileName, int size) : font(TTF_OpenFont(fileName.c_str(), size)) {}
		Font(Font &&other) noexcept : font(std::exchange(other.font, nullptr)) {}

		Font &operator=(Font &&other) noexcept
		{
			font = std::exchange(other.font, nullptr);
			return *this;
		}

		TTF_Font *GetFont() const noexcept
		{
			return font;
		}

		string GetFontName() const
		{
			return string(TTF_FontFaceFamilyName(font));
		}

		int GetFontWrappedAlign() const
		{
			return TTF_GetFontWrappedAlign(font);
		}

		void SetFontWrappedAlign(int align)
		{
			TTF_SetFontWrappedAlign(font, align);
		}

		RectSize SizeText(const string &text) const
		{
			RectSize result = {};
			TTF_SizeText(font, text.c_str(), &result.width, &result.height);
			return result;
		}

		RectSize SizeUtf8(const string &text) const
		{
			RectSize result = {};
			TTF_SizeUTF8(font, text.c_str(), &result.width, &result.height);
			return result;
		}

		void RenderText(const string &text, SDL_Color color, SDL_Renderer *renderer, const SDL_Rect *srcRect, const SDL_Rect *dstRect) const
		{
			Surface surface = Surface(TTF_RenderText_Blended(font, text.c_str(), color));
			Texture texture = Texture(renderer, surface);
			SDL_RenderCopy(renderer, texture, srcRect, dstRect);
		}

		void RenderUtf8(const string &text, SDL_Color color, SDL_Renderer *renderer, const SDL_Rect *srcRect, const SDL_Rect *dstRect) const
		{
			Surface surface = Surface(TTF_RenderUTF8_Blended(font, text.c_str(), color));
			Texture texture = Texture(renderer, surface);
			SDL_RenderCopy(renderer, texture, srcRect, dstRect);
		}

		void RenderText(const string &text, SDL_Color color, SDL_Renderer *renderer, Nullable<const SDL_Rect> srcRect, Nullable<const SDL_Rect> dstRect) const
		{
			RenderText(text, color, renderer, srcRect.operator->(), dstRect.operator->());
		}

		operator TTF_Font *() const noexcept
		{
			return font;
		}

		~Font()
		{
			TTF_CloseFont(font);
		}
	};
}

#endif // !SDL_LIB_DEFINED
