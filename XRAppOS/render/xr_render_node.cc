#include "xr_render_node.h"
#include "xr_render_light.h"
//#include "ar_scene_render_material_property.h"
#include "xr_render_geometry.h"
#include "xr_render_camera.h"
#include <algorithm>
#include <memory>

namespace XRAppOS {

    XRRenderNode::~XRRenderNode() {
        if (attached_light) {
            attached_light.reset();
            attached_light = nullptr;
        }
        if (attached_camera) {
            attached_camera.reset();
            attached_camera = nullptr;
        }
        if (attached_geometry) {
            attached_geometry.reset();
            attached_geometry = nullptr;
        }

        if (!children_node.empty()) {
            auto iter = children_node.begin();
            while (iter != children_node.end()) {
                (*iter).reset();
                iter++;
            }
            children_node.clear();
        }

        if (!gpu_filters.empty()) {
            auto iter = gpu_filters.begin();
            while (iter != gpu_filters.end()) {
                (*iter).reset();
                iter++;
            }
            gpu_filters.clear();
        }
    }

    bool XRRenderNode::attachGeometry(const std::shared_ptr<XRRenderGeometry> geometry) {
        if (!geometry) {
            return false;
        }

        if (!geometry->onAttachedToNode()) {
            return false;
        }

        if (attached_geometry) {
            attached_geometry->onDetachedFromNode();
            attached_geometry.reset();
        }

        attached_geometry = geometry;
        return true;
    }

    bool XRRenderNode::attachCamera(const std::shared_ptr<XRRenderCamera> camera) {
        if (!camera) {
            return false;
        }

        auto shared_this = shared_from_this();
        if (!camera->onAttachedToNode(shared_this)) {
            return false;
        }

        if (attached_camera) {
            attached_camera->onDetachedFromNode();
            attached_camera.reset();
        }
        attached_camera = camera;
        return true;
    }

    bool XRRenderNode::attachLight(const std::shared_ptr<XRRenderLight> light) {
        if (!light) {
            return false;
        }

        if (!light->onAttachedToNode()) {
            return false;
        }

        if (attached_light) {
            attached_light->onDetachedFromNode();
            attached_light.reset();
        }

        attached_light = light;
        return true;
    }

    bool XRRenderNode::rotationLight(const glm::mat4 *rotation) {
        if (!rotation || attached_light) {
            return false;
        }
        // 更新光照的旋转属性
        return attached_light->UpdateLightProperty("rotation", rotation);
    }

    const glm::vec4 *XRRenderNode::GetRotationOfLight() {
        if (!attached_light) {
            return nullptr;
        }
        void *tmp = const_cast<void *>(attached_light->GetLightProperty("rotation"));
        return static_cast<const glm::vec4 *>(tmp);
    }

    void XRRenderNode::addToParentNode(const std::shared_ptr<XRRenderNode> parent,
                                       const std::shared_ptr<XRRenderNode> child, size_t index) {
        if (!parent || !child) {
            return;
        }

        if (!(child->parent_node.expired())) {
            auto currentParent = child->parent_node.lock();
            if (currentParent->name == parent->name) {
                return;
            }
            currentParent->removeChildNode(child);
        }

        if (!child->onAttachedToParent(parent)) {
            return;
        }

        parent->children_node.emplace_back(child);
        child->parent_node = parent;
    }

    void XRRenderNode::removeChildNode(const std::shared_ptr<XRRenderNode> child) {
        if (!child) {
            return;
        }

        auto child_iter = std::find(children_node.begin(), children_node.end(), child);
        if (child_iter == children_node.end()) {
            return;
        }

        children_node.erase(child_iter);

        (*child_iter)->onDetachedFromParent();
    }

    void XRRenderNode::removeAllChildren(bool recursion) {
        if (children_node.empty()) {
            return;
        }

        auto iter = children_node.begin();
        while (iter != children_node.end()) {
            if (recursion) {
                (*iter)->removeAllChildren(true);
            }
            children_node.erase(iter);
        }
    }

    void XRRenderNode::detachFromParent() {
        if (parent_node.expired()) {
            return;
        }

        std::shared_ptr<XRRenderNode> parent = parent_node.lock();
        if (parent->children_node.empty()) {
            return;
        }
        auto child_iter = parent->children_node.begin();
        while (child_iter != parent->children_node.end()) {
            if ((*child_iter).get() == this) {
                parent->children_node.erase(child_iter);

                (*child_iter)->onDetachedFromParent();
                return;
            }
            child_iter++;
        }
    }

    std::shared_ptr<XRRenderNode>
    XRRenderNode::FindChildNodeByName(const std::string name, bool recursion) {
        if (children_node.empty()) {
            return nullptr;
        }

        auto child_it = children_node.cbegin();
        while (child_it != children_node.cend()) {
            if ((*child_it)->name == name) {
                return *child_it;
            }

            if (recursion) {
                auto result = (*child_it)->FindChildNodeByName(name, true);
                if (result) {
                    return result;
                }
            }
            child_it++;
        }
        return nullptr;
    }

    bool XRRenderNode::ApplyTransform(const glm::mat4 *transform) {
        if (!transform) {
            return false;
        }

        if (ApplyTransformInRenderer(transform)) {
            transformation_matrix = (*transform);
            return true;
        } else {
            return false;
        }
    }
}