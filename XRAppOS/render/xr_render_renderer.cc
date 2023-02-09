#include "xr_render_renderer.h"
//#include "gpu/gl_context.h"

namespace XRAppOS {
    std::unordered_map<std::string, XRRendererCreator> XRRenderer::all_arsr_renders;
#ifdef __ANDROID__
    // 方便子类调用Java相关的方法
    JavaVM *XRRenderer::global_java_vm_ = nullptr;
#endif

    void XRRenderer::RegisterARSRRenderer(const std::string &name, XRRendererCreator renderer) {
        auto it = all_arsr_renders.find(name);
        if (it == all_arsr_renders.end()) {
            all_arsr_renders.emplace(name, renderer);
        }
    }

    std::shared_ptr<XRRenderer> XRRenderer::GenerateARSRRenderer(std::string &name) {
        auto it = all_arsr_renders.find(name);
        if (it == all_arsr_renders.end()) {
            return nullptr;
        } else {
            return it->second();
        }
    }
}
