#pragma once

namespace wde::renderEngine {
	/**
	 * Class describing a sub-pass in a render pass
	 */
	class RenderSubpassType {
		public:
			/**
			 * Create a sub-pass in the corresponding render pass
			 * @param bindingIndex The index of the sub-pass
			 * @param attachmentBindingIndices The IDs of the attachments in the render pass that the sub-pass can access to
			 */
			RenderSubpassType(uint32_t bindingIndex, std::vector<uint32_t> attachmentBindingIndices):
					_bindingIndex(bindingIndex), _attachmentBindingIndices(attachmentBindingIndices) {}

			// Getters and setters
			uint32_t getBindingIndex() const { return _bindingIndex; }
			const std::vector<uint32_t> getAttachmentBindingIndices() const { return _attachmentBindingIndices; }

		private:
			uint32_t _bindingIndex {};
			std::vector<uint32_t> _attachmentBindingIndices {};
	};
}
