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
			explicit WdeResourceManager(std::shared_ptr<core::Subject> moduleSubject) : Module(std::move(moduleSubject)) {};
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
				_resources.erase(resource);
			}


		private:
			/** Resources list */
			std::unordered_map<std::string, std::shared_ptr<Resource>> _resources {};
	};
}
