#include "PipelineExample02.hpp"
#include "../../../src/WaterDropEngine/WaterDropEngine.hpp"

namespace examples {
    void PipelineExample02::render(CommandBuffer &commandBuffer) {
        {
            beginRenderPass(0);
                beginRenderSubPass(0);
                    auto& scene = WaterDropEngine::get().getInstance().getScene();
                    std::string lastMeshName;

                    // Fetch every scene materials
                    for (auto& mat : scene.getMaterials()) {
                        // Check if material in correct render pass
                        if (mat->getRenderStage() != std::pair<int, int>(0, 0))
                            continue;

                        // Bind material
                        mat->bind(commandBuffer);

                        // Fetch every game object with this material
                        for (int goID : mat->getGameObjectsIds()) {
                            // Check if GO has a mesh
                            auto& mesh = scene.getGameObject(goID).getModule<scene::MeshRendererModule>()->getMesh();
                            if (mesh != nullptr) {
                                if (mesh->getName() != lastMeshName) { // Mesh different from previous one
                                    // Bind mesh
                                    mesh->bind(commandBuffer);
                                    lastMeshName = mesh->getName();
                                }

                                // Render mesh
                                mesh->render();
                            }
                        }
                    }

                    /**
                     * TODO
                     * Get active camera (scene -> activeCameraID)
                     * [TODO => do culling writes GO ids to buffer]
                     * Render GO (for each material, bind pipeline + render each game objects corresponding to that pipeline)
                     */
                endRenderSubPass();

                beginRenderSubPass(1);
                    // Render GUI
                    gui::WdeGUI::render(commandBuffer);
                endRenderSubPass();
            endRenderPass();
        }
    }
}
