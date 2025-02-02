#include "UUID.hpp"

namespace Aurora
{
	static std::atomic_int32_t GlobalCounter32{0};
	static std::atomic_int64_t GlobalCounter64{0};

	UUID64::UUID64() : UUID64(0) {}
	UUID64::UUID64(uint64_t uuid) : m_UUID(uuid) {}
	UUID64::UUID64(const UUID64& other) : m_UUID(other.m_UUID) {}

	UUID64 UUID64::Generate()
	{
		return UUID64(GlobalCounter64.fetch_add(1) + 1);
	}

	UUID32::UUID32() : m_UUID(0) {}
	UUID32::UUID32(uint32_t uuid) : m_UUID(uuid) {}
	UUID32::UUID32(const UUID32& other) : m_UUID(other.m_UUID) {}

	UUID32 UUID32::Generate()
	{
		return UUID32(GlobalCounter32.fetch_add(1) + 1);
	}
}