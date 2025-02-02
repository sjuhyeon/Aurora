#pragma once

#include <utility>
#include "Aurora/Logger/Logger.hpp"
#include "Aurora/Framework/GameModeBase.hpp"

namespace Aurora
{
	class GameContext
	{

	};

	class AppContext
	{
	private:
		static GameContext* m_GameContext;
		static GameModeBase* m_GameMode;
	public:
		friend class AuroraEngine;

		virtual ~AppContext()
		{
			delete m_GameMode;
			delete m_GameContext;
		}
	private:
		inline void InternalUpdate(double delta)
		{
			if(m_GameMode)
				m_GameMode->Tick(delta);

			Update(delta);
		}
	public:
		virtual void Init() {}
		virtual void Update(double delta) {}
		virtual void Render() {}
		virtual void RenderVg() {}

		template<class T, typename... Args>
		static T* SetGameContext(Args... args)
		{
			if(m_GameContext)
			{
				AU_LOG_FATAL("Cannot change already initialized game context !");
			}

			m_GameContext = new T(std::forward<Args>(args)...);
		}

		template<class T>
		static T* GetGameContext()
		{
			return static_cast<T*>(m_GameContext);
		}

		template<class T, typename... Args>
		static T* SwitchGameMode(Args... args)
		{
			if(m_GameMode)
			{
				m_GameMode->BeginDestroy();
				delete m_GameMode;
			}

			m_GameMode = new T(std::forward<Args>(args)...);
			m_GameMode->BeginPlay();
		}
	};
}