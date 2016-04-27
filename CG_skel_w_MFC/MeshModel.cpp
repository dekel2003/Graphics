#include "StdAfx.h"
#include "MeshModel.h"
#include "vec.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

struct FaceIdcs
{
	int v[4];
	int vn[4];
	int vt[4];

	FaceIdcs()
	{
		for (int i=0; i<4; i++)
			v[i] = vn[i] = vt[i] = 0;
	}

	FaceIdcs(std::istream & aStream)
	{
		for (int i=0; i<4; i++)
			v[i] = vn[i] = vt[i] = 0;

		char c;
		for(int i = 0; i < 3; i++)
		{
			aStream >> std::ws >> v[i] >> std::ws;
			if (aStream.peek() != '/')
				continue;
			aStream >> c >> std::ws;
			if (aStream.peek() == '/')
			{
				aStream >> c >> std::ws >> vn[i];
				continue;
			}
			else
				aStream >> vt[i];
			if (aStream.peek() != '/')
				continue;
			aStream >> c >> vn[i];
		}
	}
};

vec3 vec3fFromStream(std::istream & aStream)
{
	float x, y, z;
	aStream >> x >> std::ws >> y >> std::ws >> z;
	return vec3(x, y, z);
}

vec2 vec2fFromStream(std::istream & aStream)
{
	float x, y;
	aStream >> x >> std::ws >> y;
	return vec2(x, y);
}

MeshModel::MeshModel(string fileName)
{
	loadFile(fileName);
}

MeshModel::~MeshModel(void)
{
}

void MeshModel::loadFile(string fileName)
{
	ifstream ifile(fileName.c_str());
	vector<FaceIdcs> faces;
	vector<vec3> vertices;
	
	// while not end of file
	vec4 sum = vec4(0, 0, 0, 0);
	while (!ifile.eof())
	{
		// get line
		string curLine;
		getline(ifile, curLine);

		// read type of the line
		istringstream issLine(curLine);
		string lineType;

		issLine >> std::ws >> lineType;

		// based on the type parse data
		if (lineType == "v"){
			vertices.push_back(vec3fFromStream(issLine));
			sum += vertices[vertices.size() - 1];
		}
		else if (lineType == "f")
			faces.push_back(issLine);
		else if (lineType == "vn")
			normals2vertices.push_back(vec3fFromStream(issLine));
		else if (lineType == "#" || lineType == "")
		{
			// comment / empty line
		}
		else
		{
			cout<< "Found unknown line Type \"" << lineType << "\"";
		}
	}
	massCenter = sum / vertices.size();
	//Vertex_positions is an array of vec3. Every three elements define a triangle in 3D.
	//If the face part of the obj is
	//f 1 2 3
	//f 1 3 4
	//Then vertex_positions should contain:
	//vertex_positions={v1,v2,v3,v1,v3,v4}

	num_vertices = 3 * faces.size();
	// vertex_positions = new vec3[num_vertices];
	// iterate through all stored faces and create triangles
	int k=0;
	for (vector<FaceIdcs>::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		//computer normal per face
		vec3 xi = vertices[it->v[0] - 1];
		vec3 xj = vertices[it->v[1] - 1];
		vec3 xk = vertices[it->v[2] - 1];

		vec3 normal = cross((xi - xk), (xj - xk)) / length(cross((xi - xk), (xj - xk)));
		vec3 pointOne = vec3(xi.x + xk.x + xj.x / 20, xi.y + xk.y + xj.y / 20, xi.z + xk.z + xj.z / 20);
		vec3 pointTwo = pointOne + normal/50;
		normalsToFaces.push_back( pair<vec3,vec3>(pointOne, pointTwo));
		//Done computer normal per face
		for (int i = 0; i < 3; i++)
		{
			vertex_positions.push_back(vec4(vertices[it->v[i] - 1].x, vertices[it->v[i] - 1].y, vertices[it->v[i] - 1].z, 1));
		}
	}
}

void MeshModel::draw(Renderer* renderer)
{
	renderer->SetObjectMatrices(_world_transform * model_to_world_transform, _normal_transform);
	renderer->DrawTriangles(&vertex_positions);
	renderer->setColor(200, 100, 50);
	for (vector<pair<vec3, vec3>>::iterator it = normalsToFaces.begin(); it != normalsToFaces.end(); ++it){
		renderer->SetObjectMatrices(_world_transform * model_to_world_transform, _normal_transform);
		renderer->DrawLineBetween3Dvecs(vec4((*it).first, 1), vec4((*it).second, 1));
	}
		
}

void MeshModel::drawAxis(Renderer* renderer)
{
	vec4 camera_massCenter = _world_transform * model_to_world_transform * massCenter;
	renderer->SetObjectMatrices(mat4(), _normal_transform); // For Some reason it did something bad so I changed
	renderer->setColor(255, 0, 0);
	renderer->DrawLineBetween3Dvecs(vec4(camera_massCenter.x, camera_massCenter.y, camera_massCenter.z, 1), vec4(camera_massCenter.x + 1, camera_massCenter.y, camera_massCenter.z, 1));
	renderer->setColor(0, 255, 0);
	renderer->DrawLineBetween3Dvecs(vec4(camera_massCenter.x, camera_massCenter.y, camera_massCenter.z, 1), vec4(camera_massCenter.x, camera_massCenter.y + 1, camera_massCenter.z, 1));
	renderer->setColor(255, 0, 255);
	renderer->DrawLineBetween3Dvecs(vec4(camera_massCenter.x, camera_massCenter.y, camera_massCenter.z, 1), vec4(camera_massCenter.x, camera_massCenter.y, camera_massCenter.z + 1, 1));
	renderer->setColor(255, 255, 255);
}

void MeshModel::setModelTransformation(const mat4& T){
	model_to_world_transform = T * model_to_world_transform;
}

void MeshModel::setWorldTransformation(const mat4& T){
	_world_transform = T * _world_transform;
}

vec4 MeshModel::getOrigin(){
	return massCenter;
}