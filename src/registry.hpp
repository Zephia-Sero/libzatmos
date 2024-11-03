#ifndef REGISTRY_HPP
#define REGISTRY_HPP


#include <stdexcept>
#include <string>
#include <unordered_map>

namespace ZAtmos {
template <typename T>
struct ImmutableRegistry {
private:
	std::unordered_map<std::string, T> map;
public:
	inline ImmutableRegistry()
	{}

	inline bool has_key(std::string const &key)
	{
		return map.count(key) > 0;
	}
	// Returns true if key is found.
	inline bool try_cget(std::string const &key, T const *&out)
	{
		if (has_key(key)) {
			out = &map[key];
			return true;
		}
		return false;
	}
	// Throws if key is not in registry.
	inline T const *cget(std::string const &key)
	{
		if (has_key(key))
			return &map[key];
		throw std::invalid_argument("Key '" + key + "' not found in registry");
		return nullptr; // shouldn't be reached but shh
	}
	// Adds key:value to registry
	inline void add(std::string const &key, T const &value)
	{
		if (has_key(key)) {
			throw std::invalid_argument("Key '" + key + "' has already been registered");
			return;
		}
		map[key] = value;
	}

	inline T const *operator[](std::string const &key)
	{
		return cget(key);
	}
};

template <typename T>
struct MutableRegistry : public ImmutableRegistry<T> {
private:
	std::unordered_map<std::string, T> &map = ImmutableRegistry<T>::map;
public:
	inline bool try_get(std::string const &key, T *&out)
	{
		if (ImmutableRegistry<T>::has_key(key)) {
			out = &map[key];
			return true;
		}
		return false;
	}
	inline T *get(std::string const &key)
	{
		if (ImmutableRegistry<T>::has_key(key))
			return &map[key];
		throw std::invalid_argument("Key '" + key + "' not found in registry");
		return nullptr;
	}
	inline void set(std::string const &key, T const &value)
	{
		if (ImmutableRegistry<T>::has_key(key)) {
			map[key] = value;
		} else {
			throw std::invalid_argument("Key '" + key + "' not found in registry");
		}
	}
	inline T *operator[](std::string const &key)
	{
		return get(key);
	}
};
}


#endif
