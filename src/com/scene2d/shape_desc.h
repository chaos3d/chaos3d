#ifndef _SHAPE_DESC_H
#define _SHAPE_DESC_H

#include <vector>
#include <type_traits>
#include <Eigen/Geometry>
#include "common/utility.h"

namespace scene2d {
    class collider2d;
    
    struct point {
        float x, y;
    };
    
    struct mass {
        float x, y;
        float rotate; // rotation inertia
        float weight = 0.f;
    };

    // measures in pixels
    struct shape {
        typedef Eigen::AlignedBox2f box2f;
        typedef std::tuple<shape &&> init_t;
        
        virtual ~shape() {};
        
        ATTRIBUTE(float, restitution, 0.f);
        ATTRIBUTE(float, friction, 0.2f);
        ATTRIBUTE(float, density, 1.f);
        ATTRIBUTE(int16_t, collision_group, 0);
        ATTRIBUTE(uint16_t, collision_mask, 0xFFFF);
        ATTRIBUTE(uint16_t, collision_category, 0x1);
        ATTRIBUTE(bool, is_collidable, true);
        
        shape&& forward() {
            return std::forward<shape>(*this);
        }
        
    protected:
        struct shape_def;
        void create_from_shape(collider2d*, shape_def const&) const;
        
    private:
        virtual void append_to(collider2d*, float) const = 0;
        friend class collider2d; // append_to, internal implementation
    };
    
    struct box : public shape{
        float x, y;
        float width, height;
        float angle;
        
        box(float x_, float y_, float w, float h, float angle_ = 0.f)
        : x(x_), y(y_), width(w), height(h), angle(angle_)
        {}

        explicit box(box2f const& bound)
        : x(bound.min().x()), y(bound.min().y()),
        width(bound.sizes().x()), height(bound.sizes().y()),
        angle(0.f) {
        }
        
    private:
        virtual void append_to(collider2d*, float) const override;
    };
    
    struct circle : public shape {
        float x, y;
        float radius;
        
        circle(float x_, float y_, float r)
        : x(x_), y(y_), radius(r)
        {}
        
    private:
        virtual void append_to(collider2d*, float) const override;
    };
    
    struct polygon : public shape {
        std::vector<point> points;
        
        template<class... Args>
        polygon(Args&&... args)
        : points(std::forward<Args>(args)...)
        {}
        
    private:
        virtual void append_to(collider2d*, float) const override;
    };
    
    struct chain : public shape {
        std::vector<point> points;
        
        template<class... Args>
        chain(Args&&... args)
        : points(std::forward<Args>(args)...)
        {}
        
    private:
        virtual void append_to(collider2d*, float) const override;
    };
}

#endif