#ifndef AR_SCENE_RENDER_SCENE_H
#define AR_SCENE_RENDER_SCENE_H

#include <string>
#include <XRAppOS/XRAppOSCommon.h>
#include <map>
#include <XRAppOS/protocol/XRValueBox.hpp>
// 此头文件，定义场景的概念
namespace XRAppOS {
    class XRRenderer;

    class XRRenderNode;

    using XRAppOSScreenShotCallback = std::function<void(int resultCode, std::shared_ptr<XRImage> xrImage)>;

    // AR渲染场景
    class XRRenderScene : public std::enable_shared_from_this<XRRenderScene> {
    public:
        ~XRRenderScene() {

        }

        std::shared_ptr<XRRenderNode> root_node = nullptr;

        std::shared_ptr<XRRenderNode> camera_node = nullptr;

        void SetViewPort(size_t width, size_t height) {
            m_width = width;
            m_height = height;

            OnViewPortUpdated();
        }

        const size_t &Width() {
            return m_width;
        }

        const size_t &Height() {
            return m_height;
        }

        XRAPPOS_PUBLIC void resetScene();

        inline bool
        UpdateNodeProperty(const std::string &property_name,
                           std::shared_ptr<const void> property_value) {
            if (property_name.empty()) {
                return false;
            }
            if (UpdateNodePropertyInRenderer(property_name, property_value)) {
                node_property_map[property_name] = property_value;
            }
            return true;
        }

        inline std::shared_ptr<const void>
        GetNodeProperty(const std::string &property_name, bool reset_value) {
            if (property_name.empty()) {
                return nullptr;
            }

            auto it = node_property_map.find(property_name);
            if (it == node_property_map.end()) {
                return nullptr;
            }
            auto result = it->second;
            if (reset_value) {
                // todo 是否需要再执行一次update方法
                node_property_map.erase(it);
            }
            return result;
        }

    protected:
        inline virtual bool
        UpdateNodePropertyInRenderer(const std::string &property_name,
                                     std::shared_ptr<const void> property_value) {
            return true;
        }

        virtual void OnViewPortUpdated() {

        }

        size_t m_width;
        size_t m_height;

        XRRenderScene() {}

        // 场景相关参数
        std::map<const std::string, std::shared_ptr<const void>> node_property_map;
    };
}
#endif// end AR_SCENE_RENDER_LIGHT_H