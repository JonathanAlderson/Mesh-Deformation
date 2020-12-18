///////////////////////////////////////////////////
//
//	Hamish Carr
//	January, 2018
//
//	------------------------
//	GeometricSurfaceFaceDS.cpp
//	------------------------
//
//	Base code for geometric assignments.
//
//	This is the minimalistic Face-based D/S for storing
//	surfaces, to be used as the basis for fuller versions
//
//	It will include object load / save code & render code
//
///////////////////////////////////////////////////


#include "GeometricSurfaceFaceDS.h"
#include <iostream>
#include <iterator>
#include <regex>
#include <fstream>
#include <string>
#include <math.h>
#include <string.h>
#include <cmath>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

// constructor will initialise to safe values
GeometricSurfaceFaceDS::GeometricSurfaceFaceDS()
	{ // GeometricSurfaceFaceDS::GeometricSurfaceFaceDS()
	// force the size to nil (should not be necessary, but . . .)
	vertices.resize(0);

	// set this to something reasonable
	boundingSphereSize = 1.0;

	// set the midpoint to the origin
	midPoint = Cartesian3(0.0, 0.0, 0.0);

	// how big the cage is
	cageX = 3;
	cageY = 3;
	cageSize = 1.5; // how much bigger the cage is than the item#
	interpMode = 0;

	} // GeometricSurfaceFaceDS::GeometricSurfaceFaceDS()

// read routine returns true on success, failure otherwise
bool GeometricSurfaceFaceDS::ReadFileTriangleSoup(char *fileName)
	{ // GeometricSurfaceFaceDS::ReadFileTriangleSoup()
	// these are for accumulating a bounding box for the object
	minCoords = Cartesian3(1000000.0, 1000000.0, 1000000.0);
	maxCoords = Cartesian3(-1000000.0, -1000000.0, -1000000.0);

	// reset important things
	boundingSphereSize = 1.0;
	vertices.resize(0);

	// detect the format of the file
	std::ifstream formatCheck(fileName);
	if (formatCheck.bad())
		return false;

	std::string firstLine;
	std::getline (formatCheck, firstLine); // Face count
	std::getline (formatCheck, firstLine); // First line


	// Check how many regex 'gaps' there are
	int len = 0;
	std::regex e(". +");
	std::regex_token_iterator<std::string::iterator> i(firstLine.begin(), firstLine.end(), e, -1);
	std::regex_token_iterator<std::string::iterator> end;
	while (i != end) { len++; *i++; }


	// open the input file
	std::ifstream inFile(fileName);
	if (inFile.bad())
		return false;


	// set the number of vertices and faces
	long nTriangles = 0, nVertices = 0;

	// set the midpoint to the origin
	midPoint = Cartesian3(0.0, 0.0, 0.0);

	// read in the number of vertices
	inFile >> nTriangles;
	nVertices = nTriangles * 3;

	// now allocate space for them all
	vertices.resize(nVertices);

	if(len == 2){ dimensions = 2; std::cout << "Reading 2D shape" << '\n'; }
	else if(len == 3){ dimensions = 3; std::cout << "Reading 3D shape" << '\n'; }
	else{ dimensions = 0; std::cout << "Reading Unknown Shape: " << len << '\n';}

	// now loop to read the vertices in, and hope nothing goes wrong
	for (int vertex = 0; vertex < nVertices; vertex++)
		{ // for each vertex

		if(dimensions == 2)
		{
			inFile >> vertices[vertex].x >> vertices[vertex].y;
				vertices[vertex].z = 0; // set the 2D objects to be on a 3D plane
		}

		if(dimensions == 3)
		{
			inFile >> vertices[vertex].x >> vertices[vertex].y >> vertices[vertex].z;
		}

		// keep running track of midpoint, &c.
		midPoint = midPoint + vertices[vertex];
		if (vertices[vertex].x < minCoords.x) minCoords.x = vertices[vertex].x;
		if (vertices[vertex].y < minCoords.y) minCoords.y = vertices[vertex].y;
		if (vertices[vertex].z < minCoords.z) minCoords.z = vertices[vertex].z;

		if (vertices[vertex].x > maxCoords.x) maxCoords.x = vertices[vertex].x;
		if (vertices[vertex].y > maxCoords.y) maxCoords.y = vertices[vertex].y;
		if (vertices[vertex].z > maxCoords.z) maxCoords.z = vertices[vertex].z;
		} // for each vertex

	// now sort out the size of a bounding sphere for viewing
	// and also set the midpoint's location
	midPoint = midPoint / vertices.size();

	// now go back through the vertices, subtracting the mid point
	for (int vertex = 0; vertex < nVertices; vertex++)
		{ // per vertex
		vertices[vertex] = vertices[vertex] - midPoint;
		} // per vertex

	// the bounding sphere radius is just half the distance between these
	boundingSphereSize = sqrt((maxCoords - minCoords).length()) * 1.0;

	return true;
	} // GeometricSurfaceFaceDS::ReadFileTriangleSoup()

