#pragma once

/*
    Emily Hull
*/

#include "rectangle.h"

/*
    Creates a 3d Rectangular Frustrum object that consists of two reference rectangles for the top and bottom and a height.
    There are two public functions to return the volume and surface area values of the constructed 3D object.
*/
class RectangleFrustrum{
    private:
        Rectangle _rTop, _rBottom;
        double _height;

    public:
        RectangleFrustrum(const Rectangle& rect1, const Rectangle& rect2, double height);

        double GetFrustrumVolume();
        double GetFrustrumSurfaceArea();
};