#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

Mat watershedder(Mat image) 
{
    Mat gray;
    Mat opening;
    Mat background;
    Mat M(3,3,CV_8U);

    cvtColor(image, gray, COLOR_RGBA2GRAY, 0);
    threshold(gray, gray, 0, 255, THRESH_BINARY_INV+THRESH_OTSU);

    erode(gray,gray,M);
    dilate(gray, opening, M);
    dilate(opening, background, M, Point(-1,-1), 3);

    return background;
}



int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }

    Mat image = imread( argv[1], 1 );

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
    namedWindow("Display Image", WINDOW_AUTOSIZE );
    imshow("Display Image", watershedder(image));

    waitKey(0);
    
    return 0;
}
