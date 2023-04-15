#include "CImg.h"
#include "ImageUtils.h"
#include "Structs.h"
using namespace cimg_library;

void
setPixel(CImg<uint8_t>& img, rc::RGBA p, int x, int y)
{
    img(x, y, 0) = p.r;
    img(x, y, 1) = p.g;
    img(x, y, 2) = p.b;
    img(x, y, 3) = p.a;
}

rc::RGBA
getPixel(const CImg<uint8_t>& img, int x, int y)
{
    rc::RGBA p;
    p.r = img(x, y, 0);
    p.g = img(x, y, 1);
    p.b = img(x, y, 2);
    p.a = img(x, y, 3);
    return p;
}

void
convertRcImageToCImg(rc::Image& source, CImg<uint8_t>& target)
{
    target.assign(source.w, source.h, 4);
    for (int y = 0; y < target.height(); y++) {
        for (int x = 0; x < target.width(); ++x) {
            setPixel(target, ImageUtils::getRGBA(&source, x, y), x, y);
        }
    }
}

void
convertCImgToRcImage(CImg<uint8_t>& source, rc::Image& target)
{
    for (int y = 0; y < source.height(); y++) {
        for (int x = 0; x < source.width(); ++x) {
            rc::RGBA rgba = getPixel(source, x, y);
            ImageUtils::setRGBA(&target, rgba, x, y);
        }
    }
}

int
main()
{
    rc::Image rcimage, rctarget;
    
    
    ImageUtils::loadImageAsRGBA("images/grace8.jpg", &rcimage);
    CImg<uint8_t> cimg;
    convertRcImageToCImg(rcimage, cimg);
    cimg.blur(1.0);
    convertCImgToRcImage(cimg, rcimage);
    ImageUtils::convertImageToGray(&rcimage, &rctarget, 0.8, 255);

    ImageUtils::saveImageAsPNG("cimg_test.png", &rctarget);

    ImageUtils::freeImage(&rcimage);
    ImageUtils::freeImage(&rctarget);
}