#include <iostream>
#include "BoundingBox.h"

/*
    BoundingBox is a 3d rectangular space defined by two opposite corners.  The corners will typically be defined so that
    the entirety of another object is contained inside of it. This allows a quick calculation of whether a ray intersects
    this object or not.
*/

BoundingBox::BoundingBox(Eigen::Vector3d min_corn, Eigen::Vector3d max_corn){
    // Non recursive constructor, for objects that only need a bounding box
    min_corner = min_corn;
    max_corner = max_corn;
}

BoundingBox::BoundingBox(){}

Eigen::Vector3d BoundingBox::get_min_corner(){
    return min_corner;
}

Eigen::Vector3d BoundingBox::get_max_corner(){
    return max_corner;
}

bool BoundingBox::ray_intersects(Ray& ray){
    double not_used, not_used2;
    return ray_intersects(ray, not_used, not_used2);
}

bool BoundingBox::ray_intersects(Ray& ray, double &tmin, double &tmax){
    const Eigen::Vector3d& ray_pt = ray.get_point();
    const Eigen::Vector3d& ray_dir = ray.get_dir();
    tmin = (min_corner(0) - ray_pt(0)) / ray_dir(0);
    tmax = (max_corner(0) - ray_pt(0)) / ray_dir(0);

    if (tmin > tmax) std::swap(tmin, tmax);

    double tymin = (min_corner(1) - ray_pt(1)) / ray_dir(1);
    double tymax = (max_corner(1) - ray_pt(1)) / ray_dir(1);

    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;

    if (tymax < tmax)
        tmax = tymax;

    double tzmin = (min_corner(2) - ray_pt(2)) / ray_dir(2);
    double tzmax = (max_corner(2) - ray_pt(2)) / ray_dir(2);

    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    if (tzmin > tmin)
        tmin = tzmin;

    if (tzmax < tmax)
        tmax = tzmax;

    return true;
}
