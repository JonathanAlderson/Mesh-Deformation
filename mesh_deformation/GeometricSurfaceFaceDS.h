///////////////////////////////////////////////////
//
//	Hamish Carr
//	January, 2018
//
//	------------------------
//	GeometricSurfaceFaceDS.h
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

#ifndef _GEOMETRIC_SURFACE_FACE_DS_H
#define _GEOMETRIC_SURFACE_FACE_DS_H

#include "glm.hpp"
#include <vector>

#include "Cartesian3.h"

class GeometricSurfaceFaceDS
	{ // class GeometricSurfaceFaceDS
	public:
	// vectors to store vertex and triangle information - relying on POD rule
	std::vector<Cartesian3> vertices;

	// for interpolation
	std::vector<Cartesian3> weights;
	std::vector<std::vector<int>> neighbours;

	// either 2D or 3D
	int dimensions;
	float hStep;
	float wStep; // for cage size
	float cageX = 3;
	float cageY = 3;
	float cageSize = 1.5; // how much bigger the cage is than the item
	int interpMode = 0;
	// 0


	// vectors to store vertex information on the deformation cage
	// and only on the points the mouse can interact with
	std::vector<Cartesian3> cagePoints;

	// keeping track of the size of the object
	// bounding sphere size
	float boundingSphereSize;
	Cartesian3 minCoords;
	Cartesian3 maxCoords;

	// midpoint of object
	Cartesian3 midPoint;

	// constructor will initialise to safe values
	GeometricSurfaceFaceDS();

	// read routine returns true on success, failure otherwise
	bool ReadFileTriangleSoup(char *fileName);

	// generating cage routine, true on success, false otherwise
	bool GenerateCage(float newCageSize, float newCageX, float newCageY, int mode);

	// different types of cage generation
	bool GenerateGridCage();
	bool GenerateTriangularCage();

	// works out how each point is related
	bool FindInterpolations();

	// routine to render
	void Render();

	// routine to render with interpolation
	void RenderInterpolate();

	// just render the cage
	void RenderCage();

	// different rendering methods for different settings
	void RenderTriangleCage();
	void RenderGridCage();

	// Manipulates the cage
	void updateCagePoint(int id, glm::vec2 values);

	// for FindInterpolations
	Cartesian3 calculateWeights(Cartesian3 v, std::vector<int>);

	// Lerp and anti lerp
	Cartesian3 lerp(Cartesian3 a, Cartesian3 b, float c);
	float invLerp(float a, float b, float c);

	Cartesian3 GetPoint(int i);

	}; // class GeometricSurfaceFaceDS

#endif