// generate a cage either in 2D or 3D
// that is either triangular or square based
bool GeometricSurfaceFaceDS::GenerateCage(float cs, float cx, float cy, int cm)
{
	// update if a new size/scale is provided
	if(cs != -1.){ this->cageSize = cs; }
	if(cx != -1.){ this->cageX = cx; }
	if(cy != -1.){ this->cageY = cy; }
	if(cm != -1.){ this->interpMode = cm; }

	if(interpMode == 0)
	{
		return GenerateGridCage();
	}
	if(interpMode == 1)
	{
		return GenerateTriangularCage();
	}

	return false;
}

bool GeometricSurfaceFaceDS::GenerateTriangularCage()
{
	std::cout << "Generating triangle cage" << '\n';
	cagePoints.clear();

	std::cout << "CS: " << cageSize << '\n';
	std::cout << midPoint << '\n';
	std::cout << minCoords << '\n';
	std::cout << maxCoords << '\n';
	wStep = (maxCoords.x - minCoords.x) * cageSize * 1./cageX;
	hStep = (maxCoords.y - minCoords.y) * cageSize * 1./cageY;

	float x = (minCoords.x + (maxCoords.x - minCoords.x) * 0.5) - wStep * cageX/2.;
	float y = (minCoords.y + (maxCoords.y - minCoords.y) * 0.5) - hStep * cageY/2.;

	int rows = cageY;
	if((int)cageY % 2 == 0){ rows++; }

	for(int i = 0; i < (int)rows; i ++)
	{
		x = midPoint.x - wStep * cageX/2.;

		for(int j = 0; j < (int)cageX; j ++)
		{
			// do offset on triangles every other row
			if(i%2 == 1 && j == 0){ x += wStep * .5; j++; }
			cagePoints.push_back(Cartesian3(x, y, 0));
			x += wStep;
		}
		y += hStep;
	}

	std::cout << "Cage Generated: " << cagePoints.size() << '\n';
	return true;
}

bool GeometricSurfaceFaceDS::GenerateGridCage()
{
	std::cout << "\n\n\nGenerating Grid Cage" << '\n';
	std::cout << "CS: " << cageSize << '\n';
	std::cout << midPoint << '\n';
	std::cout << minCoords << '\n';
	std::cout << maxCoords << '\n';

	wStep = (maxCoords.x - minCoords.x) * cageSize * 1./cageX;
	hStep = (maxCoords.y - minCoords.y) * cageSize * 1./cageY;

	float x = (minCoords.x + (maxCoords.x - minCoords.x) * 0.5) - wStep * cageX/2.;
	float y = (minCoords.y + (maxCoords.y - minCoords.y) * 0.5) - hStep * cageY/2.;

	// reset the cage
	cagePoints.clear();

	// step through all the vertices to make a cage
	for(int i = 0; i < (int)cageY + 1; i ++)
	{
		x = midPoint.x - wStep * cageX/2.;
		for(int j = 0; j < (int)cageX; j ++)
		{
			// mouse can interact with
			cagePoints.push_back(Cartesian3(x, y, 0));
			x += wStep;
		}
		cagePoints.push_back(Cartesian3(x, y, 0));
		y += hStep;
	}

	return true;
}


