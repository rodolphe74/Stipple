// Stipple.cpp : définit le point d'entrée de l'application.
//

#include "Stipple.h"
#define JC_VORONOI_IMPLEMENTATION
#define JC_VORONOI_CLIP_IMPLEMENTATION
#include "jc_voronoi_clip.h"

#include "ImageUtils.h"
#include <SFML/Graphics.hpp>
#include <cmath>

using namespace std;

rc::Image *image, *grayImage;
int borderWidth, lowBorderX, hiBorderX, lowBorderY, hiBorderY;

constexpr int MAX_PARTICLES = 8000;
// constexpr int MAX_PARTICLES = 4000;
constexpr float dotSizeFactor = 4; // 5;
constexpr float minDotSize = 1.75; // 2;
constexpr float maxDotSize = minDotSize * (1 + dotSizeFactor);
constexpr float dotScale = (maxDotSize - minDotSize);
constexpr int cellBuffer = 100; // Scale each cell to fit in a cellBuffer-sized
                                // square window for computing the centroid.
constexpr int iterations = 1128;
// constexpr int iterations = 200;

std::vector<sf::Vector2f> particles(MAX_PARTICLES);
std::vector<sf::Vector2f> centroids(MAX_PARTICLES);
sf::RenderTexture rt;
jcv_diagram diagram;

int
tirageAuSort(int max)
{
    return std::rand() / ((RAND_MAX + 1u) / max);
}

void
init(std::string imageFileName)
{
    image = new Image();
    grayImage = new Image();
    std::srand((unsigned int)std::time((time_t*)nullptr));
    ImageUtils::loadImageAsRGBA(imageFileName.c_str(), image);
    std::cout << "Image:" << image->w << "*" << image->h << std::endl;

    // Add blur to original image
    CImg<uint8_t> cimg;
    ImageUtils::convertRcImageToCImg(*image, cimg);
    cimg.blur(2.0);
    ImageUtils::convertCImgToRcImage(cimg, *image);
    ImageUtils::saveImageAsPNG("blur.png", image);

    ImageUtils::convertImageToGray(image, grayImage, 1.0, 255);

    rc::RGBA rgba = ImageUtils::getRGBA(image, 50, 60);

    borderWidth = (int)std::round(image->w * 0.01);
    lowBorderX = borderWidth;
    hiBorderX = (int)std::round(image->w * 0.98);
    lowBorderY = borderWidth;
    hiBorderY = (int)std::round(image->h * 0.98);

    if (!rt.create(
          image->w, image->h, sf::ContextSettings(0, 0, 0, 0, 1, 0, false))) {
        perror("can't create texture");
        return;
    }
}

void
free()
{
    delete image;
    delete grayImage;
}

void
sprinkle()
{
    int i = 0;
    float fx, fy;
    float p, t;
    uint32_t x, y;
    while (i < MAX_PARTICLES) {
        fx = (float)(lowBorderX + tirageAuSort(hiBorderX - lowBorderX));
        fy = (float)(lowBorderY + tirageAuSort(hiBorderY - lowBorderY));

        // float p = brightness(imgblur.pixels[floor(fy) * imgblur.width +
        // floor(fx)]) / 255; OK to use simple floor_ rounding here, because
        // this is a one-time operation, creating the initial distribution that
        // will be iterated.

        x = (int)std::floor(fx);
        y = (int)std::floor(fy);
        if (x < grayImage->w && y < grayImage->h) {
            p = (float)(ImageUtils::getRGBA(
                          grayImage, (int)std::floor(fx), (int)std::floor(fy))
                          .r /
                        255.0f);
            t = (float)(rand()) / (float)(RAND_MAX);
            if (t >= p) {
                sf::Vector2f p1 = { fx, fy };
                particles[i] = p1;
                i++;
            }
        }
    }
}

