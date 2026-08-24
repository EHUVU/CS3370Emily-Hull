/*
    Emily Hull
*/
#include "test_harness.h"
#include "rectanglefrustrum.h"
#include "rectangle.h"
#include <iostream>

/*
    Prompts the user to enter the height and width values of 2 rectangles. The rectangles cannot have zero or negative values. Once this check
    is passed, it will prompt for a height of the rectangluar frustrum object. This cannot be zero or negative. It will then output the results of the
    volume and surface area equations.

    If active and uncommented, the results of the test harness will output. I made some changes to this test harness to fit the separate rectangle object class
    I set up. The original values for these remain intact to test the program with the original intent.
*/

int main(){
    double x, y, frustHeight;
    char choice;

    while(true){
        try {
            std::cout << "Please enter the height of rectangle 1:" << std::endl;
            std::cin >> x;
            std::cout << "Please enter the width of rectangle 1:" << std::endl;
            std::cin >> y;

            Rectangle rect1(x, y);

            std::cout << "Please enter the height of rectangle 2:" << std::endl;
            std::cin >> x;
            std::cout << "Please enter the width of rectangle 2:" << std::endl;
            std::cin >> y;

            Rectangle rect2(x, y);

            std::cout << "Please enter the height of the frustrum:" << std::endl;
            std::cin >> frustHeight;

            RectangleFrustrum rf(rect1, rect2, frustHeight);

            std::cout << "Volume: " << rf.GetFrustrumVolume() << std::endl;
            std::cout << "Surface Area: " << rf.GetFrustrumSurfaceArea() << std::endl;
        }
        catch(const std::invalid_argument& e){
            std::cout << e.what();
        }
        std::cout << "Try again? Enter ""q"" to quit." << std::endl;
            std::cin >> choice;
            if(choice == 'q'){
                break;
            }else{
                continue;
            }
    }

    TestHarness test; // Uncomment for test results.
    test.runAll(); //Currently passes all tests.

    return 0;
}

//g++ main.cpp rectanglefrustrum.cpp rectangle.cpp -o main
//./main