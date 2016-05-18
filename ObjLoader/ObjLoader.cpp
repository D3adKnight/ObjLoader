// ObjLoader.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

using namespace std;

typedef struct _vec2
{
	float x, y;
} vec2, point2;

typedef struct _vec3
{
	float x, y, z;
} vec3, point3;

typedef struct _face
{
	int vIndex1, vIndex2, vIndex3; // vertex indices
	int tIndex1, tIndex2, tIndex3; // tex indices
	int nIndex1, nIndex2, nIndex3; // normal indices
} face;

typedef struct _vertex
{
	vec3 position;
	vec3 normal;
	vec2 texcoords;
} vertex;

typedef struct _oglModel
{
	union
	{
		vertex* vertices;
		float *vertexBuffer; // packed version
	};
} OglModel;

void copy_vec3(vec3& src, vec3& dest);
void copy_vec2(vec2& src, vec2& dest);
bool LoadObjFile(char* filename, OglModel* model);

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		cout << "Please specify filename!\nExample: ObjLoader.exe \"dark_fighter_4.obj\"" << endl;
	}
	else
	{
		char* filename = argv[1];
		OglModel model;
		LoadObjFile(filename, &model);

		cout << "{" << model.vertexBuffer[0] << " " << model.vertexBuffer[1] << " " << model.vertexBuffer[2] << "} {"
			<< model.vertexBuffer[3] << " " << model.vertexBuffer[4] << " " << model.vertexBuffer[5] << "} {"
			<< model.vertexBuffer[6] << " " << model.vertexBuffer[7] << "}" << endl;
	}
	system("PAUSE");
    return 0;
}

bool LoadObjFile(char* filename, OglModel* model)
{
	ifstream file(filename, ios::in);
	if (!file)
	{
		cout << "Error: Can't open file: " << filename << endl;
		return false;
	}

	vector<vec3> tmp_vertices;
	vector<vec3> tmp_normals;
	vector<vec2> tmp_texcoords;
	vector<face> temp_faces;

	string line;
	while (getline(file, line))
	{
		if (line.substr(0, 2) == "v ")
		{
			istringstream s(line.substr(2));
			vec3 v;
			s >> v.x; s >> v.y; s >> v.z;
			tmp_vertices.push_back(v);
		}
		else if (line.substr(0, 2) == "vt")
		{
			istringstream s(line.substr(2));
			vec2 v;
			s >> v.x; s >> v.y;
			tmp_texcoords.push_back(v);
		}
		else if (line.substr(0, 2) == "vn")
		{
			istringstream s(line.substr(2));
			vec3 v;
			s >> v.x; s >> v.y; s >> v.z;
			tmp_normals.push_back(v);
		}
		else if (line.substr(0, 2) == "f ")
		{
			istringstream s(line.substr(2));
			face f;
			unsigned short vIndex1, vIndex2, vIndex3;
			unsigned short tIndex1, tIndex2, tIndex3;
			unsigned short nIndex1, nIndex2, nIndex3;
			char dummy;
			s >> vIndex1 >> dummy >> tIndex1 >> dummy >> nIndex1; // v1/t1/n1
			s >> vIndex2 >> dummy >> tIndex2 >> dummy >> nIndex2; // v2/t2/n2
			s >> vIndex3 >> dummy >> tIndex3 >> dummy >> nIndex3; // v3/t3/n3
			// shift indexes to zerofirst array and assign to face structure
			f.vIndex1 = vIndex1; f.vIndex2 = vIndex2; f.vIndex3 = vIndex3;
			f.tIndex1 = tIndex1; f.tIndex2 = tIndex2; f.tIndex3 = tIndex3;
			f.nIndex1 = vIndex1; f.nIndex2 = nIndex2; f.nIndex3 = nIndex3;

			f.vIndex1--; f.vIndex2--; f.vIndex3--;
			f.tIndex1--; f.tIndex2--; f.tIndex3--;
			f.nIndex1--; f.nIndex2--; f.nIndex3--;

			temp_faces.push_back(f);
		}
		else if (line[0] == '#')
		{
			cout << "Comment: " << line << endl;
		}
	}

	file.close();

	// assign data to our model
	model->vertices = new vertex[temp_faces.size() * 3];

	// out data for debugging
	cout << "Vertices: " << tmp_vertices.size() << endl;
	cout << "Normals: " << tmp_normals.size() << endl;
	cout << "TexCoords: " << tmp_texcoords.size() << endl;

	for (unsigned int i = 0, j = 0; i < temp_faces.size(); i++, j += 3)
	{
		face f = temp_faces[i];
		// copy one triangle
		// first vertice (position/normal/texcoords)
		copy_vec3(tmp_vertices[f.vIndex1], model->vertices[j].position);
		copy_vec3(tmp_normals[f.nIndex1], model->vertices[j].normal);
		copy_vec2(tmp_texcoords[f.tIndex1], model->vertices[j].texcoords);

		// second vertice
		copy_vec3(tmp_vertices[f.vIndex2], model->vertices[j+1].position);
		copy_vec3(tmp_normals[f.nIndex2], model->vertices[j+1].normal);
		copy_vec2(tmp_texcoords[f.tIndex2], model->vertices[j+1].texcoords);

		// third vertice
		copy_vec3(tmp_vertices[f.vIndex2], model->vertices[j+2].position);
		copy_vec3(tmp_normals[f.nIndex2], model->vertices[j+2].normal);
		copy_vec2(tmp_texcoords[f.tIndex2], model->vertices[j+2].texcoords);
	}

	return true;
}

void copy_vec3(vec3& src, vec3& dest)
{
	dest.x = src.x;
	dest.y = src.y;
	dest.z = src.z;
}

void copy_vec2(vec2& src, vec2& dest)
{
	dest.x = src.x;
	dest.y = src.y;
}