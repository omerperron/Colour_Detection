//
//  Object.hpp
//  Project
//
//  Created by omer perron on 16/11/17.
//  Copyright © 2017 omer perron. All rights reserved.
//

#ifndef Object_hpp
#define Object_hpp

#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <vector>
#include <math.h>

using namespace cv;
using namespace std;

class Object{
    vector<Point> points;
    Point center;
    int colour;
    
    void cullPoints();
    void updateCenter();

public:

    Object(int _colour) : colour(_colour) {};
    Point getCenter();
    int getColour();
    bool addPosition(Point pos);
    static int distance(Point a, Point b);
    int getWidth();
    int getHeight();
    unsigned long getNumPoints();
    Point getTL();
};


#endif /* Object_hpp */
