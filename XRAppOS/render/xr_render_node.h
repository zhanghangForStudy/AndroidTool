#ifndef AR_SCENE_RENDER_NODE_H
#define AR_SCENE_RENDER_NODE_H

#include <string>
#include <vector>
#include <glm/mat4x4.hpp>
#include <map>
#include <XRAppOS/XRAppOSCommon.h>

// 此头文件，定义AR渲染场景节点;
namespace XRAppOS {
    class XRRenderGeometry;

    class XRRenderLight;

    class XRRenderCamera;

    class XRRenderGpuFilter;

    class XRRenderGpuFilter;

    class XRRenderer;

    // AR渲染场景节点，表示3D坐标系中可旋转、缩放的3d位置；
    // 节点本身没有可见的内容，不过可以通过attach(附着)ARSRGeometry的方式，增加可见内容；
    // 此外，节点可以attach ARSRLight的方式，增加AR渲染场景(XRRenderScene)的光源，实现光照效果；
    // 节点也可以attach XRRenderCamera, 增加一个相机功能，以定义AR渲染场景(XRRenderScene)的可见范围；
    class XRAPPOS_PUBLIC XRRenderNode : public std::enable_shared_from_this<XRRenderNode> {
    public:

        std::string name;

        ~XRRenderNode();

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

            // 优先读取引擎中的属性
            auto result_from_render = GetNodePropertyFromRender(property_name);
            if (result_from_render) {
                return result_from_render;
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

        inline std::shared_ptr<XRRenderNode> GetParent() {
            if (parent_node.expired()) {
                return nullptr;
            }

            return parent_node.lock();
        }

        // todo attach系列方法可以抽象为更加通用的方法
        bool attachCamera(const std::shared_ptr<XRRenderCamera> camera);

        inline const std::shared_ptr<XRRenderCamera> GetAttachedCamera() {
            return attached_camera;
        }

        bool attachLight(const std::shared_ptr<XRRenderLight> light);

        inline const std::shared_ptr<XRRenderLight> GetAttachedLight() {
            return attached_light;
        }

        bool attachGeometry(const std::shared_ptr<XRRenderGeometry> geometry);

        static void addToParentNode(const std::shared_ptr<XRRenderNode> parent,
                                    const std::shared_ptr<XRRenderNode> child, size_t index = 0);

        void removeChildNode(const std::shared_ptr<XRRenderNode> child);

        void removeAllChildren(bool recursion = false);

        inline size_t getChildrenNum() {
            return children_node.size();
        }

        inline std::shared_ptr<XRRenderNode> getChildAtIndex(const size_t index) {
            if (index < 0 || index > children_node.size()) {
                return nullptr;
            }
            return children_node[index];
        }

        std::shared_ptr<XRRenderNode>
        FindChildNodeByName(const std::string name, bool recursion = false);

        virtual void detachFromParent();

        void addGpuFilter(const std::shared_ptr<XRRenderGpuFilter> gpu_filter, size_t index = -1);

        void removeGpuFilter(const std::shared_ptr<XRRenderGpuFilter> gpu_filter);

        bool rotationLight(const glm::mat4 *rotation);

        const glm::vec4 *GetRotationOfLight();

        bool ApplyTransform(const glm::mat4 *transform);

        glm::mat4 getTransformMatrix() {
            // 从引擎内部获取最新的位置
            getTransformInRenderer(transformation_matrix);
            return transformation_matrix;
        }

    protected:
        friend XRRenderer;

        // 只能通过ARSRRenderer创建或者子类构建，
        // 确保ARSR统一渲染API在创建时，
        // ARSRRenderer或者子类能够有机会创建特属于渲染引擎相关的部分。
        XRRenderNode(std::string _name = "") : name(_name) {};

        inline virtual bool
        UpdateNodePropertyInRenderer(const std::string &property_name,
                                     std::shared_ptr<const void> property_value) {
            return true;
        }

        inline virtual std::shared_ptr<const void>
        GetNodePropertyFromRender(const std::string &property_name) {
            return nullptr;
        }

        inline virtual bool ApplyTransformInRenderer(const glm::mat4 *transform) {
            return true;
        }

        inline virtual void getTransformInRenderer(glm::mat4 &transform) {

        }

        inline virtual bool onAttachedToParent(const std::shared_ptr<XRRenderNode> parent) {
            return true;
        }

        inline virtual void onDetachedFromParent() {

        }

        // 一个节点，只能attach一个geometry对象
        std::shared_ptr<XRRenderGeometry> attached_geometry = nullptr;

        // 贴合的光源
        std::shared_ptr<XRRenderLight> attached_light = nullptr;

        // 贴合的相机
        std::shared_ptr<XRRenderCamera> attached_camera = nullptr;

        // 使用弱指针执行父节点，避免相互引用所造成的内存泄漏
        std::weak_ptr<XRRenderNode> parent_node;

        std::vector<std::shared_ptr<XRRenderNode>> children_node;

    private:

        // translation * rotation * scale得到的变化矩阵
        // 基于父Node的3D坐标系
        glm::mat4 transformation_matrix;

        // 个性化的gpu filter列表
        std::vector<std::shared_ptr<XRRenderGpuFilter>> gpu_filters;

        // 节点相关参数
        std::map<const std::string, std::shared_ptr<const void>> node_property_map;
    };
}

#endif// end AR_SCENE_RENDER_NODE_H