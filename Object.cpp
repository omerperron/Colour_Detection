//
//  Object.cpp
//  Project
//
//  Created by omer perron on 16/11/17.
//  Copyright © 2017 omer perron. All rights reserved.
//

#include "Object.hpp"

Point Object::getCenter(){
    return center;
}

int Object::getColour(){
    return colour;
}
void Object::updateCenter(){
   
    int x = 0, y = 0;
    for(Point point: points){
        x += point.x;
        y += point.y;
    }
    x = x / points.size();
    y = y / points.size();
    
    center = Point(x,y);

}

unsigned long Object::getNumPoints(){
    return points.size();
}

int Object::getWidth(){
    int width = 0;
    for(Point point : points){
        int dist = abs(point.x - getCenter().x);
        if(dist > width) width = dist;
    }
    
    if (width == 0) width = 1;
    return width;
}

int Object::getHeight(){
    int height = 0;
    for(Point point : points){
        int dist = abs(point.y - getCenter().y);
        if(dist > height) height = dist;
    }
    if (height == 0) height = 1;
    return height;
}

Point Object::getTL(){
    Point TL(1000, 1000);
    for(Point point : points){
        if(point.x < TL.x) TL.x = point.x;
        if(point.y < TL.y) TL.y = point.y;
    }
    return TL;
}


bool Object::addPosition(Point toAdd){
    int threshold = 50;

    if(points.size() != 0 && distance(getCenter(), toAdd) >= threshold){
        return false;
    }
    points.push_back(Point(toAdd.x, toAdd.y));
    updateCenter();
    
    return true;
}

void Object::cullPoints(){
    int threshold = 40;

    Point cen = getCenter();
    for(Point point : points){
        if(distance(cen, point) > threshold){
            points.erase(std::remove(points.begin(), points.end(), point), points.end());
        }
    }
    updateCenter();
}

int Object::distance(Point a, Point b){
    return sqrt( pow(a.x-b.x, 2) + pow(a.y-b.y, 2));
}










