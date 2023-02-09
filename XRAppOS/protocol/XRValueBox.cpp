#include "XRValueBox.hpp"
#include "XRAppOS/XRAppOSDefine.hpp"

float XRAppOS::XRImage::getChannelByFormat(const XRImage &image) {
    if (image.format == ImageFormat::YUV_NV21) {
        return 1.5f;
    } else if (image.format == ImageFormat::RGB) {
        return 3.0f;
    } else {
        return 0;
    }
}