void
drawCentroids()
{
    for (auto c = centroids.begin(); c != centroids.end(); c++) {
        uint32_t x = (int)std::floor(c->x);
        uint32_t y = (int)std::floor(c->y);
        float fv = -1;
        if (x < grayImage->w && y < grayImage->h)
            fv = (float)(ImageUtils::getRGBA(grayImage, x, y).r / 255.0f);
        if (fv >= 0) {
            float r = maxDotSize - fv * dotScale;
            sf::CircleShape c(r);
            c.setFillColor(sf::Color::Black);
            c.setOutlineColor(sf::Color::Black);
            c.setPosition((float)x - r, (float)y - r);
            rt.draw(c);
        }
    }
}

void
drawDiagram(const jcv_diagram& diagram,
            sf::Color C,
            bool withSites,
            bool withEdges)
{
    int j = 0;
    const jcv_site* sites = jcv_diagram_get_sites(&diagram);
    for (int i = 0; i < diagram.numsites; ++i) {
        const jcv_site* site = &sites[i];

        if (withSites) {
            jcv_point p = site->p;
            uint32_t x = (int)std::floor(p.x);
            uint32_t y = (int)std::floor(p.y);
            float fv = -1;
            if (x < grayImage->w && y < grayImage->h)
                fv = (float)(ImageUtils::getRGBA(grayImage, x, y).r / 255.0f);
            if (fv >= 0) {
                float r = maxDotSize - fv * dotScale;
                sf::CircleShape c(r);
                c.setFillColor(sf::Color::Black);
                c.setOutlineColor(sf::Color::Black);
                c.setPosition((float)x - r, (float)y - r);
                rt.draw(c);
            }
        }

        int count = 0;
        if (withEdges) {
            const jcv_graphedge* edge = site->edges;
            const jcv_graphedge* edgeBis = site->edges;
            while (edge) {
                // printf("%f %f\n", edge->pos[0].x, edge->pos[0].y);
                count++;
                edge = edge->next;
            }
            sf::ConvexShape cs;
            cs.setPointCount(count);
            j = 0;
            while (edgeBis) {
                cs.setPoint(j++,
                            sf::Vector2f(edgeBis->pos[0].x, edgeBis->pos[0].y));
                cs.setFillColor(sf::Color(0, 0, 0, 0));
                cs.setOutlineThickness(1.0f);
                cs.setOutlineColor(C);
                edgeBis = edgeBis->next;
            }
            rt.draw(cs);
        }
    }
}

