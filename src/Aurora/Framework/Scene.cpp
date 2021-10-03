#include "Scene.hpp"

#include <tuple>
#include "Entity.hpp"

namespace Aurora
{
	Scene::Scene()
	{

	}

	Scene::~Scene()
	{
		m_Registry.clear();
	}

	void Scene::Tick(double delta)
	{
		for(uint i = m_Actors.size(); i --> 0;)
		{
			Actor* actor = m_Actors[i];
			actor->Tick(delta);
		}

		{
			auto view = m_Registry.view<TransformComponent>();
			for (entt::entity entity : view)
			{
				TransformComponent& transformComponent = view.get<TransformComponent>(entity);
				Entity e = Entity(entity, this);
				glm::mat4 transform = GetTransformRelativeToParent(e);
				glm::vec3 translation;
				glm::vec3 rotation;
				glm::vec3 scale;
				DecomposeTransform(transform, translation, rotation, scale);

				auto rotationQuat = Quaternion(rotation);
				transformComponent.Up = glm::normalize(glm::rotate(rotationQuat, glm::vec3(0.0f, 1.0f, 0.0f)));
				transformComponent.Right = glm::normalize(glm::rotate(rotationQuat, glm::vec3(1.0f, 0.0f, 0.0f)));
				transformComponent.Forward = glm::normalize(glm::rotate(rotationQuat, glm::vec3(0.0f, 0.0f, -1.0f)));
			}
		}
	}

	Matrix4 Scene::GetTransformRelativeToParent(Entity entity)
	{
		return entity.GetModelMatrix();
	}

	Entity Scene::CreateEntity(const std::string &name)
	{
		auto entity = Entity{ m_Registry.create(), this };
		auto& idComponent = entity.AddComponent<UUID>();
		idComponent = UUID::Generate();

		entity.AddComponent<TransformComponent>();
		if (!name.empty())
			entity.AddComponent<TagComponent>(name);

		return entity;
	}

	void Scene::DestroyActor(Actor *actor)
	{
		if(actor->m_EntityHandle != entt::null)
		{
			m_Registry.release(actor->m_EntityHandle);
		}

		m_ActorMemory.DeAllocAndUnload<Actor>(actor);
		m_Actors.erase(std::find(m_Actors.begin(), m_Actors.end(), actor));
	}
}