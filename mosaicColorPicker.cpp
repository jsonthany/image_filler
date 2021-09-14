#include "mosaicColorPicker.h"
#include <stdio.h>
#include <math.h>

MosaicColorPicker::MosaicColorPicker(int mosaicwidth, PNG& inputimage)
{
  // Set fields
  width = mosaicwidth;
  img = inputimage;
  mosaicimg.resize(inputimage.width(), inputimage.height());

  double h = 0;
  double s = 0;
  double l = 0;
  double a = 0;

  int count = 0;

  int currWidthIter  = 0;
  int currHeightIter = 0;

  int maxWidthIter  = ceil((double) inputimage.width()  / (double) mosaicwidth);
  int maxHeightIter = ceil((double) inputimage.height() / (double) mosaicwidth);

  while (currHeightIter < maxHeightIter) {
    while (currWidthIter < maxWidthIter) {

      int xPos = currWidthIter  * mosaicwidth;
      int yPos = currHeightIter * mosaicwidth;

      for (int x = 0; x < mosaicwidth; x++) {
        if ((xPos + x) < inputimage.width()) {

          for (int y = 0; y < mosaicwidth; y++) {
            if ((yPos + y) < inputimage.height()) {

              HSLAPixel *pixel = img.getPixel(xPos + x, yPos + y);
              h += pixel->h;
              l += pixel->l;
              s += pixel->s;
              a += pixel->a;

              count++;
            }
          }
        }
      }

      h = h / count;
      s = s / count;
      l = l / count;
      a = a / count;

      for (int x = 0; x < mosaicwidth; x++) {
        for (int y = 0; y < mosaicwidth; y++) {
          if ((xPos + x) < mosaicimg.width() && (yPos + y) < mosaicimg.height()) {
            HSLAPixel *pixel = mosaicimg.getPixel(xPos + x, yPos + y);
            pixel->h = h;
            pixel->s = s;
            pixel->l = l;
            pixel->a = a;
          }
        }
      }

      currWidthIter++;

      h = 0;
      s = 0;
      l = 0;
      a = 0;
      count = 0;

    }

    currHeightIter++;
    currWidthIter = 0;
    
  } 
}

HSLAPixel MosaicColorPicker::operator()(point p)
{
  HSLAPixel* pixel = mosaicimg.getPixel(p.x, p.y);
  return *pixel;
}