bool GeometricSurfaceFaceDS::FindInterpolations()
{
	std::cout << "Cage Points" << '\n';
	for(unsigned int i = 0; i < cagePoints.size(); i++)
	{
		std::cout << cagePoints[i].x << " " << cagePoints[i].y << " " << cagePoints[i].z << " " << '\n';
	}
	std::cout << "  " << '\n';
	std::vector<int> closest;
	std::vector<float> dist;
	std::vector<float> allDist;
	float thisDist;

	// reset
	weights.clear();
	neighbours.clear();

	for(unsigned int i = 0; i < vertices.size(); i ++)
	{
		// reset things
		closest.clear();
		dist.clear();
		allDist.clear();

		// find the distance to each cagePoint
		for(unsigned int j = 0; j < cagePoints.size(); j++)
		{
			allDist.push_back(vertices[i].distance(cagePoints[j]));
		}

		std::sort (allDist.begin(), allDist.end());

		for(unsigned int j = 0; j < cagePoints.size(); j++)
		{
			thisDist = vertices[i].distance(cagePoints[j]);
			std::cout << "V: " << vertices[i].x << " " << vertices[i].y <<"  Dist: " << thisDist << '\n';
			// check to see if this distance is in the top 4
			for(unsigned int k = 0; k < 4; k++)
			{
				// if it's there, put the correct indexes in
				if(thisDist == allDist[k])
				{
					dist.push_back(thisDist);
					closest.push_back(j);
					break;
				}
			}

			// print
			//for(unsigned int k = 0; k < closest.size(); k++)
		//	{
		//		std::cout << closest[k] << " ";
		//	}
		//	std::cout << "   " << '\n';

		}
		//for(unsigned int k = 0; k < 4; k ++)
		//{
		//	std::cout << closest[k] << "    ";
		//}
		//std::cout << " " << '\n';
		// add this vertices neighbours to the list
		neighbours.push_back(closest);
		weights.push_back(calculateWeights(vertices[i], neighbours[i]));
		//std::cout << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << " " << '\n';
	}
	return true;
}

float GeometricSurfaceFaceDS::invLerp(float a, float b, float c)
{
	// avoids nasty 0 errors
	if(b - a == 0.){ std::cout << "Err" << a << " " << b << " " << c << '\n'; }

	return ((c - a) / (b - a));
}

// Linearly interpolates between 2 cartesian 3 coordinates
Cartesian3 GeometricSurfaceFaceDS::lerp(Cartesian3 a, Cartesian3 b, float c)
{
	  float x = a.x + c * (b.x - a.x);
		float y = a.y + c * (b.y - a.y);
		float z = a.z + c * (b.z - a.z);
    return Cartesian3(x, y, z);
}

Cartesian3 GeometricSurfaceFaceDS::calculateWeights(Cartesian3 v, std::vector<int> p)
{
	// 0 top left
	// 1 top right
	// 2 bottom left
	// 3 bottom right
	float alpha, beta, gamma;

	//std::cout << "Calculate Weights" << '\n' << '\n' << '\n';

	//for(unsigned int i = 0; i < 4; i ++){ std::cout << cagePoints[p[i]].x << " " << cagePoints[p[i]].y << " " << cagePoints[p[i]].z << "\n"; }
	//std::cout << "" << '\n';

	// find alpha value
	alpha = invLerp(cagePoints[p[0]].x, cagePoints[p[1]].x, v.x); // horizontal
	beta = invLerp(cagePoints[p[0]].y, cagePoints[p[3]].y, v.y);  // vertical
	gamma = 0.;

	//std::cout << "a: " << alpha << " b: " << beta << '\n';

	return Cartesian3(alpha, beta, gamma);
}

