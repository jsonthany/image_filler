#include "softBorderColorPicker.h"

SoftBorderColorPicker::SoftBorderColorPicker(unsigned int width, PNG& inputimage, double tol)
{
    borderwidth = width;
    img = inputimage;
    tolerance = tol;
}

HSLAPixel SoftBorderColorPicker::operator()(point p)
{
    double h = 0;
    double s = 0;
    double l = 0;
    double a = 0;
    int count = 0;

    HSLAPixel *originalPixel = &p.c.color;
    bool flag = false;

    int xMin = (int) p.x - (int) borderwidth;
    int yMin = (int) p.y - (int) borderwidth;
    int xMax = (int) p.x + (int) borderwidth;
    int yMax = (int) p.y + (int) borderwidth;

    for (int x = xMin; x <= xMax; x++) {
        for (int y = yMin; y <= yMax; y++) {
            if (isWithinImage(x, y)) {
                if (isWithinDistance(x, y, p.x, p.y)) {
                    HSLAPixel *pixel = img.getPixel(x, y);

                    h += pixel->h;
                    s += pixel->s;
                    l += pixel->l;
                    a += pixel->a;
                    count++;

                    if (p.c.color.dist(*pixel) > tolerance) {
                        flag = true;
                    }
                } 
                else if (p.x < borderwidth || p.x > img.width() - borderwidth || p.y < borderwidth || p.y > img.height() - borderwidth) {
                    flag = true;
                }
            }
        }
    }
    if (!flag) {
        return *originalPixel;
    }
    
    h = h / count;
    s = s / count;
    l = l / count;
    a = a / count;

    return HSLAPixel(h,s,l,a);
}

bool SoftBorderColorPicker::isWithinDistance(unsigned int xCurr, unsigned int yCurr, unsigned int xPoint, unsigned int yPoint) {
    unsigned int distance_squared = (xPoint - xCurr)*(xPoint - xCurr) + (yPoint - yCurr)*(yPoint - yCurr);
    return (distance_squared <= borderwidth * borderwidth);
}

bool SoftBorderColorPicker::isWithinImage(unsigned int x, unsigned int y) {
    return ((x >= 0) && (y >= 0) && (x < img.width()) && (y < img.height()));
}