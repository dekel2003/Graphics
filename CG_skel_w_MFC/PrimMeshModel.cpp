#include "StdAfx.h"
#include "MeshModel.h"
#include "vec.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

class PrimMeshModel : public MeshModel {
public:
	void setSphere(Renderer* renderer){
		minX = MAXINT, minY = MAXINT, minZ = MAXINT,
			maxX = MININT, maxY = MININT, maxZ = MININT;
		generateIcosahedronVertices();
		tesselateIcosahedron(2);
		computeNormalsPerFace();

		if (normalsToVerticesGeneralForm.size() == 0)
			VBO = renderer->AddTriangles(&vertex_positions, color, &normalsToFacesGeneralForm);
		else
			VBO = renderer->AddTriangles(&vertex_positions, color, &normalsToFacesGeneralForm, &normalsToVerticesGeneralForm);
	}

private:

	void generateIcosahedronVertices(){
		float t = (1.0f + sqrt(5.0f)) / 2.0f;
		vector<vec4> vertices;

		vertices.push_back(refineVector(vec4(-1, t, 0, 1)));
		vertices.push_back(refineVector(vec4(1, t, 0, 1)));
		vertices.push_back(refineVector(vec4(-1, -t, 0, 1)));
		vertices.push_back(refineVector(vec4(1, -t, 0, 1)));

		vertices.push_back(refineVector(vec4(0, -1, t, 1)));
		vertices.push_back(refineVector(vec4(0, 1, t, 1)));
		vertices.push_back(refineVector(vec4(0, -1, -t, 1)));
		vertices.push_back(refineVector(vec4(0, 1, -t, 1)));

		vertices.push_back(refineVector(vec4(t, 0, -1, 1)));
		vertices.push_back(refineVector(vec4(t, 0, 1, 1)));
		vertices.push_back(refineVector(vec4(-t, 0, -1, 1)));
		vertices.push_back(refineVector(vec4(-t, 0, 1, 1)));

		//inserting to vertex_positions
		vertex_positions.push_back(vertices[0]);
		vertex_positions.push_back(vertices[5]);
		vertex_positions.push_back(vertices[11]);

		vertex_positions.push_back(vertices[0]);
		vertex_positions.push_back(vertices[5]);
		vertex_positions.push_back(vertices[1]);

		vertex_positions.push_back(vertices[0]);
		vertex_positions.push_back(vertices[1]);
		vertex_positions.push_back(vertices[7]);

		vertex_positions.push_back(vertices[0]);
		vertex_positions.push_back(vertices[7]);
		vertex_positions.push_back(vertices[10]);

		vertex_positions.push_back(vertices[0]);
		vertex_positions.push_back(vertices[10]);
		vertex_positions.push_back(vertices[11]);

		vertex_positions.push_back(vertices[1]);
		vertex_positions.push_back(vertices[5]);
		vertex_positions.push_back(vertices[9]);

		vertex_positions.push_back(vertices[5]);
		vertex_positions.push_back(vertices[11]);
		vertex_positions.push_back(vertices[4]);

		vertex_positions.push_back(vertices[11]);
		vertex_positions.push_back(vertices[10]);
		vertex_positions.push_back(vertices[2]);

		vertex_positions.push_back(vertices[10]);
		vertex_positions.push_back(vertices[7]);
		vertex_positions.push_back(vertices[6]);

		vertex_positions.push_back(vertices[7]);
		vertex_positions.push_back(vertices[1]);
		vertex_positions.push_back(vertices[8]);

		vertex_positions.push_back(vertices[3]);
		vertex_positions.push_back(vertices[9]);
		vertex_positions.push_back(vertices[4]);

		vertex_positions.push_back(vertices[3]);
		vertex_positions.push_back(vertices[4]);
		vertex_positions.push_back(vertices[2]);

		vertex_positions.push_back(vertices[3]);
		vertex_positions.push_back(vertices[2]);
		vertex_positions.push_back(vertices[6]);

		vertex_positions.push_back(vertices[3]);
		vertex_positions.push_back(vertices[6]);
		vertex_positions.push_back(vertices[8]);

		vertex_positions.push_back(vertices[3]);
		vertex_positions.push_back(vertices[8]);
		vertex_positions.push_back(vertices[9]);

		vertex_positions.push_back(vertices[4]);
		vertex_positions.push_back(vertices[9]);
		vertex_positions.push_back(vertices[5]);

		vertex_positions.push_back(vertices[2]);
		vertex_positions.push_back(vertices[4]);
		vertex_positions.push_back(vertices[11]);

		vertex_positions.push_back(vertices[6]);
		vertex_positions.push_back(vertices[2]);
		vertex_positions.push_back(vertices[10]);

		vertex_positions.push_back(vertices[8]);
		vertex_positions.push_back(vertices[6]);
		vertex_positions.push_back(vertices[7]);

		vertex_positions.push_back(vertices[9]);
		vertex_positions.push_back(vertices[8]);
		vertex_positions.push_back(vertices[1]);
	}

