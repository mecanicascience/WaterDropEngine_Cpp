#include "DescriptorLayoutCache.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::render {
	// Constructors
	DescriptorLayoutCache::~DescriptorLayoutCache() {
		WDE_PROFILE_FUNCTION();
		auto& device = WaterDropEngine::get().getRender().getInstance().getDevice().getDevice();

		// Delete every descriptor
		for (auto pair : _layoutCache)
			vkDestroyDescriptorSetLayout(device, pair.second, nullptr);
	}


	// Core functions
	VkDescriptorSetLayout DescriptorLayoutCache::createDescriptorLayout(VkDescriptorSetLayoutCreateInfo *info) {
		WDE_PROFILE_FUNCTION();
		auto& device = WaterDropEngine::get().getRender().getInstance().getDevice().getDevice();

		DescriptorLayoutInfo layoutInfo;
		layoutInfo.bindings.reserve(info->bindingCount);
		bool isSorted = true;
		int lastBinding = -1;

		// Copy info structures into this class layout
		for (int i = 0; i < info->bindingCount; i++) {
			layoutInfo.bindings.push_back(info->pBindings[i]);

			// Check if bindings are strictly increasing
			if (info->pBindings[i].binding > lastBinding)
				lastBinding = static_cast<int>(info->pBindings[i].binding);
			else
				isSorted = false;
		}

		// Sort bindings if not in order
		if (!isSorted)
			std::sort(layoutInfo.bindings.begin(), layoutInfo.bindings.end(), [](VkDescriptorSetLayoutBinding& a, VkDescriptorSetLayoutBinding& b){
				return a.binding < b.binding;
			});

		// Try to grab layout from cache
		auto it = _layoutCache.find(layoutInfo);
		if (it != _layoutCache.end()){
			return (*it).second;
		}
		else {
			// Create a new layout
			VkDescriptorSetLayout layout;
			vkCreateDescriptorSetLayout(device, info, nullptr, &layout);

			//add to cache
			_layoutCache[layoutInfo] = layout;
			return layout;
		}
	}


	// Descriptor Layout
	bool DescriptorLayoutCache::DescriptorLayoutInfo::operator==(const DescriptorLayoutCache::DescriptorLayoutInfo &other) const {
		if (other.bindings.size() != bindings.size())
			return false;
		else {
			// Compare (sorted) bindings
			for (int i = 0; i < bindings.size(); i++) {
				if (other.bindings[i].binding != bindings[i].binding)
					return false;
				if (other.bindings[i].descriptorType != bindings[i].descriptorType)
					return false;
				if (other.bindings[i].descriptorCount != bindings[i].descriptorCount)
					return false;
				if (other.bindings[i].stageFlags != bindings[i].stageFlags)
					return false;
			}
			return true;
		}
	}

	size_t DescriptorLayoutCache::DescriptorLayoutInfo::hash() const {
		using std::size_t;
		using std::hash;
		size_t result = hash<size_t>()(bindings.size());

		for (const VkDescriptorSetLayoutBinding& b : bindings) {
			// Pack binding data into single int64
			size_t binding_hash = b.binding | b.descriptorType << 8 | b.descriptorCount << 16 | b.stageFlags << 24;
			// Shuffle packed binding data and xor it with the main hash
			result ^= hash<size_t>()(binding_hash);
		}
		return result;
	}
}
