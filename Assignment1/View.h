#pragma once

#define GLEW_STATIC
#include <iostream>
#include<stdio.h>
#include<string.h>

#include <GL/glew.h>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>	

/******
	Header file for loading the different vertices, vertex coordinates and vertex normals.
	Code referred from the assignment document.
*******/

enum Type_Render
{
	pointsRender,
	linesRender,
	trianglesRender
};

enum Type
{
	points_standard,
	triangles
};

class Renderer
{
public:
	Renderer(std::string modelAddress) {
		this->m_modelAddress = modelAddress;
		load_obj(this->m_modelAddress);
		centerOfView();
		objectType = triangles;
		objectRenderType = trianglesRender;
	};

	Renderer(std::vector<glm::vec3> pointArray)
	{
		this->vao_vertices.clear();
		this->veo_indices.clear();
		this->faceIndexes.clear();

		this->originalPositions.clear();
		this->originalNormals.clear();
		this->originalCoordinates.clear();

		int i = 0;

		while (i < pointArray.size()) {
			glm::vec3 currentPoint = pointArray[i];
			this->originalPositions.push_back(currentPoint);
			Vertex currentVertex;
			currentVertex.Position = currentPoint;
			this->vao_vertices.push_back(currentVertex);
			i++;
		}

		centerOfView();
		objectType = points_standard;
		objectRenderType = pointsRender;
	};

public:
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	struct vertIndex {
		int positionIndex;
		int normalIndex;
		int textCoordinateIndex;
	};

	struct faceIndex {
		vertIndex vertex[3];
	};

	std::vector<Vertex> vao_vertices;
	std::vector<int> veo_indices;

	std::vector<glm::vec3> originalPositions;
	std::vector<glm::vec3> originalNormals;
	std::vector<glm::vec2> originalCoordinates;

	std::vector<faceIndex> faceIndexes;

	glm::vec3 objectCenter;

	glm::vec4 objectColor = glm::vec4(0.7, 0.7, 0.7, 1.0);
	glm::mat4 objectMatrix = glm::mat4(1.0f);

	std::string m_modelAddress;
	std::string objectName;

	GLfloat sharpness = 25.0f;

	GLuint vao, vbo;

	Type objectType;
	Type_Render objectRenderType;

public:
	void load_obj(std::string modelAddress)
	{
		int path_str_length = modelAddress.size();
		std::string suffix = modelAddress.substr(path_str_length - 3, path_str_length);

		if (suffix == "obj") {
			this->vao_vertices.clear();
			this->veo_indices.clear();
			this->faceIndexes.clear();

			this->originalPositions.clear();
			this->originalNormals.clear();
			this->originalCoordinates.clear();

			std::ifstream ifs;
			try {
				ifs.open(modelAddress);
				std::string one_line;
				while (getline(ifs, one_line)) {
					std::stringstream ss(one_line);
					std::string type;
					ss >> type;
					if (type == "v") {
						glm::vec3 vert_pos;
						ss >> vert_pos[0] >> vert_pos[1] >> vert_pos[2];
						this->originalPositions.push_back(vert_pos);
					}
					else if (type == "vt") {
						glm::vec2 tex_coord;
						ss >> tex_coord[0] >> tex_coord[1];
						this->originalCoordinates.push_back(tex_coord);
					}
					else if (type == "vn") {
						glm::vec3 vert_norm;
						ss >> vert_norm[0] >> vert_norm[1] >> vert_norm[2];
						this->originalNormals.push_back(vert_norm);
					}
					else if (type == "f") {
						faceIndex face_idx;
						for (int i = 0; i < 3; i++) {
							std::string s_vertex;
							ss >> s_vertex;
							int positionIndex = -1;
							int tex_idx = -1;
							int norm_idx = -1;
							sscanf(s_vertex.c_str(), "%d/%d/%d", &positionIndex, &tex_idx, &norm_idx);
							face_idx.vertex[i].positionIndex = positionIndex > 0 ? positionIndex - 1 : -1;
							face_idx.vertex[i].textCoordinateIndex = tex_idx > 0 ? tex_idx - 1 : -1;
							face_idx.vertex[i].normalIndex = norm_idx > 0 ? norm_idx - 1 : -1;
						}
						faceIndexes.push_back(face_idx);
					}
				}
			}
			catch (const std::exception&) {
				std::cout << "Error: Obj file cannot be read\n";
			}
			for (int i = 0; i < faceIndexes.size(); i++) {
				faceIndex cur_idx_face = faceIndexes[i];
				glm::vec3 v0 = originalPositions[cur_idx_face.vertex[0].positionIndex];
				glm::vec3 v1 = originalPositions[cur_idx_face.vertex[1].positionIndex];
				glm::vec3 v2 = originalPositions[cur_idx_face.vertex[2].positionIndex];
				glm::vec3 new_normal = glm::cross(v1 - v0, v2 - v0);

				for (int j = 0; j < 3; j++) {
					Vertex currentVertex;
					vertIndex cur_idx_vertex = cur_idx_face.vertex[j];
					if (cur_idx_vertex.positionIndex >= 0) {
						currentVertex.Position = originalPositions[cur_idx_vertex.positionIndex];
					}
					if (cur_idx_vertex.normalIndex >= 0) {
						currentVertex.Normal = originalNormals[cur_idx_vertex.normalIndex];
					}
					else {
						currentVertex.Normal = new_normal;
					}
					if (cur_idx_vertex.textCoordinateIndex >= 0) {
						currentVertex.TexCoords = originalCoordinates[cur_idx_vertex.textCoordinateIndex];
					}
					vao_vertices.push_back(currentVertex);
					veo_indices.push_back(i * 3 + j);
				}
			}
		}
	};

	void centerOfView()
	{
		glm::vec3 maximum(INT_MIN);
		glm::vec3 minimum(INT_MAX);
		for (auto vertex : this->vao_vertices) {
			maximum[0] = std::max(vertex.Position[0], maximum[0]);
			maximum[1] = std::max(vertex.Position[1], maximum[1]);
			maximum[2] = std::max(vertex.Position[2], maximum[2]);
			minimum[0] = std::min(vertex.Position[0], minimum[0]);
			minimum[1] = std::min(vertex.Position[1], minimum[1]);
			minimum[2] = std::min(vertex.Position[2], minimum[2]);
		}
		this->objectCenter = (maximum + minimum) * 0.5f;

	};
	~Renderer(){}

};