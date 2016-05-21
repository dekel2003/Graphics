
#include "StdAfx.h"
#include"Our_Model.h"
#include "vec.h"

Our_Model::Our_Model(){
	vertex_positions.push_back(vec4(-1, -1, 0, 1));
	vertex_positions.push_back(vec4(1, -1, 0, 1));
	vertex_positions.push_back(vec4(0, 0, 1, 1));

	vertex_positions.push_back(vec4(1, -1, 0, 1));
	vertex_positions.push_back(vec4(-0.77, 0.77, 0, 1));
	vertex_positions.push_back(vec4(0, 0, 1, 1));

	vertex_positions.push_back(vec4(-0.77, 0.77, 0, 1));
	vertex_positions.push_back(vec4(-1, -1, 0, 1));
	vertex_positions.push_back(vec4(0, 0, 1, 1));

	

	normalsToVerticesGeneralForm.push_back(vec3(-1, -1, 0));
	normalsToVerticesGeneralForm.push_back(vec3(1, -1, 0));
	normalsToVerticesGeneralForm.push_back(vec3(0, 0, 1));

	normalsToVerticesGeneralForm.push_back(vec3(1, -1, 0));
	normalsToVerticesGeneralForm.push_back(vec3(-0.77, 0.77, 0));
	normalsToVerticesGeneralForm.push_back(vec3(0, 0, 1));

	normalsToVerticesGeneralForm.push_back(vec3(-0.77, 0.77, 0));
	normalsToVerticesGeneralForm.push_back(vec3(-1, -1, 0));
	normalsToVerticesGeneralForm.push_back(vec3(0, 0, 1));




	colors[0] = vec3(256, 0, 0);
	colors[1] = vec3(0, 256, 0);
	colors[2] = vec3(0, 0, 256);

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
}


void Our_Model::draw(Renderer* renderer){
	renderer->SetObjectMatrices(_world_transform * model_to_world_transform, _normal_transform);
	//renderer->DrawTriangles(&vertex_positions);	normals2vertices
	if (normalsToVerticesGeneralForm.size() == 0){
		renderer->AddTriangles(&vector<vec4>(&vertex_positions[0], &vertex_positions[2]), colors[0], &normalsToFacesGeneralForm);
		renderer->AddTriangles(&vector<vec4>(&vertex_positions[3], &vertex_positions[5]), colors[1], &normalsToFacesGeneralForm);
		renderer->AddTriangles(&vector<vec4>(&vertex_positions[6], &vertex_positions[8]), colors[2], &normalsToFacesGeneralForm);
	}
	else{
		renderer->AddTriangles(&vector<vec4>(&vertex_positions[0], &vertex_positions[2]), colors[0], &normalsToFacesGeneralForm, &normalsToVerticesGeneralForm);
		renderer->AddTriangles(&vector<vec4>(&vertex_positions[3], &vertex_positions[5]), colors[1], &normalsToFacesGeneralForm, &normalsToVerticesGeneralForm);
		renderer->AddTriangles(&vector<vec4>(&vertex_positions[6], &vertex_positions[8]), colors[2], &normalsToFacesGeneralForm, &normalsToVerticesGeneralForm);
	}
		
}
void Our_Model::setModelColor(float R, float G, float B){

}