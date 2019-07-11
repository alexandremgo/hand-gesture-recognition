#include "contour.hpp"

/**
 * From RGB image
 */
bool getOutline(std::vector<cv::Point> &largest_outline, const cv::Mat &image, 
        int red_saturation_threshold, bool display_imgs) {
    char detect;

    int DIM,index,indexNB;
    unsigned int numc;
    uchar R,G,B;
    std::vector<std::vector<cv::Point>> contours; 
    std::vector<cv::Vec4i> hierarchy; 

    int Y=image.rows,
        X=image.cols;


    // Colors detection
    cv::Mat bluredImage;
    cv::Mat Binaire(Y, X, CV_8UC1);

    /* 
     ** GaussianBlur: to smooth the noises of the input image 
     ** Parameters: src, dest, 
     ** ksize: Gaussian kernel size, 
     ** Gaussian kernel standard deviation in X direction,
     ** Gaussian kernel standard deviation in Y direction, 
     */
    // cv::GaussianBlur(image, bluredImage, cv::Size(7,7), 1.5, 1.5);
    cv::GaussianBlur(image, bluredImage, cv::Size(15,15), 31, 31);

    // If RGB, DIM = 3
    DIM=bluredImage.channels();

    /*
     ** Garde les points pour lesquels la couleur est peu saturée (proche des teintes de
     ** gris), et qui ont une composante dominante dans le rouge (comme la main)
     ** Donc mets à 0 les points où le B et G sont importants
     **
     ** + le red_saturation_threshold est important, - les B et G doivent être saturés par rapport à R
     */
    for (index=0,indexNB=0;index<DIM*X*Y;index+=DIM,indexNB++)
    {
        detect=0;
        B=bluredImage.data[index    ];
        G=bluredImage.data[index + 1];
        R=bluredImage.data[index + 2];
        if ((R>G) && (R>B))
            if (((R-B)>=red_saturation_threshold) || ((R-G)>=red_saturation_threshold))
                detect=1;

        // Sature (255 = blanc) la couleur si détecter
        if (detect==1)
            Binaire.data[indexNB]=255;
        // Sinon élimine
        else
            Binaire.data[indexNB]=0;

    }

    // Contour detection
    /*
     ** findContours parameters:
     ** Source: an 8-bit single-channel image,
     ** Output: Each contour is stored as a vector of points,
     ** Output: Containing information about the image topology (nesting contours)
     ** Mode: CV_RETR_EXTERNAL retrieves only the extreme outer contours
     ** Method: CV_CHAIN_APPROX_NONE stores absolutely all the contour points.
     */
    cv::findContours( Binaire, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    if(contours.size() == 0)
        return false;

    cv::Mat Dessin = cv::Mat::zeros(Y, X, CV_8UC1);
    cv::Mat Contour = cv::Mat::zeros(Y, X, CV_8UC1);
    std::vector<cv::Point> curContour;
    unsigned int id_max = 0, contourMax = 0;

    // Only keep the main/largest contour
    for(numc = 0; numc < contours.size(); numc++ ) {
        /*
           curContour = contours[numc];
           if(int curCSize = curContour.size() > contourMax) {
           id_max = numc;
           contourMax = curCSize;
           }
           */

        bool on_edge = false;        // True if contour "on the edge" of frame
        curContour = contours[numc];
        for(unsigned int index = 0; index < contours[numc].size(); index++ ){
            if(contours[numc][index].x == X or contours[numc][index].y == Y){
                // X,Y dimensions of frame
                on_edge = true;
            }
        }
        if(curContour.size() > contourMax and !on_edge) {                                   
            id_max = numc;                                                       
            contourMax = curContour.size();                                      
        }       
    }

    // Draw only the largest contour
    cv::drawContours(Contour, contours, id_max, 255);

    // Save the image in pgm for matlab
    /*
       istringstream name_stream(argv[1]);
       string name;
       getline(name_stream, name, '.');
       imwrite(name + ".pgm", Contour);
       */

    largest_outline = contours[id_max];

    // cout << "Largest contour points: " << endl << largest_outline << endl;

    if(display_imgs) {
        cv::namedWindow("Image",1);
        cv::namedWindow("Blured Image",1);
        cv::namedWindow("Detection",1);
        cv::namedWindow("Contours",1);
        cv::imshow("Image", image);
        cv::imshow("Blured Image", bluredImage);
        cv::imshow("Detection", Binaire);
        cv::imshow("Contours", Contour);
    }

    return true;
}

/**
 * Get the contour from an image 8UC1 in black and white
 */
bool getOutlineFromBinaryImg(std::vector<cv::Point> &largest_outline, const cv::Mat &binary_img) {
    std::vector<std::vector<cv::Point>> contours; 
    std::vector<cv::Vec4i> hierarchy; 

    int Y=binary_img.rows,
        X=binary_img.cols;

    // Contour detection
    /*
     ** findContours parameters:
     ** Source: an 8-bit single-channel image,
     ** Output: Each contour is stored as a vector of points,
     ** Output: Containing information about the image topology (nesting contours)
     ** Mode: CV_RETR_EXTERNAL retrieves only the extreme outer contours
     ** Method: CV_CHAIN_APPROX_NONE stores absolutely all the contour points.
     */
    cv::findContours( binary_img, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    if(contours.size() == 0)
        return false;

    unsigned int id_max = 0, contourMax = 0;
    std::vector<cv::Point> curContour;

    // Only keep the main/largest contour
    for(unsigned int numc = 0; numc < contours.size(); numc++ ) {

        bool on_edge = false;        // True if contour "on the edge" of frame
        curContour = contours[numc];
        for(unsigned int index = 0; index < contours[numc].size(); index++ ){
            if(contours[numc][index].x == X or contours[numc][index].y == Y){
                // X,Y dimensions of frame
                on_edge = true;
            }
        }
        if(curContour.size() > contourMax and !on_edge) {                                   
            id_max = numc;                                                       
            contourMax = curContour.size();                                      
        }       
    }

    largest_outline = contours[id_max];

    return true;
}

