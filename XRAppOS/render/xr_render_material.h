#ifndef AR_SCENE_RENDER_MATERIAL_H
#define AR_SCENE_RENDER_MATERIAL_H

#include <string>
#include <map>
// 此头文件，定义渲染材料概念
// 渲染材料，表示某个3D形状(XRRenderGeometry)的外观
namespace XRAppOS {

    class XRRenderer;

    class XRRenderGeometry;

    // 渲染材料
    class XRRenderMaterial {
    public:
        inline ~XRRenderMaterial(){

        }
        // geometry哪个部位的material，用来进行material覆盖
        const int index;

        bool
        UpdateMaterialProperty(const std::string &property_name, const void *property_value);

        inline const void *GetMaterialProperty(const std::string &property_name) {
            if (property_name.empty()) {
                return nullptr;
            }

            return material_property_map[property_name];
        }

        // 返回引擎层的material实例
        // 完全由子类，根据其所处的引擎环境来决定相应的内部实例；
        // 只会由引擎环境内部调用；对上层无实意，禁止上层调用；
        inline virtual const void *GetMaterialInstanceInRenderer() {
            return nullptr;
        }

    protected:
        friend XRRenderGeometry;

        inline virtual bool UpdateMaterialPropertyInRenderer(const std::string &property_name,
                                                             const void *property_value) {
            return true;
        }

        // 只能通过ARSRRenderer或该类子类创建
        XRRenderMaterial(const int _index) : index(_index), material_property_map() {}

    private:
        std::map<const std::string, const void *> material_property_map;
    };
}

#endif// end AR_SCENE_RENDER_MATERIAL_H