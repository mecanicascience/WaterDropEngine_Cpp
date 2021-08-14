#pragma once

// Main Engine
#include "wde.hpp"
#include "WaterDropEngine/WaterDropEngine.hpp"

// Core
#include "WaterDropEngine/WdeCore/WdeModule.hpp"
#include "WaterDropEngine/WdeCore/WdeInstance.hpp"

// Rendering engine
#include "WaterDropEngine/WdeRenderingEngine/buffers/BufferUtils.hpp"
#include "WaterDropEngine/WdeRenderingEngine/buffers/Framebuffers.hpp"
#include "WaterDropEngine/WdeRenderingEngine/commands/CommandBuffer.hpp"
#include "WaterDropEngine/WdeRenderingEngine/commands/CommandPool.hpp"
#include "WaterDropEngine/WdeRenderingEngine/core/CoreDevice.hpp"
#include "WaterDropEngine/WdeRenderingEngine/core/CoreDeviceHelper.hpp"
#include "WaterDropEngine/WdeRenderingEngine/core/CoreInstance.hpp"
#include "WaterDropEngine/WdeRenderingEngine/core/CoreWindow.hpp"
#include "WaterDropEngine/WdeRenderingEngine/descriptors/Descriptor.hpp"
#include "WaterDropEngine/WdeRenderingEngine/descriptors/DescriptorPool.hpp"
#include "WaterDropEngine/WdeRenderingEngine/descriptors/DescriptorSet.hpp"
#include "WaterDropEngine/WdeRenderingEngine/images/Image.hpp"
#include "WaterDropEngine/WdeRenderingEngine/images/Image2D.hpp"
#include "WaterDropEngine/WdeRenderingEngine/images/ImageDepth.hpp"
#include "WaterDropEngine/WdeRenderingEngine/images/Texture2D.hpp"
#include "WaterDropEngine/WdeRenderingEngine/pipelines/Pipeline.hpp"
#include "WaterDropEngine/WdeRenderingEngine/pipelines/PipelineGraphics.hpp"
#include "WaterDropEngine/WdeRenderingEngine/pipelines/ShaderUtils.hpp"
#include "WaterDropEngine/WdeRenderingEngine/renderer/descriptors/RenderPassAttachment.hpp"
#include "WaterDropEngine/WdeRenderingEngine/renderer/descriptors/RenderStage.hpp"
#include "WaterDropEngine/WdeRenderingEngine/renderer/descriptors/RenderSubpassDescription.hpp"
#include "WaterDropEngine/WdeRenderingEngine/renderer/descriptors/RenderSubpassType.hpp"
#include "WaterDropEngine/WdeRenderingEngine/renderer/managers/SubrenderersManager.hpp"
#include "WaterDropEngine/WdeRenderingEngine/renderer/managers/TypeInfo.hpp"
#include "WaterDropEngine/WdeRenderingEngine/renderer/passes/RenderPass.hpp"
#include "WaterDropEngine/WdeRenderingEngine/renderer/passes/RenderPassVulkan.hpp"
#include "WaterDropEngine/WdeRenderingEngine/renderer/passes/SwapChain.hpp"
#include "WaterDropEngine/WdeRenderingEngine/renderer/Renderer.hpp"
#include "WaterDropEngine/WdeRenderingEngine/subrenderers/Subrenderer.hpp"
#include "WaterDropEngine/WdeRenderingEngine/subrenderers/GizmoSubrenderer.hpp"
#include "WaterDropEngine/WdeRenderingEngine/subrenderers/GUISubrenderer.hpp"
#include "WaterDropEngine/WdeRenderingEngine/subrenderers/MeshSubrenderer.hpp"
#include "WaterDropEngine/WdeRenderingEngine/subrenderers/filters/FilterSubrenderer.hpp"
#include "WaterDropEngine/WdeRenderingEngine/subrenderers/filters/InvertFilter.hpp"
#include "WaterDropEngine/WdeRenderingEngine/subrenderers/filters/DefaultFilter.hpp"
#include "WaterDropEngine/WdeRenderingEngine/subrenderers/filters/DepthViewerFilter.hpp"
#include "WaterDropEngine/WdeRenderingEngine/utils/FPSUtils.hpp"
#include "WaterDropEngine/WdeRenderingEngine/WdeRenderEngine.hpp"

// Common
#include "WaterDropEngine/WdeCommon/WdeError/WdeException.hpp"
#include "WaterDropEngine/WdeCommon/WdeError/WdeStatus.hpp"
#include "WaterDropEngine/WdeCommon/WdeFiles/WdeFileUtils.hpp"
#include "WaterDropEngine/WdeCommon/WdeLogger/Logger.hpp"
#include "WaterDropEngine/WdeCommon/WdeLogger/Instrumentation.hpp"
#include "WaterDropEngine/WdeCommon/WdeMaths/Color.hpp"
#include "WaterDropEngine/WdeCommon/WdeUtils/Constants.hpp"
#include "WaterDropEngine/WdeCommon/WdeUtils/NonCopyable.hpp"

// Gui
#include "WaterDropEngine/WdeGUI/GUIBar.hpp"
#include "WaterDropEngine/WdeGUI/GUILogger.hpp"
#include "WaterDropEngine/WdeGUI/GUIRenderer.hpp"
#include "WaterDropEngine/WdeGUI/GUITheme.hpp"
#include "WaterDropEngine/WdeGUI/WdeGUI.hpp"

// Input engine
#include "WaterDropEngine/WdeInputEngine/InputController.hpp"
#include "WaterDropEngine/WdeInputEngine/InputManager.hpp"

// Scene
#include "WaterDropEngine/WdeScene/gizmos/Gizmo.hpp"
#include "WaterDropEngine/WdeScene/gizmos/GizmoRenderer.hpp"
#include "WaterDropEngine/WdeScene/materials/ColorMaterial.hpp"
#include "WaterDropEngine/WdeScene/materials/ColorMaterialOutline.hpp"
#include "WaterDropEngine/WdeScene/materials/Material.hpp"
#include "WaterDropEngine/WdeScene/materials/TextureMaterial.hpp"
#include "WaterDropEngine/WdeScene/modules/CameraModule.hpp"
#include "WaterDropEngine/WdeScene/modules/ModelModule.hpp"
#include "WaterDropEngine/WdeScene/modules/Module.hpp"
#include "WaterDropEngine/WdeScene/modules/TransformControllerModule.hpp"
#include "WaterDropEngine/WdeScene/modules/TransformModule.hpp"
#include "WaterDropEngine/WdeScene/objects/models/ModelCube.hpp"
#include "WaterDropEngine/WdeScene/objects/models/ModelLoader.hpp"
#include "WaterDropEngine/WdeScene/objects/GameObject.hpp"
#include "WaterDropEngine/WdeScene/objects/Model.hpp"
#include "WaterDropEngine/WdeScene/Scene.hpp"
#include "WaterDropEngine/WdeScene/WdeSceneManager.hpp"
#include "WaterDropEngine/WdeScene/utils/SceneSerializerUtils.hpp"