	vec4  calculateMidPoint(vec4 vec1, vec4 vec2){
		vec4 ret= vec4((vec1.x + vec2.x) / 2, (vec1.y + vec2.y) / 2, (vec1.z + vec2.z) / 2, 1);
		return refineVector(ret);
	}

	void tesselateIcosahedron(int tesselationLevel)
	{
		vec3 sum = vec3(0, 0, 0);
		int size = 12;
		for (int i = 0; i < tesselationLevel; i++)
		{
			vector<vec4> newVertices;
			for (vector<vec4>::iterator it = vertex_positions.begin(); it != vertex_positions.end();)
			{
				vec4 v1 = (*it++);
				vec4 v2 = (*it++);
				vec4 v3 = (*it++);

				

				vec4 a = calculateMidPoint(v1, v2);
				vec4 b = calculateMidPoint(v2, v3);
				vec4 c = calculateMidPoint(v3, v1);
				size += 3;
				vec4 vecArr[6];
				vecArr[0] = v1, vecArr[1] = v2, vecArr[2] = v3, vecArr[3] = a, vecArr[4] = b, vecArr[5] = c; 
				for (int i = 0; i < 6; i++){
					maxX = vecArr[i].x > maxX ? vecArr[i].x : maxX;
					maxY = vecArr[i].y > maxY ? vecArr[i].y : maxY;
					maxZ = vecArr[i].z > maxZ ? vecArr[i].z : maxZ;
					minX = vecArr[i].x < minX ? vecArr[i].x : minX;
					minY = vecArr[i].y < minY ? vecArr[i].y : minY;
					minZ = vecArr[i].z < minZ ? vecArr[i].z : minZ;
					sum += vec3(vecArr[i].x, vecArr[i].y, vecArr[i].z);
				}

				//Pushing new triangles
				newVertices.push_back(v1);
				newVertices.push_back(a);
				newVertices.push_back(c);

				newVertices.push_back(v2);
				newVertices.push_back(a);
				newVertices.push_back(b);

				newVertices.push_back(v3);
				newVertices.push_back(b);
				newVertices.push_back(c);

				newVertices.push_back(a);
				newVertices.push_back(b);
				newVertices.push_back(c);
			}
			vertex_positions = newVertices;
		}
		massCenter = sum / size;

		cube[0] = vec4(minX, minY, minZ, 1.0);
		cube[1] = vec4(minX, minY, maxZ, 1.0);
		cube[2] = vec4(minX, maxY, minZ, 1.0);
		cube[3] = vec4(minX, maxY, maxZ, 1.0);
		cube[4] = vec4(maxX, minY, minZ, 1.0);
		cube[5] = vec4(maxX, minY, maxZ, 1.0);
		cube[6] = vec4(maxX, maxY, minZ, 1.0);
		cube[7] = vec4(maxX, maxY, maxZ, 1.0);
	}

	vec4 refineVector(vec4 p)
	{
		double length = sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
		return vec4(p.x / length, p.y / length, p.z / length, 1);
	}

};