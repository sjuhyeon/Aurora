#include "SMaterial.hpp"
#include "Aurora/Engine.hpp"
#include "Aurora/Graphics/RenderManager.hpp"

namespace Aurora
{
	SMaterial::SMaterial(MaterialDefinition* matDef)
		: m_MatDef(matDef), m_UniformData(matDef->m_BaseUniformData)
	{
		for(const auto& it : m_MatDef->m_PassDefs)
		{
			m_PassStates[it.first] = it.second.GetMaterialPassState();
		}
	}

	SMaterial::~SMaterial() = default;

	///////////////////////////////////// RENDER PASS /////////////////////////////////////
#pragma region RenderPass

	void SMaterial::BeginPass(PassType_t pass, DrawCallState& drawState)
	{
		IRenderDevice* renderDevice = GetEngine()->GetRenderDevice();

		MaterialPassDef* passDef = m_MatDef->GetPassDefinition(pass);

		if(passDef == nullptr)
		{
			//TODO: Do something
			AU_LOG_WARNING("Pass ", pass, " not found for for ", m_MatDef->m_Name);
		}

		Shader_ptr shader = passDef->GetShader(m_Macros);

		if(shader == nullptr)
		{
			//TODO: Do something
			AU_LOG_WARNING("Shader for pass", pass, " is null !");
		}

		drawState.Shader = shader;
		renderDevice->SetShader(shader);

		// Set buffers

		for(uint8 uniformBlockIndex : m_MatDef->m_PassUniformBlockMapping[pass])
		{
			const MUniformBlock& block = m_MatDef->m_UniformBlocksDef[uniformBlockIndex];

			VBufferCacheIndex cacheIndex;
			uint8* data = GetEngine()->GetRenderManager()->GetUniformBufferCache().GetOrMap(block.Size, cacheIndex);
			std::memcpy(data, m_UniformData.data() + block.Offset, block.Size);
			GetEngine()->GetRenderManager()->GetUniformBufferCache().Unmap(cacheIndex);
			drawState.BindUniformBuffer(block.Name, cacheIndex.Buffer, cacheIndex.Offset, cacheIndex.Size);
		}

		// Set textures
		for(TTypeID texId : m_MatDef->m_PassTextureMapping[pass])
		{
			MTextureVar* textureVar = GetTextureVar(texId);

			if(textureVar->Texture == nullptr)
				continue;

			drawState.BindTexture(textureVar->InShaderName, textureVar->Texture);
			drawState.BindSampler(textureVar->InShaderName, textureVar->Sampler);
		}

		renderDevice->BindShaderResources(drawState);

		MaterialPassState& passState = m_PassStates[pass];
		renderDevice->SetRasterState(passState.RasterState);
		renderDevice->SetDepthStencilState(passState.DepthStencilState);
		// TODO: Set blend state


	}

	void SMaterial::EndPass(PassType_t pass, DrawCallState& state)
	{
		(void)pass;
		(void)state;

		GetEngine()->GetRenderManager()->GetUniformBufferCache().Reset();
	}

	FRasterState& SMaterial::RasterState(PassType_t pass)
	{
		return m_PassStates[pass].RasterState;
	}

	FDepthStencilState& SMaterial::DepthStencilState(PassType_t pass)
	{
		return m_PassStates[pass].DepthStencilState;
	}

	FBlendState& SMaterial::BlendState(PassType_t pass)
	{
		return m_PassStates[pass].BlendState;
	}

	std::shared_ptr<SMaterial> SMaterial::Clone()
	{
		auto cloned = std::make_shared<SMaterial>(m_MatDef);
		cloned->m_UniformData = m_UniformData;
		cloned->m_PassStates = m_PassStates;
		cloned->m_TextureVars = m_TextureVars;

		m_MatDef->AddRef(cloned);

		return cloned;
	}

#pragma endregion RenderPass

	///////////////////////////////////// BLOCKS /////////////////////////////////////

