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
	minX = MAXINT, minY = MAXINT, minZ = MAXINT,
		maxX = MININT, maxY = MININT, maxZ = MININT;
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
	vec3 sum = vec3(0, 0, 0);
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

	num_vertices = 3 * faces.size();
	// iterate through all stored faces and create triangles

	int k=0;
	vec3 normal1, normal2, normal3;
	for (vector<FaceIdcs>::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		if (it->vn){
			normal1 = normals2vertices[it->vn[0] - 1];
			normal2 = normals2vertices[it->vn[1] - 1];
			normal3 = normals2vertices[it->vn[2] - 1];
			vec3 point1 = vertices[it->v[0] - 1];
			vec3 point2 = vertices[it->v[1] - 1];
			vec3 point3 = vertices[it->v[2] - 1];
			vec3 point1Two = point1 + normalize(normal1)*normalVectorsSize;
			vec3 point2Two = point2 + normalize(normal2)*normalVectorsSize;
			vec3 point3Two = point3 + normalize(normal3)*normalVectorsSize;
			normalsToVertices.push_back(pair<vec3, vec3>(point1, point1Two));
			normalsToVertices.push_back(pair<vec3, vec3>(point2, point2Two));
			normalsToVertices.push_back(pair<vec3, vec3>(point3, point3Two));
			normalsToVerticesGeneralForm.push_back(normalize(normal1));
			normalsToVerticesGeneralForm.push_back(normalize(normal2));
			normalsToVerticesGeneralForm.push_back(normalize(normal3));
		}
		for (int i = 0; i < 3; i++)
		{
			vertex_positions.push_back(vec4(vertices[it->v[i] - 1].x, vertices[it->v[i] - 1].y, vertices[it->v[i] - 1].z, 1));
			maxX = vertices[it->v[i] - 1].x > maxX ? vertices[it->v[i] - 1].x : maxX;
			maxY = vertices[it->v[i] - 1].y > maxY ? vertices[it->v[i] - 1].y : maxY;
			maxZ = vertices[it->v[i] - 1].z > maxZ ? vertices[it->v[i] - 1].z : maxZ;
			minX = vertices[it->v[i] - 1].x < minX ? vertices[it->v[i] - 1].x : minX;
			minY = vertices[it->v[i] - 1].y < minY ? vertices[it->v[i] - 1].y : minY;
			minZ = vertices[it->v[i] - 1].z < minZ ? vertices[it->v[i] - 1].z : minZ;
		}
	}
	cube[0] = vec4(minX, minY, minZ, 1.0);
	cube[1] = vec4(minX, minY, maxZ, 1.0);
	cube[2] = vec4(minX, maxY, minZ, 1.0);
	cube[3] = vec4(minX, maxY, maxZ, 1.0);
	cube[4] = vec4(maxX, minY, minZ, 1.0);
	cube[5] = vec4(maxX, minY, maxZ, 1.0);
	cube[6] = vec4(maxX, maxY, minZ, 1.0);
	cube[7] = vec4(maxX, maxY, maxZ, 1.0);
	computeNormalsPerFace();
}

void MeshModel::draw(Renderer* renderer)
{
	renderer->SetObjectMatrices(_world_transform * model_to_world_transform, _normal_transform);
	//renderer->DrawTriangles(&vertex_positions);	normals2vertices
	if (normalsToVerticesGeneralForm.size() == 0)
		renderer->AddTriangles(&vertex_positions, color, &normalsToFacesGeneralForm);
	else
		renderer->AddTriangles(&vertex_positions, color, &normalsToFacesGeneralForm, &normalsToVerticesGeneralForm);
}

void MeshModel::drawFaceNormals(Renderer* renderer)
{
	renderer->SetObjectMatrices(_world_transform * model_to_world_transform, _normal_transform);
	renderer->setColor(200, 100, 50);
	for (vector<pair<vec3, vec3>>::iterator it = normalsToFaces.begin(); it != normalsToFaces.end(); ++it){
		//renderer->SetObjectMatrices(_world_transform * model_to_world_transform, _normal_transform);
		renderer->DrawLineBetween3Dvecs(vec4(it->first), vec4(it->second));
	}
}


void MeshModel::drawVertexNormals(Renderer* renderer)
{
	renderer->SetObjectMatrices(_world_transform * model_to_world_transform, _normal_transform);
	renderer->setColor(200, 50, 100);
	for (vector<pair<vec3, vec3>>::iterator it = normalsToVertices.begin(); it != normalsToVertices.end(); ++it){
		//renderer->SetObjectMatrices(_world_transform * model_to_world_transform, _normal_transform);
		renderer->DrawLineBetween3Dvecs(vec4(it->first), vec4(it->second));
	}
}


