#pragma once

#include "../../../wde.hpp"

namespace wde::scene {
	class SceneSerializerUtils {
		public:
			// SERIALIZE
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



			// DESERIALIZE
			static glm::vec4 toVec4(json v) { return glm::vec4(float(v[0]), float(v[1]), float(v[2]), float(v[3])); }
			static glm::vec3 toVec3(json v) { return glm::vec3(float(v[0]), float(v[1]), float(v[2])); }
			static glm::vec2 toVec2(json v) { return glm::vec2(float(v[0]), float(v[1])); }

			static glm::mat4 toMat4(json v) {
				glm::mat4 mat;
				for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 4; j++) {
						mat[i][j] = float(v[i][j]);
					}
				}
				return mat;
			}

			static glm::mat3 toMat3(json v) {
				glm::mat3 mat;
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						mat[i][j] = float(v[i][j]);
					}
				}
				return mat;
			}
	};
}

