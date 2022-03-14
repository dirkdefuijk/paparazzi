#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

Mat canny_edge(Mat image)
{
    Mat dst;
    Mat img_blur;
    cvtColor(image, image, COLOR_RGB2GRAY, 0);
    GaussianBlur(image,img_blur, Size(5,5), BORDER_DEFAULT);
    threshold(img_blur, img_blur, 86, 255, THRESH_BINARY);
    Canny(image, dst, 40, 255, 3, false);
    // closing = morphology(dst,MORPH_CLOSE)

    return dst;
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
    imshow("Display Image", canny_edge(image));

    waitKey(0);
    
    return 0;
}