void MeshModel::drawAxis(Renderer* renderer)
{
	vec4 camera_massCenter = _world_transform * model_to_world_transform * massCenter;
	renderer->SetObjectMatrices(mat4(), _normal_transform); // For Some reason it did something bad so I changed
	renderer->setColor(256, 0, 0);
	renderer->DrawLineBetween3Dvecs(vec4(camera_massCenter.x, camera_massCenter.y, camera_massCenter.z, 1.0), vec4(camera_massCenter.x + 1, camera_massCenter.y, camera_massCenter.z, 1.0));
	renderer->setColor(0, 256, 0);
	renderer->DrawLineBetween3Dvecs(vec4(camera_massCenter.x, camera_massCenter.y, camera_massCenter.z, 1.0), vec4(camera_massCenter.x, camera_massCenter.y + 1, camera_massCenter.z, 1.0));
	renderer->setColor(256, 0, 256);
	renderer->DrawLineBetween3Dvecs(vec4(camera_massCenter.x, camera_massCenter.y, camera_massCenter.z, 1.0), vec4(camera_massCenter.x, camera_massCenter.y, camera_massCenter.z + 1, 1.0));
	renderer->setColor(255, 255, 255);
}

void MeshModel::setModelTransformation(const mat4& T){
	model_to_world_transform = T * model_to_world_transform;
		_normal_transform = T * _normal_transform;
}

void MeshModel::setWorldTransformation(const mat4& T){
	_world_transform = T * _world_transform;
	if (T[0][3] == 0 && T[1][3] == 0 && T[2][3] == 0)
		_normal_transform = T * _normal_transform;
}

void MeshModel::computeNormalsPerFace(){
	for (vector<vec4>::iterator it = vertex_positions.begin(); it != vertex_positions.end(); ++it){
		vec4 xi = (*it++);
		vec4 xj = (*it++);
		vec4 xk = (*it);
		vec3 normal = normalize(cross((xi - xj), (xj - xk)));
		vec3 pointOne = vec3((xi.x + xk.x + xj.x) / 3, (xi.y + xk.y + xj.y) / 3, (xi.z + xk.z + xj.z) / 3);
		vec3 pointTwo = pointOne + normalize(normal) * normalVectorsSize;
		normalsToFaces.push_back(pair<vec3, vec3>(pointOne, pointTwo));
		normalsToFacesGeneralForm.push_back(normalize(pointTwo - pointOne));
	}
}

vec4 MeshModel::getOrigin(){
	return _world_transform * model_to_world_transform * massCenter;
}

vec3 MeshModel::getTopRightFar(){
	return vec3(maxX, maxY, maxZ);
}

vec3 MeshModel::getBottomLeftNear(){
	return vec3(minX, minY, minZ);
}

void MeshModel::drawBoundingBox(Renderer* renderer){
	/*
	mat4 T = _world_transform * model_to_world_transform;
	mat4 T = mat4();
	vec4 _cube[8];
	
	for (int i = 0; i < 8; i++){
		cout << cube[i] << endl;
	}
	*/
	renderer->DrawLineBetween3Dvecs(cube[0], cube[1]);
	renderer->DrawLineBetween3Dvecs(cube[2], cube[3]);
	renderer->DrawLineBetween3Dvecs(cube[4], cube[5]);
	renderer->DrawLineBetween3Dvecs(cube[6], cube[7]);

	renderer->DrawLineBetween3Dvecs(cube[0], cube[2]);
	renderer->DrawLineBetween3Dvecs(cube[1], cube[3]);
	renderer->DrawLineBetween3Dvecs(cube[4], cube[6]);
	renderer->DrawLineBetween3Dvecs(cube[5], cube[7]);

	renderer->DrawLineBetween3Dvecs(cube[0], cube[4]);
	renderer->DrawLineBetween3Dvecs(cube[1], cube[5]);
	renderer->DrawLineBetween3Dvecs(cube[2], cube[6]);
	renderer->DrawLineBetween3Dvecs(cube[3], cube[7]);
}

void MeshModel::setModelColor(float red, float green, float blue){
	color.x = red / 256.0;
	color.y = green / 256.0;
	color.z = blue / 256.0;
}