#include "MarchingCube.h"
#include <math.h>

void MarchingCube::MarchingCubeCore(const float& target_value)
{
	for (int i = 0; i < max_threads; i++)
	{
		threadVerticesCount[i] = 0;
	}
#ifndef _DEBUG
	#pragma omp parallel for num_threads(max_threads) schedule (static)
#endif // _DEBUG
	// ѭ������ÿ�������壬����������ϸ��������
	for (int i = 0; i < cube_size_x - 1; ++i)
	{
		const uint8 tIndex = omp_get_thread_num();
		for (int j = 0; j < cube_size_y - 1; ++j)
		{
			for (int k = 0; k < cube_size_z - 1; ++k)
			{
				Vec3_Int32_SIMD index(i, j, k);
				MarchingCubeCore(tIndex, target_value, index);
			}
		}
	}
}

__forceinline void MarchingCube::MarchingCubeCore(const uint8& threadIndex
	, const float& target_value, const Vec3_Int32_SIMD& index)
{
	const Vec3_Float32_SIMD subdivie_point = Vec3_Float32_SIMD(index) * step_size;	//������
	// �������꣬�����㷨�е�e1-e12���������ֵ��ֱ��ȡ�е�Ļ����ǿ���Ԥ�ȼ���õ�
	Vec3_Float32_SIMD isosurface_vertices_pos[12];
	Vec3_Float32_SIMD isosurface_vertices_normal[12];

	float cube_vertex_value[8]; // cubeÿ�������ֵ
	uint8 cube8_flag_index = 0; // ���cube�Ķ���״̬��flag

	// ��������������cube�����ڱ����ڲ����ⲿ
	for (uint8 i = 0; i < 8; ++i)
	{
		const Vec3_Int32_SIMD offset = a2fVertexOffset[i];
		const uint32 dot = (index + offset).Dot(sizeTemp);
		cube_vertex_value[i] = data[dot];
		//�������������������
		//cube_vertex_value[i] = GetDataUseXYZ(data, x_index + x_offset, y_index + y_offset, z_index + z_offset);

		if (cube_vertex_value[i] <= target_value)
			cube8_flag_index |= 1 << i; //λ����
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
			isosurface_vertices_normal[i] = CalVerticesNormal(index);
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

__forceinline Vec3_Float32_SIMD MarchingCube::CalVerticesNormal(const Vec3_Int32_SIMD& index)const
{
	
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
		//����Ҫ�ȼ��㣬release���Ż�����
		const uint32 x1 = x_index * cube_size_yz + y_index * cube_size_z + z_index - cube_size_yz;
		const uint32 x2 = x_index * cube_size_yz + y_index * cube_size_z + z_index + cube_size_yz;
		const uint32 y1 = x_index * cube_size_yz + y_index * cube_size_z + z_index - cube_size_z;
		const uint32 y2 = x_index * cube_size_yz + y_index * cube_size_z + z_index + cube_size_z;
		const uint32 z1 = x_index * cube_size_yz + y_index * cube_size_z + z_index - 1;
		const uint32 z2 = x_index * cube_size_yz + y_index * cube_size_z + z_index + 1;
		
		Vec3_Float32_SIMD normal(data[x1] - data[x2], data[y1] - data[y2],data[z1] - data[z2]);
		normal.Normalize();
		return normal;
	}

	//�������ٶȲ��
	//normal.x = GetDataUseXYZ(data, x_index - 1, y_index, z_index) - GetDataUseXYZ(data, x_index + 1, y_index, z_index);
	//normal.y = GetDataUseXYZ(data, x_index, y_index - 1, z_index) - GetDataUseXYZ(data, x_index, y_index + 1, z_index);
	//normal.z = GetDataUseXYZ(data, x_index, y_index, z_index - 1) - GetDataUseXYZ(data, x_index, y_index, z_index + 1);
	return Vec3_Float32_SIMD();
}