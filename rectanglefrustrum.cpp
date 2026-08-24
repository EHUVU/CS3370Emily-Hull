/*
    Emily Hull
*/
#include "rectanglefrustrum.h"
#include <cmath>
#include <stdexcept>

/*
    Passes in reference rectangles and height. Initializes the top and bottom rectangles as well as the height. Checks to see if the height is valid before completing the object construction.
*/
RectangleFrustrum::RectangleFrustrum(const Rectangle& rect1, const Rectangle& rect2, double height) : _rTop(rect1), _rBottom(rect2), _height(height){
    if(height <= 0){
        throw std::invalid_argument("Height cannot be zero or a negative value\n");
    }
}

/*
    Returns the result of the formula to calculate the volume of the Rectangular Frustrum.
*/
double RectangleFrustrum::GetFrustrumVolume(){
    return _height/3 * (_rTop.GetArea() + _rBottom.GetArea() + std::sqrt(_rTop.GetArea() * _rBottom.GetArea()));
}

/*
    Returns the result of the formula to calculate the surface area of the Rectangular Frustrum.
*/
double RectangleFrustrum::GetFrustrumSurfaceArea(){
    return _rTop.GetArea() + _rBottom.GetArea() + _height * (_rTop.GetHeight() + _rBottom.GetHeight()) + _height * (_rTop.GetWidth() + _rBottom.GetWidth());
}