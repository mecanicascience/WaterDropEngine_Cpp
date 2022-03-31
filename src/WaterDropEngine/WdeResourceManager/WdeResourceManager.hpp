#pragma once

#include <utility>

#include "../../wde.hpp"
#include "../WdeCore/Core/Module.hpp"
#include "Resource.hpp"

namespace wde::resource {
	/**
	 * Resource manager module
	 */
	class WdeResourceManager : public core::Module {
		public:
			// Core methods
			explicit WdeResourceManager(std::shared_ptr<core::Subject> moduleSubject);
			void tick() override;
			void cleanUp() override;
			void onNotify(const core::Event& event) override;


			// Resources getter
			/**
			 * Load a given resource
			 * @tparam T Type of the resource
			 * @param resource The path to the resource
			 */
			template<typename T>
			std::shared_ptr<T> load(const std::string& resource) {
				if (_resources.contains(resource))
					return std::dynamic_pointer_cast<T>(_resources[resource]);

				// Create resource
				std::shared_ptr<T> baseRes (new T(resource));
				auto res = std::dynamic_pointer_cast<Resource> (baseRes);
				_resources[resource] = res;
				_resourcesByType[res->getType()][resource] = res;
				return baseRes;
			}

			/**
			 * Unload a given resource
			 * @param resource The path to the resource
			 */
			void unload(const std::string& resource) {
				if (!_resources.contains(resource))
					return;

				// Unload resource
				_resourcesByType[_resources.at(resource)->getType()].erase(resource);
				_resources.erase(resource);
			}


			// Getters and setters
			bool& displayResourceGUI() { return _displayResourceGUI; }



		private:
			/** True if the resources GUI editor should be shown */
			bool _displayResourceGUI = false;
			/** Resources list by path */
			std::unordered_map<std::string, std::shared_ptr<Resource>> _resources {};
			/** Resources list by type */
			std::unordered_map<Resource::ResourceType, std::unordered_map<std::string, std::shared_ptr<Resource>>> _resourcesByType {};
	};
}
