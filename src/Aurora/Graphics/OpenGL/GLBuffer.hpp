#pragma once

#include "../Base/Buffer.hpp"
#include <glad/glad.h>

namespace Aurora
{
	class GLBuffer : public IBuffer
	{
	private:
		BufferDesc m_Desc;
		GLuint m_Handle;
		GLenum m_BindTarget;
	public:
		GLBuffer(BufferDesc desc, GLuint handle, GLenum bindTarget) {}
		~GLBuffer() override = default;

		[[nodiscard]] const BufferDesc& GetDesc() const noexcept override { return m_Desc; }
	};
}