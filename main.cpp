#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <math.h>
#include "Object.hpp"
using namespace cv;
using namespace std;

vector<Object> objects;
Mat originalImage;
Mat image;

//gets the position and colour of a pixel, and checks if it can be added to an existing object.
//if there is no existing object within a arbitrary distance, a new object is created with the pixel as its center.
void addNewPosition(int x, int y, int colour){
    Point toAdd(x, y);
    
    //check if object exists near by, and add the pixel to it if it does
    for(Object &obj : objects){
        if(colour == obj.getColour() && obj.addPosition(toAdd)) return;
    }
    //otherwise create a new object and add the pixel as its center
    Object newObj(colour);
    newObj.addPosition(toAdd);
    objects.push_back(newObj);
}

//dilates and then erodes an image passed as a parameter.
void morphologicalClosing(int x, int y, Mat img){
    
    dilate( img, img, getStructuringElement(MORPH_RECT, Size(x, y)) );
    erode(img, img, getStructuringElement(MORPH_RECT, Size(x, y)) );
}

//erodes and then dilates an image passed as a parameter.
void morphologicalOpening(int x, int y, Mat img){
    
    erode(img, img, getStructuringElement(MORPH_RECT, Size(x, y)) );
    dilate( img, img, getStructuringElement(MORPH_RECT, Size(x, y)) );
}

//check if  a pixels HSV values categorise it as a red pixel
bool checkRed(int hVal, int sVal, int vVal){
    if((hVal >= 114 && hVal <= 122) && (sVal > 173 && sVal < 255) && (vVal > 58 && vVal < 255)) return true;
    return false;
}

//check if  a pixels HSV values categorise it as a blue pixel
bool checkBlue(int hVal, int sVal, int vVal){
    if((hVal >= 0 && hVal <= 52) && (sVal > 0 && sVal < 255) && (vVal > 0 && vVal < 255)) return true;
    return false;
}

//check if  a pixels HSV values categorise it as a green pixel
bool checkGreen(int hVal, int sVal, int vVal){
    if((hVal >= 66 && hVal <= 87) && (sVal >= 72 && sVal <= 164) && (vVal >= 80 && vVal <= 141)) return true;
    return false;
}

//check if  a pixels HSV values categorise it as a yellow pixel
bool checkYellow(int hVal, int sVal, int vVal){
    if((hVal >= 96 && hVal <= 107) && (sVal >= 96 && sVal <= 172) && (vVal >= 149 && vVal <= 218)) return true;
    return false;
}


//this function loops through each object, finds its width and height and draws a rectangle around it of the correct
//colour.
void drawRectangles(){
    for(Object obj : objects){
        //if object doesnt have enough pixels it is too small, we dont print it
        if(obj.getNumPoints() < 60) continue;

        
        //get objects width, height and top left corner for printing
        Point tl = obj.getTL();
        int width = obj.getWidth();
        int height = obj.getHeight();
        int length = width;
        if(height > width) length = height;
        
        //draw red rectangle
        if(obj.getColour() == 0){
            Rect rect(tl.x, tl.y, length*2, length*2);
            rectangle(originalImage, rect, Scalar(0, 0, 255));
            rectangle(originalImage, rect, Scalar(0, 0, 255));

        //draw blue rectangle
        }else if(obj.getColour() == 1){
            Rect rect(tl.x, tl.y, length*2, length*2);
            rectangle(originalImage, rect, Scalar(255, 0, 0));
            rectangle(image, rect, Scalar(255, 0, 0));

        //draw green rectangle
        }else if(obj.getColour() == 2){
            Rect rect(tl.x, tl.y, length*2, length*2);
            rectangle(originalImage, rect, Scalar(0, 255, 0));
            rectangle(image, rect, Scalar(0, 255, 0));

        //draw yellow rectangle
        }else if(obj.getColour() == 3){
            Rect rect(tl.x, tl.y, length*2, length*2);
            rectangle(originalImage, rect, Scalar(0, 255, 255));
            rectangle(image, rect, Scalar(0, 255, 255));
        }
    }
}

//this function set all pixels who arnt red, blue, green or yellow to 0 (black).
//then a morphological process is performed to remove any noise
//finally, all pixels are then looped over again and each remaining pixel is added to an object
//or created into an object in addNewPosition(x,y, colour);
Mat getColours(Mat original){
    
    //convert image to HSV
    cvtColor(original, image, CV_RGB2HSV);

    //get height and width
    int height = image.rows;
    int width = image.cols;
    
    //loop through each pixel
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            Vec3b HSV = image.at<Vec3b>(i,j);

            int hVal = (int) HSV[0];
            int sVal = (int) HSV[1];
            int vVal = (int) HSV[2];

            //make it black if its not red, blue, green or blue
            if(!checkRed(hVal, sVal, vVal) && !checkBlue(hVal, sVal, vVal) &&  !checkGreen(hVal, sVal, vVal)
                   && !checkYellow(hVal, sVal, vVal)){
                image.at<Vec3b>(i,j) = 0;
            }
        }
    }
    
    morphologicalOpening(6, 6, image);

    //loop through again and add pixel to object
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            Vec3b HSV = image.at<Vec3b>(i,j);
            
            int hVal = (int) HSV[0];
            int sVal = (int) HSV[1];
            int vVal = (int) HSV[2];

            if(checkRed(hVal, sVal, vVal)) addNewPosition(j, i, 0);
            else if(checkBlue(hVal, sVal, vVal)) addNewPosition(j, i, 1);
            else if(checkGreen(hVal, sVal, vVal)) addNewPosition(j, i, 2);
            else if(checkYellow(hVal, sVal, vVal)) addNewPosition(j, i, 3);
    
        }
    }
    //draw rectangles ontop of image
    drawRectangles();
    
    cvtColor(image, image, CV_HSV2RGB);
    return image;
}


int main( int argc, char** argv ){
    
    VideoCapture cap("/Users/omer/Documents/PicProcessing/Project/Final_Project/videos/video1.mp4");
    if(!cap.isOpened()){  // check if we succeeded
        //cout << "there was an error opening the file" << endl;
        return -1;
    }
    int width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    int height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    int fps = cap.get(CAP_PROP_FPS);
    int my_fourcc = CV_FOURCC('m', 'p', '4', 'v');
    int frm_count = cap.get(CAP_PROP_FRAME_COUNT);


    VideoWriter  v_dst("/Users/omer/Documents/PicProcessing/Project/Final_Project/videos/video1Result.mp4", my_fourcc, fps, Size(width,height), 1);
    Mat photo;
    photo = imread("/Users/omer/Documents/PicProcessing/Project/Final_Project/videos/image.png", CV_LOAD_IMAGE_COLOR);   // Read the file
    bool pho = false;
    
    Mat original;
    
    if(pho){
        image = getColours(photo);
        namedWindow("Processed Hue", WINDOW_AUTOSIZE);
        imshow( "Processed Hue", image );

    }else{
        cap >> originalImage; // get a new frame from camera
        originalImage.copyTo(image);
        for(int i = 0; i < frm_count-1; i++){
            getColours(image);
            cout << objects.size() << endl;
            v_dst.write(originalImage);

            objects.clear();
            imshow( "Frame", originalImage );
            cap >> originalImage;
            originalImage.copyTo(image);

            char c = (char)waitKey(33);
            if( c == 27 ) break;
        }
        
        //imshow( "Frame", originalImage );
        //waitKey(0);                                          // Wait for a keystroke in the window

    }
    

    return 0;
}








