#pragma once

#include <set>

#include "Aurora/Core/Types.hpp"
#include "Aurora/Core/String.hpp"
#include "Aurora/Tools/robin_hood.h"
#include "Base/Format.hpp"
#include "Base/Texture.hpp"
#include "Base/ShaderBase.hpp"
#include "Base/Sampler.hpp"
#include "Base/Buffer.hpp"
#include "BufferCache.hpp"

namespace Aurora
{
	class IRenderDevice;

	class RenderManager;

	namespace Samplers
	{
		extern Sampler_ptr ClampClampLinearLinear;
		extern Sampler_ptr WrapWrapLinearLinear;

		extern Sampler_ptr ClampClampNearestNearest;
		extern Sampler_ptr WrapWrapNearestNearest;
	}

	struct RTCacheSort
	{
		uint Width;
		uint Height;
		GraphicsFormat Format;
		EDimensionType DimensionType;
		uint MipLevels;
		uint DepthOrArraySize;
		TextureDesc::EUsage Usage;

		uint Handle;

		[[nodiscard]] int compareNH(const RTCacheSort &other) const
		{
			if (Width < other.Width) return -1;
			if (Width > other.Width) return 1;
			if (Height < other.Height) return -1;
			if (Height > other.Height) return 1;

			if ((uint8_t) Format < (uint8_t) other.Format) return -1;
			if ((uint8_t) Format > (uint8_t) other.Format) return 1;

			if (DimensionType < other.DimensionType) return -1;
			if (DimensionType > other.DimensionType) return 1;

			if (MipLevels < other.MipLevels) return -1;
			if (MipLevels > other.MipLevels) return 1;

			if (DepthOrArraySize < other.DepthOrArraySize) return -1;
			if (DepthOrArraySize > other.DepthOrArraySize) return 1;

			if ((uint8_t) Usage < (uint8_t) other.Usage) return -1;
			if ((uint8_t) Usage > (uint8_t) other.Usage) return 1;

			return 0;
		}

		[[nodiscard]] int compare(const RTCacheSort &other) const
		{
			int comp = compareNH(other);
			if (comp) return comp;
			if (Handle < other.Handle) return -1;
			if (Handle > other.Handle) return 1;
			return 0;
		}

		bool operator<(const RTCacheSort &other) const
		{ return compare(other) < 0; }

		bool operator>(const RTCacheSort &other) const
		{ return compare(other) > 0; }

		bool operator==(const RTCacheSort &other) const
		{ return compare(other) == 0; }
	};

	class TemporalRenderTarget
	{
		friend class RenderManager;

	private:
		Texture_ptr m_Texture;
		RenderManager *m_Manager;
		int m_Index = -1;
	public:
		operator const Texture_ptr &()
		{
			return m_Texture;
		}

		const Texture_ptr &operator->() const
		{
			return m_Texture;
		}

		void Free();
	};

	struct TemporalRenderTargetStorage
	{
		String Name;
		RTCacheSort Cache;
		Texture_ptr Texture;
		double LastUseTime;
	};

	class RenderManager
	{
		friend class TemporalRenderTarget;

	private:
		IRenderDevice *m_RenderDevice;

		Shader_ptr m_BlitShader;
		std::vector<TemporalRenderTargetStorage> m_TemporalRenderTargets;

		BufferCache m_UniformBufferCache;
	public:
		explicit RenderManager(IRenderDevice *renderDevice);

		TemporalRenderTarget CreateTemporalRenderTarget(const String &name, uint width, uint height, GraphicsFormat format, EDimensionType dimensionType = EDimensionType::TYPE_2D, uint mipLevels = 1,
		                                                uint depthOrArraySize = 0, TextureDesc::EUsage usage = TextureDesc::EUsage::Default);

		TemporalRenderTarget CreateTemporalRenderTarget(const String &name, const Vector2i &size, GraphicsFormat format, EDimensionType dimensionType = EDimensionType::TYPE_2D, uint mipLevels = 1,
		                                                uint depthOrArraySize = 0, TextureDesc::EUsage usage = TextureDesc::EUsage::Default)
		{
			return std::move(CreateTemporalRenderTarget(name, size.x, size.y, format, dimensionType, mipLevels, depthOrArraySize, usage));
		}

		Texture_ptr
		CreateRenderTarget(const String &name, uint width, uint height, GraphicsFormat format, EDimensionType dimensionType = EDimensionType::TYPE_2D, uint mipLevels = 1, uint depthOrArraySize = 0,
		                   TextureDesc::EUsage usage = TextureDesc::EUsage::Default);

		Texture_ptr
		CreateRenderTarget(const String &name, const Vector2i &size, GraphicsFormat format, EDimensionType dimensionType = EDimensionType::TYPE_2D, uint mipLevels = 1, uint depthOrArraySize = 0,
		                   TextureDesc::EUsage usage = TextureDesc::EUsage::Default)
		{
			return std::move(CreateRenderTarget(name, size.x, size.y, format, dimensionType, mipLevels, depthOrArraySize, usage));
		}

		void Blit(const Texture_ptr &src, const Texture_ptr &dest);

		void Blit(const Texture_ptr &src)
		{
			Blit(src, nullptr);
		}

		// Not used for now
		BufferCache &GetUniformBufferCache()
		{
			return m_UniformBufferCache;
		}

		void EndFrame();
	};

#define BEGIN_UB(type, name) \
{ \
    VBufferCacheIndex cacheIndex; \
    auto* name = m_RenderManager->GetUniformBufferCache().GetOrMap<type>(sizeof(type) * 1, cacheIndex); \
    if(name) \
    {

#define END_UB(bufferName) \
    m_RenderManager->GetUniformBufferCache().Unmap(cacheIndex); \
    }          \
    drawCallState.BindUniformBuffer(#bufferName, cacheIndex.Buffer, cacheIndex.Offset, cacheIndex.Size);}

}