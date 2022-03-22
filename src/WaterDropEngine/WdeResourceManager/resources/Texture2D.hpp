#pragma once

#include "../Resource.hpp"

namespace wde::resource {
	class Texture2D : public Resource {
		public:
			explicit Texture2D(const std::string& path) : Resource(path, ResourceType::SHADER) {
				WDE_PROFILE_FUNCTION();
				logger::log(LogLevel::DEBUG, LogChannel::RES) << "Loading texture '" << path << "'." << logger::endl;

				// Load texture 2D

			}


		private:

	};
}