	uint8* SMaterial::GetBlockMemory(TTypeID id, size_t size)
	{
		MUniformBlock* block = m_MatDef->FindUniformBlock(id);

		if(block == nullptr)
		{
			AU_LOG_WARNING("Requested block ", id, " not found !");
			return nullptr;
		}

		if(block->Size != size)
		{
			AU_LOG_WARNING("Requested block ", id, " has incorrect size, ActualBlockSize(", block->Size, ") vs RequestedBlockSize(", size, ")");
			return nullptr;
		}

		uint8* memoryStart = m_UniformData.data() + block->Offset;

		// TODO: do ifdef or some global var for validation
		if(true) // Validate memory
		{
			uint8* memoryEnd = memoryStart + size;
			uint8* lastMemoryAddress = &m_UniformData[m_UniformData.size() - 1] + 1;

			if(memoryEnd > lastMemoryAddress)
			{
				AU_LOG_WARNING("Memory out of bounds for block ", id, " !");
				return nullptr;
			}
		}

		return memoryStart;
	}

	uint8 *SMaterial::GetVariableMemory(TTypeID varId, size_t size)
	{
		MUniformBlock* block = nullptr;
		MUniformVar* var = m_MatDef->FindUniformVar(varId, &block);

		if(var == nullptr)
		{
			AU_LOG_WARNING("Requested variable ", varId, " not found !");
			return nullptr;
		}

		if(var->Size != size)
		{
			AU_LOG_WARNING("Requested variable ", varId, " has incorrect size, ActualVarSize(", var->Size, ") vs RequestedVarSize(", size, ")");
			return nullptr;
		}

		uint8* blockMemoryStart = m_UniformData.data() + block->Offset;
		uint8* varMemoryStart = blockMemoryStart + var->Offset;

		// TODO: do ifdef or some global var for validation
		if(true) // Validate memory
		{
			uint8* blockMemoryEnd = blockMemoryStart + block->Size;
			uint8* varMemoryEnd = varMemoryStart + var->Size;

			if(varMemoryEnd > blockMemoryEnd)
			{
				AU_LOG_WARNING("Memory out of bounds for variable ", varId, " !");
			}
		}

		return varMemoryStart;
	}

	bool SMaterial::SetVariable(TTypeID varId, uint8 *data, size_t size)
	{
		uint8* varMemoryStart = GetVariableMemory(varId, size);

		if(!varMemoryStart)
		{
			return false;
		}

		std::memcpy(varMemoryStart, data, size);
		return true;
	}

	///////////////////////////////////// TEXTURES /////////////////////////////////////

	MTextureVar *SMaterial::GetTextureVar(TTypeID varId)
	{
		const auto& it = m_TextureVars.find(varId);

		if(it == m_TextureVars.end())
		{
			const auto& it2 = m_MatDef->m_TextureVars.find(varId);

			if(it2 == m_MatDef->m_TextureVars.end())
			{
				AU_LOG_WARNING("Texture property ", varId, " not found !");
				return nullptr;
			}

			return &(m_TextureVars[varId] = it2->second);
		}

		return &it->second;
	}

	bool SMaterial::SetTexture(TTypeID varId, const Texture_ptr& texture)
	{
		MTextureVar* var = GetTextureVar(varId);

		if(var == nullptr)
			return false;

		var->Texture = texture;
		return true;
	}

	bool SMaterial::SetSampler(TTypeID varId, const Sampler_ptr& sampler)
	{
		MTextureVar* var = GetTextureVar(varId);

		if(var == nullptr)
			return false;

		var->Sampler = sampler;
		return true;
	}

	Texture_ptr SMaterial::GetTexture(TTypeID varId)
	{
		MTextureVar* var = GetTextureVar(varId);

		if(var == nullptr)
			return nullptr;

		return var->Texture;
	}

	Sampler_ptr SMaterial::GetSampler(TTypeID varId)
	{
		MTextureVar* var = GetTextureVar(varId);

		if(var == nullptr)
			return nullptr;

		return var->Sampler;
	}
}