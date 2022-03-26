#pragma once

#include <utility>
#include "../../wde.hpp"

namespace wde::resource {
	class Resource {
		public:
			enum ResourceType {
				MATERIAL, SHADER, IMAGE, MESH
			};

			// Core methods
			explicit Resource(std::string path, ResourceType type) : _path(std::move(path)), _type(type) {}
			virtual ~Resource() = default;


			// Getters and setters
			std::string getPath() const { return _path; }
			ResourceType getType() const { return _type; }


		protected:
			std::string _path;
			ResourceType _type;
	};
}

