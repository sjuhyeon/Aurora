#pragma once

#include <memory>
#include <cstdint>
#include <string>
#include <utility>
#include <filesystem>
#include <vector>
#include <map>
#include <iostream>
#include "../Logger/Logger.hpp"
#include "Types.hpp"
#include "Library.hpp"

template<typename T>
class SharedFromThis : public std::enable_shared_from_this<T>
{
public:
	virtual ~SharedFromThis() = default;
	template<typename B>
	std::shared_ptr<B> AsShared();

	template<typename B>
	std::shared_ptr<B> AsSharedSafe();

	inline std::shared_ptr<T> ThisShared()
	{
		return this->shared_from_this();
	}
};

template<typename T>
template<typename B>
std::shared_ptr<B> SharedFromThis<T>::AsShared()
{
	return std::static_pointer_cast<B, T>(this->shared_from_this());
}

template<typename T>
template<typename B>
std::shared_ptr<B> SharedFromThis<T>::AsSharedSafe()
{

	return std::dynamic_pointer_cast<B, T>(this->shared_from_this());
}

template<typename T>
inline bool VectorRemove(std::vector<T>& vector, T data)
{
	auto iter = std::find(vector.begin(), vector.end(), data);
	if(iter == vector.end()) {
		return false;
	}

	vector.erase(iter);
	return true;
}

template<typename T>
inline bool VectorContains(std::vector<T>& vector, T&& data)
{
	return std::find(vector.begin(), vector.end(), data) != vector.end();
}

struct dotted : std::numpunct<char> {
	char do_thousands_sep()   const override { return ' '; }  // separate with dots
	std::string do_grouping() const override { return "\3"; } // groups of 3 digits
	static void imbue(std::ostream &os) {
		os.imbue(std::locale(os.getloc(), new dotted));
	}
};

template<typename T>
inline std::string Stringify(T val)
{
	std::ostringstream oss;
	dotted::imbue(oss);
	oss << val;
	return oss.str();
}
