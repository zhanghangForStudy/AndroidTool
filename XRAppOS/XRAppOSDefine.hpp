//
//  XRAppOSDefine.hpp
//  XRAppOS
//
//  Created by tsia on 2022/5/6.
//

#ifndef XRAppOSDefine_hpp
#define XRAppOSDefine_hpp

#include <stdio.h>
#include <stdint.h>


namespace XRAppOS {

struct Point2D {
    float x;
    float y;
    Point2D(float x, float y) :x(x), y(y) {}
};

struct Point3D {
    float x;
    float y;
    float z;
};

enum ImageFormat {
    RGBA     = 0,
    RGB      = 1,
    BGR      = 2,
    GRAY     = 3,
    BGRA     = 4,
    YCrCb    = 5,
    YUV      = 6,
    HSV      = 7,
    XYZ      = 8,
    BGR555   = 9,
    BGR565   = 10,
    YUV_NV21 = 11,
    YUV_NV12 = 12,
    YUV_I420 = 13,
    HSV_FULL = 14,
};


}

#endif /* XRAppOSDefine_hpp */
