
#include "StdAfx.h"
#include"Our_Model.h"
#include "vec.h"

Our_Model::Our_Model(Renderer* renderer){
	vertex_positions.push_back(vec4(-0.1, -0.1, 0, 2));
	vertex_positions.push_back(vec4(0.1, -0.1, 0, 2));
	vertex_positions.push_back(vec4(0, 0, 0.1, 2));

	vertex_positions.push_back(vec4(0.1, -0.1, 0, 2));
	vertex_positions.push_back(vec4(-0.077, 0.077, 0, 2));
	vertex_positions.push_back(vec4(0, 0, 0.1, 2));

	vertex_positions.push_back(vec4(-0.077, 0.077, 0, 2));
	vertex_positions.push_back(vec4(-0.1, -0.1, 0, 2));
	vertex_positions.push_back(vec4(0, 0, 0.1, 2));


	vertex_positions.push_back(vec4(-0.077, 0.077, 0,2));
	vertex_positions.push_back(vec4(-0.1, -0.1, 0,2));
	vertex_positions.push_back(vec4(0.1, -0.1, 0,2));
	

	normalsToVerticesGeneralForm.push_back(vec3(-1, -1, 0));
	normalsToVerticesGeneralForm.push_back(vec3(1, -1, 0));
	normalsToVerticesGeneralForm.push_back(vec3(0, 0, 1));

	normalsToVerticesGeneralForm.push_back(vec3(1, -1, 0));
	normalsToVerticesGeneralForm.push_back(vec3(-0.77, 0.77, 0));
	normalsToVerticesGeneralForm.push_back(vec3(0, 0, 1));

	normalsToVerticesGeneralForm.push_back(vec3(-0.77, 0.77, 0));
	normalsToVerticesGeneralForm.push_back(vec3(-1, -1, 0));
	normalsToVerticesGeneralForm.push_back(vec3(0, 0, 1));

	normalsToVerticesGeneralForm.push_back(vec3(-0.77, 0.77, 0));
	normalsToVerticesGeneralForm.push_back(vec3(-1, -1, 0));
	normalsToVerticesGeneralForm.push_back(vec3(1, -1, 0));

	//normalsToVerticesGeneralForm.push_back(vec3());


	colors[0] = vec3(256, 0, 0);
	colors[1] = vec3(0, 256, 0);
	colors[2] = vec3(0, 0, 256);
	colors[3] = vec3(256, 256, 256);

	GLfloat minX=-1, minY=-1, minZ=0, maxX=1, maxY=0.77, maxZ=1;
	cube[0] = vec4(minX, minY, minZ, 1.0);
	cube[1] = vec4(minX, minY, maxZ, 1.0);
	cube[2] = vec4(minX, maxY, minZ, 1.0);
	cube[3] = vec4(minX, maxY, maxZ, 1.0);
	cube[4] = vec4(maxX, minY, minZ, 1.0);
	cube[5] = vec4(maxX, minY, maxZ, 1.0);
	cube[6] = vec4(maxX, maxY, minZ, 1.0);
	cube[7] = vec4(maxX, maxY, maxZ, 1.0);


	computeNormalsPerFace();
	

	massCenter = vec4(0, 0, 0.4, 1);

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	VCO[0] = renderer->AddTriangles(&vector<vec4>(vertex_positions.begin(), vertex_positions.begin() + 3), colors[0], &normalsToFacesGeneralForm, &normalsToVerticesGeneralForm, EMISSIVE);
	VCO[1] = renderer->AddTriangles(&vector<vec4>(vertex_positions.begin() + 3, vertex_positions.begin() + 6), colors[1], &normalsToFacesGeneralForm, &vector<vec3>(normalsToVerticesGeneralForm.begin() + 3, normalsToVerticesGeneralForm.begin() + 6), DIFFUSE);
	VCO[2] = renderer->AddTriangles(&vector<vec4>(vertex_positions.begin() + 6, vertex_positions.begin() + 9), colors[2], &normalsToFacesGeneralForm, &vector<vec3>(normalsToVerticesGeneralForm.begin() + 6, normalsToVerticesGeneralForm.begin() + 9), SPECULAR);
	VCO[3] = renderer->AddTriangles(&vector<vec4>(vertex_positions.begin() + 9, vertex_positions.begin() + 12), colors[3], &normalsToFacesGeneralForm, &vector<vec3>(normalsToVerticesGeneralForm.begin() + 9, normalsToVerticesGeneralForm.begin() + 12), ALL);
}


void Our_Model::draw(Renderer* renderer){
	glBindVertexArray(this->VAO);
	renderer->SetObjectMatrices(_world_transform * model_to_world_transform, _normal_transform);

	glBindBuffer(GL_ARRAY_BUFFER, VCO[0]);
	renderer->setColor(colors[0].x, colors[0].y, colors[0].z);
	renderer->draw();
	glBindBuffer(GL_ARRAY_BUFFER, VCO[1]);
	renderer->setColor(colors[1].x, colors[1].y, colors[1].z);
	renderer->draw();
	glBindBuffer(GL_ARRAY_BUFFER, VCO[2]);
	renderer->setColor(colors[2].x, colors[2].y, colors[2].z);
	renderer->draw();
	glBindBuffer(GL_ARRAY_BUFFER, VCO[3]);
	renderer->setColor(colors[3].x, colors[3].y, colors[3].z);
	renderer->draw();

	glBindVertexArray(0);

}
void Our_Model::setModelColor(float R, float G, float B){

}