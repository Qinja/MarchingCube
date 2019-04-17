#include "MarchingCube.h"

void MarchingCube::MarchingCubeCore(const float& target_value)
{
	for (int i = 0; i < max_threads; i++)
	{
		threadVerticesCount[i] = 0;
		threadWorkingNum[i] = 0;
	}
#ifndef _DEBUG
#pragma omp parallel for num_threads(max_threads) schedule (static)
#endif // _DEBUG
	// ѭ������ÿ�������壬����������ϸ��������
	for (int i = 1; i < cube_size_x - 1; ++i)
	{
		const uint8 tIndex = omp_get_thread_num();
		const uint16 i_mapping = max_threads * threadWorkingNum[tIndex] + tIndex + 1;
		for (int j = 1; j < cube_size_y - 1; ++j)
		{
			for (int k = 1; k < cube_size_z - 1; ++k)
			{
				MarchingCubeCore(tIndex, target_value, i_mapping, j, k);
			}
		}
		threadWorkingNum[tIndex]++;
	}
}

__forceinline void MarchingCube::MarchingCubeCore(const uint8& threadIndex
	, const float& target_value, const uint16& x_index
	, const uint16& y_index, const uint16& z_index)
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

		if (cube_vertex_value[i] <= target_value)
		{
			cube8_flag_index |= 1 << i; //λ����
		}
	}

	uint16 edge_flag = aiCubeEdgeFlags[cube8_flag_index]; // ��ȡcube�Ľ���flag
	if (edge_flag == 0) //���û����������Ҫ��
	{
		return;
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
	const uint32 currentCount = threadVerticesCount[threadIndex];
	for (uint8 i = 0; i < triangles_edge_indices_num; ++i)
	{
		threadVertices[threadIndex][currentCount + i] = isosurface_vertices_pos[a2iTriangleConnectionTable[cube8_flag_index][i]];
		threadNormals[threadIndex][currentCount + i] = isosurface_vertices_normal[a2iTriangleConnectionTable[cube8_flag_index][i]];
	}
	threadVerticesCount[threadIndex] = currentCount + triangles_edge_indices_num;
}

__forceinline Vec3 MarchingCube::CalVerticesNormal(const uint16& x_index
	, const uint16& y_index, const uint16& z_index)const
{
	const uint32 x1 = x_index * cube_size_yz + y_index * cube_size_z + z_index - cube_size_yz;
	const uint32 x2 = x_index * cube_size_yz + y_index * cube_size_z + z_index + cube_size_yz;
	const uint32 y1 = x_index * cube_size_yz + y_index * cube_size_z + z_index - cube_size_z;
	const uint32 y2 = x_index * cube_size_yz + y_index * cube_size_z + z_index + cube_size_z;
	const uint32 z1 = x_index * cube_size_yz + y_index * cube_size_z + z_index - 1;
	const uint32 z2 = x_index * cube_size_yz + y_index * cube_size_z + z_index + 1;
	Vec3 normal(data[x1] - data[x2], data[y1] - data[y2], data[z1] - data[z2]);
	normal.Normalize();
	return normal;
}