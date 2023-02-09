#include "xr_render_material.h"

namespace XRAppOS {
    inline bool
    XRRenderMaterial::UpdateMaterialProperty(const std::string &property_name,
                                             const void *property_value) {
        if (property_name.empty() || !property_value) {
            return false;
        }
        if (UpdateMaterialPropertyInRenderer(property_name, property_value)) {
            material_property_map[property_name] = property_value;
        }
        return true;
    }
}