// routine to render
void GeometricSurfaceFaceDS::Render()
	{ // GeometricSurfaceFaceDS::Render()
	// walk through the faces rendering each one
	glBegin(GL_TRIANGLES);

	// we will loop in 3's, assuming CCW order
	for (unsigned int vertex = 0; vertex < vertices.size(); )
		{ // per triangle
		// use increment to step through them
		Cartesian3 *v0 = &(vertices[vertex++]);
		Cartesian3 *v1 = &(vertices[vertex++]);
		Cartesian3 *v2 = &(vertices[vertex++]);
		// now compute the normal vector
		Cartesian3 uVec = *v1 - *v0;
		Cartesian3 vVec = *v2 - *v0;
		Cartesian3 normal = uVec.cross(vVec).normalise();

		glNormal3fv(&normal.x);
		glVertex3fv(&v0->x);
		glVertex3fv(&v1->x);
		glVertex3fv(&v2->x);
		} // per triangle
	glEnd();
} // GeometricSurfaceFaceDS::Render()


Cartesian3 GeometricSurfaceFaceDS::GetPoint(int i)
{
	std::vector<int> n = neighbours[i];

	Cartesian3 top = lerp(cagePoints[n[0]], cagePoints[n[1]], weights[i].x);
	Cartesian3 bottom = lerp(cagePoints[n[2]], cagePoints[n[3]], weights[i].x);

	Cartesian3 final = lerp(top, bottom, weights[i].y);

	return final;

}

// routine to render
// but with in the deformed embedding space
void GeometricSurfaceFaceDS::RenderInterpolate()
	{ // GeometricSurfaceFaceDS::Render()
	// walk through the faces rendering each one
	glBegin(GL_TRIANGLES);

	// we will loop in 3's, assuming CCW order
	for (unsigned int vertex = 0; vertex < vertices.size(); vertex += 3)
		{ // per triangle
		// use increment to step through them
		Cartesian3 v0 = GetPoint(vertex);
		Cartesian3 v1 = GetPoint(vertex + 1);
		Cartesian3 v2 = GetPoint(vertex + 2);

		Cartesian3 *v0Ptr = &v0;
		Cartesian3 *v1Ptr = &v1;
		Cartesian3 *v2Ptr = &v2;
		// now compute the normal vector
		Cartesian3 uVec = *v1Ptr - *v0Ptr;
		Cartesian3 vVec = *v2Ptr - *v0Ptr;
		Cartesian3 normal = uVec.cross(vVec).normalise();

		glNormal3fv(&normal.x);
		glVertex3fv(&v0Ptr->x);
		glVertex3fv(&v1Ptr->x);
		glVertex3fv(&v2Ptr->x);
		} // per triangle
	glEnd();
} // GeometricSurfaceFaceDS::Render()



void GeometricSurfaceFaceDS::RenderCage()
{
	if(interpMode == 0)
	{
		RenderGridCage();
	}

	if(interpMode == 1)
	{
		RenderTriangleCage();
	}



}

