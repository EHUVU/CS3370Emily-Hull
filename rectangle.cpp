/*
    Emily Hull
*/

#include "rectangle.h"
#include <stdexcept>

/*
    Initializes a Rectangle object and checks to see if the values are valid.
*/
Rectangle::Rectangle(double height, double width) : _height(height), _width(width){
    if(height <= 0 || width <= 0){
        throw std::invalid_argument("Width and height cannot be zero or a negative value\n");
    }
}

/*
    Returns the height of a valid Rectangle object.
*/
double Rectangle::GetHeight(){
    return _height;
}

/*
    Returns the width of a valid Rectangle object.
*/
double Rectangle::GetWidth(){
    return _width;
}

/*
    This calculates the area of a valid Rectangle object. This will be called to the RectangleFrustrum class when calculating the volume and surface area.
*/
double Rectangle::GetArea(){
    return _height * _width;
}