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
			virtual void drawGUI() {};


			// Getters and setters
			std::string getPath() const { return _path; }
			ResourceType getType() const { return _type; }
			/** @return The name of the resource (default : will return resource path) */
			virtual std::string getName() const { return _path; }

			static std::string getName(const ResourceType& type) {
				switch (type) {
					case MATERIAL:
						return "Material";
					case SHADER:
						return "Shader";
					case IMAGE:
						return "Image";
					case MESH:
						return "Mesh";
				}
				return "";
			}

			static std::string getIcon(const ResourceType& type) {
				switch (type) {
					case MATERIAL:
						return ICON_FA_LAYER_GROUP;
					case SHADER:
						return ICON_FA_FILE_CODE;
					case IMAGE:
						return ICON_FA_IMAGES;
					case MESH:
						return ICON_FA_GHOST;
				}
				return "  ";
			}


		protected:
			std::string _path;
			ResourceType _type;
	};
}

