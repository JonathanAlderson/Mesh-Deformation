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

#include "MousePick.h"
#include <math.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "MousePick.h"

MousePick::MousePick(std::vector<Cartesian3> *cagePoints)
{
  interceptPoints = cagePoints;
  dragging = false;
  closest = -1;
  start = glm::vec2(0., 0.);
}

glm::vec2 MousePick::drag(float x, float y)
{
  //std::cout << interceptPoints[0][closest].x << " " << interceptPoints[0][closest].y << '\n';
  glm::vec2 curr = glm::vec2(x, y);

  glm::vec2 change = start - curr;
  start = curr;

  return change;
}

// Finds the point that has been clicked
int MousePick::click(float x, float y)
{
  // Calculate inverse projection matrix
  float frustum[16];
  glGetFloatv(GL_PROJECTION_MATRIX, frustum);
  glm::mat4 proj = glm::make_mat4(frustum);
  //proj = glm::inverse(proj);

  float modelview[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
  glm::mat4 modelView = glm::make_mat4(modelview);

  glm::vec3 camPos = glm::vec3(0., 0., -1.);

  // create matricies needed for checks
  glm::vec4 viewRay = glm::vec4(x, -y, 1., 1.);
  viewRay = viewRay * glm::inverse(proj); // go to camera space
  viewRay = glm::vec4(viewRay.x, viewRay.y, 1., 0.);
  glm::vec3 viewRay3 = glm::normalize(glm::vec3(viewRay.x, viewRay.y, viewRay.z));


  float radiusSquared = .3;
  glm::vec3 intersectPosition;
  glm::vec3 intersectNormal;
  glm::vec4 sphereCenter4;
  glm::vec3 sphereCenter;
  bool intersect = false;
  float closestDist = 99999.;

  // reset
  dragging = false;
  closest = -1;

  // Test every point to see which is the closest
  for(unsigned int i = 0; i < interceptPoints->size(); i++)
  {
    sphereCenter4 = glm::vec4(interceptPoints[0][i].x,
                              interceptPoints[0][i].y,
                              interceptPoints[0][i].z,
                              1.0);

    //sphereCenter4 = sphereCenter4 * proj;
    sphereCenter4 = sphereCenter4 * modelView;
    sphereCenter = glm::vec3(sphereCenter4.x, sphereCenter4.y, 0.);

    intersect = glm::intersectRaySphere(camPos,
                                        viewRay3,
                                        sphereCenter,
                                        radiusSquared,
                                        intersectPosition,
                                        intersectNormal
                                       );

    if(intersect == true)
    {
      dragging = true;
      start = glm::vec2(x, y);
      if(sphereCenter4.z < closestDist)
      {
        closest = i;
        closestDist = sphereCenter4.z;
      }
    }
  }
  return closest;
}
