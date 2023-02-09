#ifndef AR_SCENE_RENDER_RENDERER_H
#define AR_SCENE_RENDER_RENDERER_H

#include <XRAppOS/XRAppOSCommon.h>
#include <memory>
#include <unordered_map>
#include "xr_render_scene.h"
#include "xr_render_material.h"
#include "xr_render_geometry.h"
#include "xr_render_node.h"
#include "xr_render_camera.h"
#include "xr_render_light.h"
//#include "xr_3d_model.hpp"

#ifdef __ANDROID__

#include <jni.h>

#endif

// 此头文件，定义渲染器的概念
namespace XRAppOS {
    using XRRendererCreator = std::function<std::shared_ptr<XRRenderer>()>;

//    using XRRendererNodeCallback = std::function<void(int resultCode,
//                                                      std::shared_ptr<const XR3DModel> modelInfo,
//                                                      std::shared_ptr<XRRenderNode>)>;

    // AR场景渲染器
    // 其子类表示具体的渲染引擎：
    //  XRFilamentRenderer : [待实现]filament引擎实现的XR渲染器
    //  XRNeuralRenderer   : 神经渲染引擎实现的XR渲染器
    //  XRAceTinyRenderer  : AceTiny渲染引擎实现的XR渲染器
    class XRRenderer {
    public:
#ifdef __ANDROID__
        // 方便子类调用Java相关的方法
        static JavaVM *global_java_vm_;
#endif

        // 渲染流程相关的公共方法
        /**
         * 初始化渲染引擎；
         * 可能为异步方法，
         * 通过ARSRRenderer::isInited方法来确定是否初始化成功；
         *
         * @param scene_width
         * @param scene_height
         * @param engine_init_resource_path
         * @param sharedGLContext
         * @return
         */
        virtual bool initRenderer(const size_t scene_width,
                                  const size_t scene_height,
                                  std::string engine_init_resource_path = "",
                                  void *sharedGLContext = nullptr) = 0;

        bool isInited() {
            return arsr_scene != nullptr;
        }

        virtual /**GlTexture*/ void render() = 0;

        virtual void destroyRender() = 0;

        // 创建AR渲染统一API对象的相关API
        inline virtual std::shared_ptr<XRRenderMaterial>
        loadARSRMaterialFromUri(const std::string &uri) {
            return nullptr;
        }

        inline virtual std::shared_ptr<XRRenderGeometry>
        loadARSRGeometryFromUri(const std::string &uri) {
            return nullptr;
        }

//        inline virtual std::shared_ptr<XRRenderNode>
//        loadARSRNodeFromUri(const XRAppOS::XR3DModel &model_info) {
//            return nullptr;
//        }
//
//        inline virtual void
//        loadARSRNodeFromUriAsync(std::shared_ptr<const XR3DModel> model_info,
//                                 XRRendererNodeCallback callback) {
//
//        }

        inline virtual std::shared_ptr<XRRenderScene>
        loadARSRSceneFromUri(const std::string &scene_file_path) {
            return nullptr;
        }

        inline virtual std::shared_ptr<XRRenderNode>
        createARSRNode(const std::string &_name = "") {
            return std::shared_ptr<XRRenderNode>(new XRRenderNode(_name));
        }

        inline virtual std::shared_ptr<XRRenderCamera>
        createARSRCamera() {
            return nullptr;
        }

        inline virtual std::shared_ptr<XRRenderLight>
        createARSRLight(XRRenderLightType type = IBL_TYPE,
                        std::string ibl_path = "/textures/environment",
                        float ibl_intensity = 3000.0f) {
            return std::shared_ptr<XRRenderLight>(new XRRenderLight(type));
        }

        inline virtual std::shared_ptr<XRRenderGeometry>
        createARSRGeometry() {
            return std::shared_ptr<XRRenderGeometry>(new XRRenderGeometry);
        }

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

        static void XRAPPOS_PUBLIC
        RegisterARSRRenderer(const std::string &name, XRRendererCreator renderer);

        static std::shared_ptr<XRRenderer> XRAPPOS_PUBLIC GenerateARSRRenderer(std::string &name);

//        inline std::shared_ptr<XRAppOS::GlCalculatorHelper> GetGpuHelper() {
//            return gpu_helper;
//        }

//        inline void SetGpuHelper(std::shared_ptr<XRAppOS::GlCalculatorHelper> _gpu_helper_ptr) {
//            gpu_helper = _gpu_helper_ptr;
//        }

        inline const std::shared_ptr<XRRenderScene> GetARSRScene() {
            return arsr_scene;
        }

        inline void resetARSRScene() {
            arsr_scene = nullptr;
        }

    protected:
        std::shared_ptr<XRRenderScene> arsr_scene = nullptr;
//        std::shared_ptr<XRAppOS::GlCalculatorHelper> gpu_helper = nullptr;

        inline virtual bool
        UpdateNodePropertyInRenderer(const std::string &property_name,
                                     std::shared_ptr<const void> property_value) {
            return true;
        }

    private:
        static std::unordered_map<std::string, XRRendererCreator> all_arsr_renders;

        // 渲染器相关参数
        std::map<const std::string, std::shared_ptr<const void>> node_property_map;
    };

}

#endif// end AR_SCENE_RENDER_RENDERER_H
