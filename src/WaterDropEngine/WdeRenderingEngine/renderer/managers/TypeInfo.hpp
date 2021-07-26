#pragma once

#include <typeindex>
#include <unordered_map>

namespace wde::renderEngine {
	using TypeID = std::size_t;

	/**
	 * Handles the type (generally of a sub-renderer)
	 * @tparam T
	 */
	template<typename T>
	class TypeInfo {
		public:
            explicit TypeInfo() = delete;

			/**
			 * Get the type ID of K which is a base of T.
			 * @tparam K The type ID K.
			 * @return The type ID.
			 */
			template<typename K, typename = std::enable_if_t<std::is_convertible_v<K*, T*>>>
			static TypeID getTypeID() noexcept {
				std::type_index typeIndex(typeid(K));
				if (auto it = _typeMap.find(typeIndex); it != _typeMap.end())
					return it->second;
				const auto id = nextTypeID();
				_typeMap[typeIndex] = id;
				return id;
			}

		private:
			/**
			 * Get the next type ID for T
			 * @return The next type ID for T.
			 */
			static TypeID nextTypeID() noexcept {
				const auto id = _nextTypeID;
				++_nextTypeID;
				return id;
			}

		// Next type ID for T.
		static TypeID _nextTypeID;
		static std::unordered_map<std::type_index, TypeID> _typeMap;
	};


	// Initializers of static variables
	template<typename K>
	TypeID TypeInfo<K>::_nextTypeID = 0;

	template<typename K>
	std::unordered_map<std::type_index, TypeID> TypeInfo<K>::_typeMap = {};
}
