#pragma once

/*
    Emily Hull
*/


/*
    A simple class to create a Rectangle object.
*/
class Rectangle{
    private:
        double _width;
        double _height;
    public:
        Rectangle(double width, double height);
        double GetHeight();
        double GetWidth();
        double GetArea();
};