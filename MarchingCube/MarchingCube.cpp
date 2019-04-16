#include "MarchingCube.h"
#include <math.h>
#include <omp.h>
#define max(a,b)   (((a) > (b)) ? (a) : (b))

Mesh MarchingCube::MarchingCubeCore(const float& target_value)
{
	const float step_size = 1.0f / max(max(cube_size_x, cube_size_y), cube_size_z);
	uint32 VerticesCount = 0;
	// ѭ������ÿ�������壬����������ϸ��������
#ifndef _DEBUG
	const int max_threads = omp_get_max_threads();
	#pragma omp parallel for num_threads(max_threads) schedule (static) shared(VerticesCount)
#endif // _DEBUG
	for (int i = 0; i < cube_size_x - 1; ++i)
	{
		for (int j = 0; j < cube_size_y - 1; ++j)
		{
			for (int k = 0; k < cube_size_z - 1; ++k)
			{
				const uint32 num = MarchingCubeCore(mesh, target_value, i, j, k, step_size, VerticesCount);
				VerticesCount += num;
			}
		}
	}
	printf("VertexCount: %d\n", VerticesCount);
	mesh.VerticesCount = VerticesCount;
	return mesh;
}

__forceinline uint32 MarchingCube::MarchingCubeCore(Mesh& mesh, const float& target_value
	, const uint16& x_index, const uint16& y_index, const uint16& z_index
	, const float& step_size, const uint32 currentCount)const
{
	const Vec3 subdivie_point = Vec3(x_index, y_index, z_index)* step_size;	//������
	// �������꣬�����㷨�е�e1-e12���������ֵ��ֱ��ȡ�е�Ļ����ǿ���Ԥ�ȼ���õ�
	Vec3 isosurface_vertices_pos[12];
	Vec3 isosurface_vertices_normal[12];

	float cube_vertex_value[8]; // cubeÿ�������ֵ
	uint8 cube8_flag_index = 0; // ���cube�Ķ���״̬��flag

	// ��������������cube�����ڱ����ڲ����ⲿ
	for (uint8 i = 0; i < 8; ++i)
	{
		const uint8 x_offset = a2fVertexOffset[i][0];
		const uint8 y_offset = a2fVertexOffset[i][1];
		const uint8 z_offset = a2fVertexOffset[i][2];

		const uint32 index = (x_index + x_offset) * cube_size_yz + (y_index + y_offset) * cube_size_z + z_index + z_offset;
		cube_vertex_value[i] = data[index];	
		//�������������������
		//cube_vertex_value[i] = GetDataUseXYZ(data, x_index + x_offset, y_index + y_offset, z_index + z_offset);

		if (cube_vertex_value[i] <= target_value)
			cube8_flag_index |= 1 << i; //λ����
	}

	uint16 edge_flag = aiCubeEdgeFlags[cube8_flag_index]; // ��ȡcube�Ľ���flag
	if (edge_flag == 0) //���û����������Ҫ��
	{
		return 0;
	}

	for (uint8 i = 0; i < 12; ++i) 
	{
		if (edge_flag & (1 << i)) 
		{
			// cubeһ���ߵ����������ֵ֮��
			const float delta = cube_vertex_value[cube_edges_indices[i][1]] - cube_vertex_value[cube_edges_indices[i][0]];
			float a_offset = 0.5f; //��ֵ����cube�ıߵĽ��㣬ȡcube�ߵ��е�
			if (delta != 0.0)
			{
				a_offset = (target_value - cube_vertex_value[cube_edges_indices[i][0]]) / delta; //������ݱ���ֵ���в�ֵ
			}
			//��ֵ��ÿ�����ϵĽ���
			isosurface_vertices_pos[i] = subdivie_point + (cube_vertex_position[cube_edges_indices[i][0]]
				+ cube_edge_direction[i] * a_offset) * step_size;
			isosurface_vertices_normal[i] = CalVerticesNormal(x_index, y_index, z_index);
		}
	}

	uint8 triangles_num = 0; // cube��Ӧ�������ε�����
	for (uint8 i = 0; i < 5; ++i)
	{
		if (a2iTriangleConnectionTable[cube8_flag_index][3 * i] < 0) // <0 �ȼ��� ==-1
		{
			break;
		}
		triangles_num++;
	}

	//cube��Ӧ�����εıߵ���������
	const uint8 triangles_edge_indices_num = triangles_num * 3;
	for (uint8 i = 0; i < triangles_edge_indices_num; ++i)
	{
		mesh.Vertices[currentCount + i] = isosurface_vertices_pos[a2iTriangleConnectionTable[cube8_flag_index][i]];
		mesh.Normals[currentCount + i] = isosurface_vertices_normal[a2iTriangleConnectionTable[cube8_flag_index][i]];
	}
	return triangles_edge_indices_num;
}

