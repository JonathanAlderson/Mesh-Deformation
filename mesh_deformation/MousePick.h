///////////////////////////////////////////////////
//
//	Jonathan Alderson
//	October, 2020
//
//	------------------------
//	MousePick.h
//	------------------------
//
//  Class to get mouse position in world space from screen space
//
///////////////////////////////////////////////////

#ifndef MOUSEPICK_H
#define MOUSEPICK_H

#include "Cartesian3.h"
#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include "gtx/intersect.hpp"
#include <vector>

class MousePick
{
public:

  MousePick(std::vector<Cartesian3> *cagePoints);

  int click(float x, float y);

  glm::vec2 drag(float x, float y);

  bool dragging;
  int closest;
  glm::vec2 start;

  std::vector<Cartesian3> *interceptPoints;

};

#endif
