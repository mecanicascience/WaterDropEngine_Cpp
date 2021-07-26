#pragma once

#include "../../../../wde.hpp"
#include "../../../WdeCommon/WdeMaths/Color.hpp"

namespace wde::renderEngine {
	/**
	 * Class that describes an attachment in a render pass that represents an object in the render pipeline.
	 */
	class RenderPassAttachment {
		public:
			/** Type of the attachment (currently, only swapchain attachments supported) */
			enum class Type {
				Swapchain, Depth
			};

			/**
			 * Creates a new attachment that represents a object in the render pipeline of the render pass.
			 * @param The index the attachment is bound to in the render pass
			 * @param uniqueName The unique name of the object for all render passes
			 * @param type The attachment type
			 */
            RenderPassAttachment(uint32_t bindingIndex, std::string uniqueName, Type type, const Color &clearColor = Color(0, 0, 0))
				: _bindingIndex(bindingIndex), _uniqueName(std::move(uniqueName)), _type(type), _clearColor(clearColor) {}


			// Getters and setters
			uint32_t getBindingIndex() const { return _bindingIndex; }
			const std::string& getUniqueName() const { return _uniqueName; }
			Type getType() const { return _type; }
			const Color getClearColor() const { return _clearColor; }


		private:
			uint32_t _bindingIndex {};
			std::string _uniqueName {};
			Type _type;
			Color _clearColor;
	};
}
