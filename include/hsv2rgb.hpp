// Source: https://gist.github.com/fairlight1337/4935ae72bcbcc1ba5c72

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include <glm/vec3.hpp>

using namespace std;

/*! \brief Convert RGB to HSV color space

  Converts a given set of RGB values `r', `g', `b' into HSV
  coordinates. The input RGB values are in the range [0, 1], and the
  output HSV values are in the ranges h = [0, 360], and s, v = [0,
  1], respectively.

  \param rgb.r Red component, used as input, range: [0, 1]
  \param rgb.g Green component, used as input, range: [0, 1]
  \param rgb.b Blue component, used as input, range: [0, 1]
  \param hsv.x Hue component, used as output, range: [0, 360]
  \param hsv.y Hue component, used as output, range: [0, 1]
  \param hsv.z Hue component, used as output, range: [0, 1]

*/
glm::vec3 RGBtoHSV(glm::vec3 rgb);

/*! \brief Convert HSV to RGB color space

  Converts a given set of HSV values `h', `s', `v' into RGB
  coordinates. The output RGB values are in the range [0, 1], and
  the input HSV values are in the ranges h = [0, 360], and s, v =
  [0, 1], respectively.

  \param rgb.r Red component, used as output, range: [0, 1]
  \param rgb.g Green component, used as output, range: [0, 1]
  \param rgb.b Blue component, used as output, range: [0, 1]
  \param hsv.x Hue component, used as input, range: [0, 360]
  \param hsv.y Hue component, used as input, range: [0, 1]
  \param hsv.z Hue component, used as input, range: [0, 1]

*/
glm::vec3 HSVtoRGB(glm::vec3 hsv);