// Renders the editable points in a triangle structure
void GeometricSurfaceFaceDS::RenderTriangleCage()
{
	int idx; // index

	int row = cageX;


	idx = 0;


	for(int k = 0; k < row/2; k++)
	{
		glBegin(GL_LINE_STRIP);

		// first sideways triangle
		glVertex3f(cagePoints[idx].x, cagePoints[idx].y, cagePoints[idx].z);
		idx += row * 2 - 1;
		glVertex3f(cagePoints[idx].x, cagePoints[idx].y, cagePoints[idx].z);
		idx -= row - 1;
		glVertex3f(cagePoints[idx].x, cagePoints[idx].y, cagePoints[idx].z);
		idx -= row;
		glVertex3f(cagePoints[idx].x, cagePoints[idx].y, cagePoints[idx].z);

		// generate normal triangles
		for(int i = 0; i < row - 1; i ++)
		{
			glVertex3f(cagePoints[idx].x, cagePoints[idx].y, cagePoints[idx].z);
			idx += row;
			glVertex3f(cagePoints[idx].x, cagePoints[idx].y, cagePoints[idx].z);
			idx -= row - 1;
			glVertex3f(cagePoints[idx].x, cagePoints[idx].y, cagePoints[idx].z);
			idx -= 1;
			glVertex3f(cagePoints[idx].x, cagePoints[idx].y, cagePoints[idx].z);
			idx ++;
		}
		glEnd();
		glBegin(GL_LINE_STRIP);

		// do upside down triangles
		idx ++;
		for(int i = 0; i < row - 2; i++)
		{
			glVertex3f(cagePoints[idx].x, cagePoints[idx].y, cagePoints[idx].z);
			idx += row;
			glVertex3f(cagePoints[idx].x, cagePoints[idx].y, cagePoints[idx].z);
			idx -= row - 1;
			glVertex3f(cagePoints[idx].x, cagePoints[idx].y, cagePoints[idx].z);
			idx -= 1;
			glVertex3f(cagePoints[idx].x, cagePoints[idx].y, cagePoints[idx].z);
			idx++;
		}

		// generate last sideways triangle
		glVertex3f(cagePoints[idx].x, cagePoints[idx].y, cagePoints[idx].z);
		idx += row;
		glVertex3f(cagePoints[idx].x, cagePoints[idx].y, cagePoints[idx].z);
		idx -= row * 2 - 1;
		glVertex3f(cagePoints[idx].x, cagePoints[idx].y, cagePoints[idx].z);
		idx += row - 1;
		glVertex3f(cagePoints[idx].x, cagePoints[idx].y, cagePoints[idx].z);

		glEnd();
		idx++;

	}
	glBegin(GL_LINE_STRIP);

	//draw the last line
	for(int i = 0; i < row; i++)
	{
		glVertex3f(cagePoints[idx].x, cagePoints[idx].y, cagePoints[idx].z);
		idx++;
	}

	glEnd();


}


// Renders the editable points in a grid structure
void GeometricSurfaceFaceDS::RenderGridCage()
{
	int idx; // index
	for(int j = 0; j < cageY ; j++)
	{
		glBegin(GL_LINE_STRIP);
		for(int i = 0; i < cageX ; i++)
		{
			idx = i + j * (cageX + 1);
			// self
			glVertex3f(cagePoints[idx].x, cagePoints[idx].y, cagePoints[idx].z);
			// right
			glVertex3f(cagePoints[idx + 1].x, cagePoints[idx + 1].y, cagePoints[idx + 1].z);
			// down right
			glVertex3f(cagePoints[idx + 2 + cageX].x, cagePoints[idx + 2 + cageX].y, cagePoints[idx + 2 + cageX].z);
			// down left
			glVertex3f(cagePoints[idx + 1 + cageX].x, cagePoints[idx + 1 + cageX].y, cagePoints[idx + 1 + cageX].z);
			// self
			glVertex3f(cagePoints[idx].x, cagePoints[idx].y, cagePoints[idx].z);
		}
		glEnd();
	}
}





void 	GeometricSurfaceFaceDS::updateCagePoint(int id, glm::vec2 values)
{

	id = ((cageY + 1) * (cageX + 1)) - id;
	int row = (id - 1) / (cageX + 1);
	int column = cageX - (id - row * (cageX + 1)) + cageX - 2;


	cagePoints[row * (cageX + 1) + column].x -= values.x;
	cagePoints[row * (cageX + 1) + column].y -= values.y;
	//std::cout << id <<  " " <<values.x << " " << values.y << '\n';
}
