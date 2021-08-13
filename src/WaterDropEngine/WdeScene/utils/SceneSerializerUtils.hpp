#pragma once

#include "../../../wde.hpp"

namespace wde::scene {
	class SceneSerializerUtils {
		public:
			// Matrix
			/** @return the serialized matrix */
			static json asJson(glm::mat4& matrix) {
				json jMat = json::array();
				for (int i = 0; i < 4; i++) {
					jMat[i] = json::array();
					for (int j = 0; j < 4; j++) {
						jMat[i][j] = matrix[i][j];
					}
				}
				return jMat;
			}

			/** @return the serialized matrix */
			static json asJson(glm::mat3& matrix) {
				json jMat = json::array();
				for (int i = 0; i < 3; i++) {
					jMat[i] = json::array();
					for (int j = 0; j < 3; j++) {
						jMat[i][j] = matrix[i][j];
					}
				}
				return jMat;
			}


			// Vectors
			/** @return the serialized vector */
			static json asJson(glm::vec4& v) {
				return json::array({v.x, v.y, v.z, v.w});
			}

			/** @return the serialized vector */
			static json asJson(glm::vec3& v) {
				return json::array({v.x, v.y, v.z});
			}

			/** @return the serialized vector */
			static json asJson(glm::vec2& v) {
				return json::array({v.x, v.y});
			}
	};
}