__forceinline Vec3 MarchingCube::CalVerticesNormal(const uint16& x_index
	, const uint16& y_index, const uint16& z_index)const
{
	Vec3 normal;
	//const uint32 standard = x_index * cube_size_yz + y_index * cube_size_z + z_index;
	//const uint32 x1 = standard - cube_size_yz;
	//const uint32 x2 = standard + cube_size_yz;
	//const uint32 y1 = standard - cube_size_z;
	//const uint32 y2 = standard + cube_size_z;
	//const uint32 z1 = standard - 1;
	//const uint32 z2 = standard + 1;
	//normal.x = data[standard - cube_size_yz] - data[standard + cube_size_yz];
	//normal.y = data[standard - cube_size_z] - data[standard + cube_size_z];
	//normal.z = data[standard - 1] - data[standard + 1];

	//const uint32 x1 = x_index * cube_size_yz + y_index * cube_size_z + z_index - cube_size_yz;
	//const uint32 x2 = x_index * cube_size_yz + y_index * cube_size_z + z_index + cube_size_yz;
	//const uint32 y1 = x_index * cube_size_yz + y_index * cube_size_z + z_index - cube_size_z;
	//const uint32 y2 = x_index * cube_size_yz + y_index * cube_size_z + z_index + cube_size_z;
	//const uint32 z1 = x_index * cube_size_yz + y_index * cube_size_z + z_index - 1;
	//const uint32 z2 = x_index * cube_size_yz + y_index * cube_size_z + z_index + 1;
	//
	//normal.x = data[x1] - data[x2];
	//normal.y = data[y1] - data[y2];
	//normal.z = data[z1] - data[z2];
	if ((x_index >= 1 && x_index < cube_size_x) &&
		(y_index >= 1 && y_index < cube_size_y) &&
		(z_index >= 1 && z_index < cube_size_z))
	{
		const uint32 x1 = x_index * cube_size_yz + y_index * cube_size_z + z_index - cube_size_yz;
		const uint32 x2 = x_index * cube_size_yz + y_index * cube_size_z + z_index + cube_size_yz;
		const uint32 y1 = x_index * cube_size_yz + y_index * cube_size_z + z_index - cube_size_z;
		const uint32 y2 = x_index * cube_size_yz + y_index * cube_size_z + z_index + cube_size_z;
		const uint32 z1 = x_index * cube_size_yz + y_index * cube_size_z + z_index - 1;
		const uint32 z2 = x_index * cube_size_yz + y_index * cube_size_z + z_index + 1;

		normal.x = data[x1] - data[x2];
		normal.y = data[y1] - data[y2];
		normal.z = data[z1] - data[z2];
		normal.normalize();
	}

	//�������ٶȲ��
	//normal.x = GetDataUseXYZ(data, x_index - 1, y_index, z_index) - GetDataUseXYZ(data, x_index + 1, y_index, z_index);
	//normal.y = GetDataUseXYZ(data, x_index, y_index - 1, z_index) - GetDataUseXYZ(data, x_index, y_index + 1, z_index);
	//normal.z = GetDataUseXYZ(data, x_index, y_index, z_index - 1) - GetDataUseXYZ(data, x_index, y_index, z_index + 1);
	return normal;
}

__forceinline float MarchingCube::GetDataUseXYZ(const float * data, const uint16& x
	, const uint16& y, const uint16& z)const
{
	const uint32 index = x * cube_size_yz + y * cube_size_z + z;
	return data[index];
}