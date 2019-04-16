#include "MarchingCube.h"
#include <math.h>
#include <Windows.h>

Mesh MarchingCube::MarchingCubeCore(const float& target_value) const
{
	const long maxLength = cube_size_x * cube_size_y * cube_size_z * 3 * 5;
	Mesh mesh;
	mesh.Vertices = new Vec3[maxLength];
	mesh.Normals = new Vec3[maxLength];
	const int subdivide_cube_num_max = max(max(cube_size_x, cube_size_y), cube_size_z);
	const float step_size = 1.0f / subdivide_cube_num_max;

	LARGE_INTEGER start; LARGE_INTEGER end; LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);
		// ѭ������ÿ�������壬����������ϸ��������
		for (int i = 0; i < cube_size_x - 1; ++i)
		{
			for (int j = 0; j < cube_size_y - 1; ++j)
			{
				for (int k = 0; k < cube_size_z - 1; ++k)
				{
					MarchingCubeCore(mesh, target_value, i, j, k, step_size, step_size);
				}
			}
		}
	QueryPerformanceCounter(&end);
	printf("MarchingCube Core: %d ms\n", (int)((end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart));

	return mesh;
}

void MarchingCube::MarchingCubeCore(Mesh& mesh, const float& target_value
	, const  uint16& x_index, const  uint16& y_index, const  uint16& z_index
	, const float& step_size, const float& scale)const
{
	// �������꣬�����㷨�е�e1-e12���������ֵ��ֱ��ȡ�е�Ļ����ǿ���Ԥ�ȼ���õ�
	Vec3 isosurface_vertices_pos[12];
	Vec3 isosurface_vertices_normal[12];

	float cube_vertex_value[8]; // cubeÿ�������ֵ
	int cube8_flag_index = 0; // ���cube�Ķ���״̬��flag
	int edge_flag; // ��ȡcube�Ľ���flag
	float a_offset;
	int triangles_num = 0; // cube��Ӧ�������ε�����

	//������
	const float x = x_index * step_size;
	const float y = y_index * step_size;
	const float z = z_index * step_size;
	const Vec3 subdivie_point(x, y, z);

	// ��������������cube�����ڱ����ڲ����ⲿ
	for (int i = 0; i < 8; ++i)
	{
		int x_offset = 0, y_offset = 0, z_offset = 0;
		if (i == 1 || i == 2 || i == 5 || i == 6)
		{
			if (x_index != cube_size_x - 1)
				x_offset = 1;
			else { cube_vertex_value[i] = 0; continue; } //�����߽綼���0����
		}
		if (i == 2 || i == 3 || i == 6 || i == 7)
		{
			if (y_index != cube_size_y - 1)
				y_offset = 1;
			else { cube_vertex_value[i] = 0; continue; } //�����߽綼���0����
		}
		if (i == 4 || i == 5 || i == 6 || i == 7)
		{
			if (z_index != cube_size_z - 1)
				z_offset = 1;
			else { cube_vertex_value[i] = 0; continue; } //�����߽綼���0����
		}
		x_offset = a2fVertexOffset[i][0];
		y_offset = a2fVertexOffset[i][1];
		z_offset = a2fVertexOffset[i][2]; //������ע�͵��Ĵ��빦��һ��
		cube_vertex_value[i] = GetDataUseXYZ(data, x_index + x_offset, y_index + y_offset, z_index + z_offset);
		if (cube_vertex_value[i] <= target_value)
			cube8_flag_index |= 1 << i; //λ����
	}

	edge_flag = aiCubeEdgeFlags[cube8_flag_index];
	if (edge_flag == 0) //���û����������Ҫ��
	{
		return;
	}

	for (int i = 0; i < 12; ++i) 
	{
		if (edge_flag & (1 << i)) 
		{
			// cubeһ���ߵ����������ֵ֮��
			float delta = cube_vertex_value[cube_edges_indices[i][1]] - cube_vertex_value[cube_edges_indices[i][0]];
			a_offset = 0.5f; //��ֵ����cube�ıߵĽ��㣬ȡcube�ߵ��е�
			if (delta != 0.0)
			{
				a_offset = (target_value - cube_vertex_value[cube_edges_indices[i][0]]) / delta; //������ݱ���ֵ���в�ֵ
			}
			//��ֵ��ÿ�����ϵĽ���
			isosurface_vertices_pos[i].x = x + (cube_vertex_position[cube_edges_indices[i][0]].x
				+ a_offset * cube_edge_direction[i].x) * scale;
			isosurface_vertices_pos[i].y = y + (cube_vertex_position[cube_edges_indices[i][0]].y
				+ a_offset * cube_edge_direction[i].y) * scale;
			isosurface_vertices_pos[i].z = z + (cube_vertex_position[cube_edges_indices[i][0]].z
				+ a_offset * cube_edge_direction[i].z) * scale;
			isosurface_vertices_normal[i] = CalVerticesNormal(x_index, y_index, z_index);
		}
	}

	for (int i = 0; i < 5; ++i) 
	{
		if (a2iTriangleConnectionTable[cube8_flag_index][3 * i] < 0) // <0 �ȼ��� ==-1
		{
			break;
		}
		triangles_num++;
	}

	//cube��Ӧ�����εıߵ���������
	const int triangles_edge_indices_num = triangles_num * 3;
	for (int i = 0; i < triangles_edge_indices_num; ++i)
	{
		mesh.Vertices[mesh.VerticesCount] = isosurface_vertices_pos[a2iTriangleConnectionTable[cube8_flag_index][i]];
		mesh.Normals[mesh.VerticesCount] = isosurface_vertices_normal[a2iTriangleConnectionTable[cube8_flag_index][i]];
		mesh.VerticesCount++;
	}
}

Vec3 MarchingCube::CalVerticesNormal(const uint16& x_index
	, const uint16& y_index, const uint16& z_index)const
{
	Vec3 normal;
	if ((x_index >= 1 && x_index < cube_size_x) &&
		(y_index >= 1 && y_index < cube_size_y) &&
		(z_index >= 1 && z_index < cube_size_z))
	{
		int offset = -1;
		// ���Ĳ��
		normal.x = GetDataUseXYZ(data, x_index + offset, y_index, z_index) - GetDataUseXYZ(data, x_index - offset, y_index, z_index);
		normal.y = GetDataUseXYZ(data, x_index, y_index + offset, z_index) - GetDataUseXYZ(data, x_index, y_index - offset, z_index);
		normal.z = GetDataUseXYZ(data, x_index, y_index, z_index + offset) - GetDataUseXYZ(data, x_index, y_index, z_index - offset);
		normal.normalize();
	}
	return normal;
}

inline float MarchingCube::GetDataUseXYZ(const float * data, const uint16& x
	, const uint16& y, const uint16& z)const
{
	return data[x * cube_size_y * cube_size_z + y * cube_size_z + z];;
}