int
centroid()
{
    float xMax = 0;
    float xMin = (float)grayImage->w;
    float yMax = 0;
    float yMin = (float)grayImage->h;
    float xt, yt;
    float xDiff = xMax - xMin;
    float yDiff = yMax - yMin;
    float maxSize = max(xDiff, yDiff);
    float minSize = min(xDiff, yDiff);
    float scaleFactor = 1.0;
    float xSum = 0;
    float ySum = 0;
    float dSum = 0;
    float picDensity = 1.0;
    float xTemp = (xSum);
    float yTemp = (ySum);
    float stepSize = (1 / scaleFactor);

    centroids.clear();
    const jcv_site* sites = jcv_diagram_get_sites(&diagram);
    for (int i = 0; i < diagram.numsites; ++i) {

        xMax = 0;
        xMin = (float)grayImage->w;
        yMax = 0;
        yMin = (float)grayImage->h;
        xt, yt;

        const jcv_site* site = &sites[i];

        jcv_point sum = site->p;
        int count = 1;

        const jcv_graphedge* edge = site->edges;

        while (edge) {

            xt = edge->pos[0].x;
            yt = edge->pos[0].y;

            if (xt < xMin)
                xMin = xt;
            if (xt > xMax)
                xMax = xt;
            if (yt < yMin)
                yMin = yt;
            if (yt > yMax)
                yMax = yt;

            edge = edge->next;
        }

        xDiff = xMax - xMin;
        yDiff = yMax - yMin;
        maxSize = max(xDiff, yDiff);
        minSize = min(xDiff, yDiff);
        scaleFactor = 1.0;

        while (maxSize > cellBuffer) {
            scaleFactor *= 0.5;
            maxSize *= 0.5;
        }

        while (maxSize < (cellBuffer / 2)) {
            scaleFactor *= 2;
            maxSize *= 2;
        }

        if ((minSize * scaleFactor) > (cellBuffer / 2)) {
            // Special correction for objects of
            // near-unity (square-like) aspect ratio,
            // which have larger area *and* where it is less essential to
            // find the exact centroid:
            scaleFactor *= 0.5;
        }

        stepSize = (1 / scaleFactor);

        xSum = 0;
        ySum = 0;
        dSum = 0;
        picDensity = 1.0;

        for (float x = xMin; x <= xMax; x += stepSize) {
            for (float y = yMin; y <= yMax; y += stepSize) {

                sf::Vector2f p0 = sf::Vector2f(x, y);

                // Thanks to polygon clipping, NO vertices will be
                // beyond the sides of imgblur.
                // PicDensity =
                //  255.001 -
                //  (brightness(imgblur.pixels[round(y) * imgblur.width +
                //                             round(x)]));

                picDensity =
                  255.001f - (float)(ImageUtils::getRGBA(grayImage,
                                                         (int)std::floor(x),
                                                         (int)std::floor(y))
                                       .r);

                xSum += picDensity * x;
                ySum += picDensity * y;
                dSum += picDensity;
            }
        }

        if (dSum > 0) {
            xSum /= dSum;
            ySum /= dSum;
        }

        sf::Vector2f centr;

        xTemp = (xSum);
        yTemp = (ySum);

        if ((xTemp <= lowBorderX) || (xTemp >= hiBorderX) ||
            (yTemp <= lowBorderY) || (yTemp >= hiBorderY)) {

            // tant pis

            // If new centroid is computed to be outside the visible region,
            // use the geometric centroid instead. This will help to prevent
            // runaway points due to numerical artifacts.
            // centr = region.getCentroid();
            // xTemp = centr.x;
            // yTemp = centr.y;

            //// Enforce sides, if absolutely necessary:  (Failure to do so
            //// *will* cause a crash, eventually.)

            // if (xTemp <= lowBorderX)
            //     xTemp = lowBorderX + 1;
            // if (xTemp >= hiBorderX)
            //     xTemp = hiBorderX - 1;
            // if (yTemp <= lowBorderY)
            //     yTemp = lowBorderY + 1;
            // if (yTemp >= hiBorderY)
            //     yTemp = hiBorderY - 1;
        }
        centroids.push_back(sf::Vector2f(xTemp, yTemp));
    }

    return diagram.numsites;
}

void
voronoi()
{
    jcv_real width = (jcv_real)image->w;
    jcv_real height = (jcv_real)image->h;

    size_t size = particles.size();
    jcv_point* points = new jcv_point[size];
    int i = 0;
    for (auto v = particles.begin(); v != particles.end(); v++) {
        if (i < size) {
            points[i].x = v->x;
            points[i].y = v->y;
        }
        i++;
    }

    jcv_rect bounding_box = { { 10.0f, 10.0f },
                              { width - 10.0f, height - 10.0f } };

    memset(&diagram, 0, sizeof(jcv_diagram));
    jcv_diagram_generate(
      i, (const jcv_point*)points, &bounding_box, 0, &diagram);

    delete[] points;
}

int
main()
{
    init("images/grace.jpg");
    rt.clear(sf::Color::White);

    sprinkle();

    for (int i = 0; i < iterations; i++) {
        voronoi();
        // drawDiagram(diagram, sf::Color::Black, true, false);
        int c = centroid();
        cout << "Iteration " << i << "  -  centroids:" << c << endl;
        particles = centroids;
        jcv_diagram_free(&diagram);
    }

    drawCentroids();

    ImageUtils::saveTextureAsPNG("Stipple.png", rt);

    free();

    return 0;
}
