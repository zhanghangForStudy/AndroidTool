#include "xr_render_scene.h"
#include "xr_render_node.h"

namespace XRAppOS {
    void XRRenderScene::resetScene() {
        if (root_node) {
            root_node->removeAllChildren(true);
            root_node.reset();
            root_node = nullptr;
        }

        if (camera_node) {
            camera_node.reset();
            camera_node = nullptr;
        }

        m_width = m_height = 0;
    }
}