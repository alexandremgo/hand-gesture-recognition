#include "outline.hpp"


bool getOutline(std::vector<cv::Point> &largest_outline, const cv::Mat &image, 
        int red_saturation_threshold, bool display_imgs) {
    char detect;

    int DIM,index,indexNB;
    unsigned int numc;
    uchar R,G,B;
    std::vector<std::vector<cv::Point>> outlines; 
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

    // Finds the outlines in the image
    //
    // OpenCV findContours parameters:
    // Source: an 8-bit single-channel image,
    // Output: Each contour is stored as a vector of points,
    // Output: Containing information about the image topology (nesting outlines)
    // Mode: CV_RETR_EXTERNAL retrieves only the extreme outer outlines
    // Method: CV_CHAIN_APPROX_NONE stores absolutely all the contour points.
    cv::findContours( Binaire, outlines, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    if(outlines.size() == 0)
        return false;

    cv::Mat largest_outline_img = cv::Mat::zeros(Y, X, CV_8UC1);
    std::vector<cv::Point> cur_outline;
    unsigned int id_max = 0, contourMax = 0;

    // Only keep the main/largest contour
    for(numc = 0; numc < outlines.size(); numc++ ) {

        bool on_edge = false;        // True if the outline "on the edge" of frame
        cur_outline = outlines[numc];
        for(unsigned int index = 0; index < outlines[numc].size(); index++ ){
            if(outlines[numc][index].x == X or outlines[numc][index].y == Y){
                // X,Y dimensions of frame
                on_edge = true;
            }
        }
        if(cur_outline.size() > contourMax and !on_edge) {                                   
            id_max = numc;                                                       
            contourMax = cur_outline.size();                                      
        }       
    }

    // Draw only the largest contour
    cv::drawContours(largest_outline_img, outlines, id_max, 255);

    largest_outline = outlines[id_max];

    if(display_imgs) {
        std::string window_name_0 = "Blured Image";
        cv::namedWindow(window_name_0, cv::WINDOW_NORMAL);
        cv::resizeWindow(window_name_0, WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT);
        cv::moveWindow(window_name_0, 0, 0);
        cv::imshow(window_name_0, bluredImage);

        std::string window_name_1 = "Detection";
        cv::namedWindow(window_name_1, cv::WINDOW_NORMAL);
        cv::resizeWindow(window_name_1, WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT);
        cv::moveWindow(window_name_1, WINDOW_SIZE_WIDTH + WINDOW_POS_OFFSET, 0);
        cv::imshow(window_name_1, Binaire);

        std::string window_name_2 = "Largest outline";
        cv::namedWindow(window_name_2, cv::WINDOW_NORMAL);
        cv::resizeWindow(window_name_2, WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT);
        cv::moveWindow(window_name_2, 2*WINDOW_SIZE_WIDTH + WINDOW_POS_OFFSET, 0);
        cv::imshow(window_name_2, largest_outline_img);
    }

    return true;
}


bool getOutlineFromBinaryImg(std::vector<cv::Point> &largest_outline, const cv::Mat &binary_img) {
    std::vector<std::vector<cv::Point>> outlines; 
    std::vector<cv::Vec4i> hierarchy; 

    int Y=binary_img.rows,
        X=binary_img.cols;

    // Contour detection
    /*
     ** findContours parameters:
     ** Source: an 8-bit single-channel image,
     ** Output: Each contour is stored as a vector of points,
     ** Output: Containing information about the image topology (nesting outlines)
     ** Mode: CV_RETR_EXTERNAL retrieves only the extreme outer outlines
     ** Method: CV_CHAIN_APPROX_NONE stores absolutely all the contour points.
     */
    cv::findContours( binary_img, outlines, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    if(outlines.size() == 0)
        return false;

    unsigned int id_max = 0, contourMax = 0;
    std::vector<cv::Point> cur_outline;

    // Only keep the main/largest contour
    for(unsigned int numc = 0; numc < outlines.size(); numc++ ) {

        bool on_edge = false;        // True if contour "on the edge" of frame
        cur_outline = outlines[numc];
        for(unsigned int index = 0; index < outlines[numc].size(); index++ ){
            if(outlines[numc][index].x == X or outlines[numc][index].y == Y){
                // X,Y dimensions of frame
                on_edge = true;
            }
        }
        if(cur_outline.size() > contourMax and !on_edge) {                                   
            id_max = numc;                                                       
            contourMax = cur_outline.size();                                      
        }       
    }

    largest_outline = outlines[id_max];

    return true;
}

