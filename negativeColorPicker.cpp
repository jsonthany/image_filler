#include "negativeColorPicker.h"

NegativeColorPicker::NegativeColorPicker(PNG& inputimg)
{
  /* your code here */
  img = inputimg;
}

HSLAPixel NegativeColorPicker::operator()(point p)
{
  // get the pixel
  HSLAPixel *pixel = img.getPixel(p.x, p.y);

  double h = pixel->h;
  double s = pixel->s;
  double l = pixel->l;

  // invert the h and l values
  h = ((int) h + 180) % 360;
  l = 1 - l;

  // set new inverted pixel colors
  HSLAPixel result;
  result.h = h;
  result.s = s;
  result.l = l;

  // return the pixel
  return result;
}
