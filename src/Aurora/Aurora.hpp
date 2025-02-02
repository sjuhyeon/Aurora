#pragma once

#include "App/IWindow.hpp"
#include "App/AppContext.hpp"
#include "Engine.hpp"

namespace Aurora
{
	class IRenderDevice;
	class RenderManager;
	class ResourceManager;

	namespace Input
	{
		class IManager;
	}

	class AU_API AuroraEngine
	{
	private:
		IWindow* m_Window;
		ISwapChain* m_SwapChain;
		IRenderDevice* m_RenderDevice;
		RenderManager* m_RenderManager;
		ResourceManager* m_ResourceManager;
		Input::IManager* m_InputManager;
		RmlUI* m_RmlUI;
		VgRender* m_VgRender;
#ifdef NEWTON
		PhysicsWorld* m_PhysicsWorld;
#endif

		AppContext* m_AppContext;
	public:
		AuroraEngine();
		~AuroraEngine();

		void Init(AppContext* appContext, WindowDefinition& windowDefinition);
		void Run();
	};
}
