#ifndef _BASE_TYPES_H
#define _BASE_TYPES_H

#include <memory>

#include <Eigen/Geometry>
#include <Eigen/Dense>

typedef Eigen::Vector2f vector2f;
typedef Eigen::Vector3f vector3f;
typedef Eigen::Vector4f vector4f;
typedef Eigen::Affine3f affine3f;
typedef Eigen::Matrix2f matrix2f;
typedef Eigen::Matrix3f matrix3f;
typedef Eigen::Matrix4f matrix4f;
typedef Eigen::Translation3f translation3f;
typedef Eigen::Quaternionf quaternionf;
typedef Eigen::AngleAxisf angle_axisf;

typedef Eigen::AlignedBox2i rect2d;
typedef Eigen::AlignedBox2f box2f;

// potential need to optimize this??? hope not!
template<typename C>
using enable_shared_from_this = std::enable_shared_from_this<C>;

template<typename C>
using shared_ptr = std::shared_ptr<C>;

template<typename C>
using weak_ptr = std::weak_ptr<C>;

#endif