#ifndef _CHAOS3D_PLATFORM_DEVICE_H
#define _CHAOS3D_PLATFORM_DEVICE_H

#include <Eigen/Dense>
#include <string>

class render_window;
class render_device;
class render_context;

class device {
public:
    typedef Eigen::Vector2i vector2i;
    
public:
    static float best_pixel_ratio();
    static vector2i screen_size();
    static std::string device_name();
};

#endif