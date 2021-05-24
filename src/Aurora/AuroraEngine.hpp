#pragma once

#include <memory>
#include <vector>
#include <iostream>
#include <thread>

#include "App/Window.hpp"
#include "App/WindowGameContext.hpp"

#include "Assets/AssetManager.hpp"
#include "Sound/SoundSystem.hpp"
#include "Graphics/UI/UIRenderer.hpp"

namespace Diligent
{
	class ImGuiImplDiligent;
}

namespace Aurora
{
	using namespace Sound;

	class AuroraEngine
	{
	private:
		static bool IsInitialized;
		static bool IsRunning;
		static std::vector<WindowGameContext_ptr> GameContexts;
		static std::map<std::thread::id, WindowGameContext_ptr> GameContextsByThread;

		static std::unique_ptr<Diligent::ImGuiImplDiligent> ImGuiImpl;
	public:
		static IRendererInterface* RenderDevice;
		static AssetManager_ptr AssetManager;
#ifdef FMOD_SUPPORTED
		static SoundSystem_ptr SoundSystem;
#endif
		static UIRenderer_ptr UI_Renderer;
	public:
		static void Init();
		static int Run();

		static WindowGameContext_ptr AddWindow(const WindowGameContext_ptr& gameContext, const Window_ptr & window, const WindowDefinition& windowDef, bool showImmediately = true);
		static const std::vector<WindowGameContext_ptr>& GetGameContexts();
		static WindowGameContext_ptr GetCurrentThreadContext();
	public:
		template<class GameContext>
		static std::shared_ptr<WindowGameContext> AddWindow(int width, int height, const String& title, bool showImmediately = true)
		{
			if(!IsInitialized) {
				std::cerr << "Cannot add window(" << title << ") ! Engine not initialized." << std::endl;
				return nullptr;
			}

			WindowDefinition windowDefinition = {};
			windowDefinition.Width = width;
			windowDefinition.Height = height;
			windowDefinition.HasOSWindowBorder = true;
			windowDefinition.Title = title;
			windowDefinition.Maximized = true;

			Window_ptr window = std::make_shared<Window>();
			return AddWindow(std::make_shared<GameContext>(window), window, windowDefinition, showImmediately);
		}
	private:
		static bool CreateSwapChain(const Window_ptr& window, const SwapChainDesc& desc, ISwapChain_ptr& swapChain);
		static void joystick_callback(int jid, int event);
	public:
		static void Play2DSound(const String& path, float volume = 1.0f, float pitch = 1.0f);
	};

#define ASM AuroraEngine::AssetManager
}
