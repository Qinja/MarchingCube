#include "MarchingCube.h"

void MarchingCube::MarchingCubeCore(const float& target_value)
{
	for (char i = 0; i < max_threads; i++)
	{
		threadVerticesCount[i] = 0;
		threadWorkingNum[i] = 0;
	}
#ifndef _DEBUG
#pragma omp parallel for num_threads(max_threads) schedule (static)
#endif // _DEBUG
	// 循环遍历每个立方体，立方体数由细分数决定
	for (short i = 1; i < cube_size_x - 1; ++i)
	{
		const uint8 tIndex = omp_get_thread_num();
		const uint16 i_mapping = max_threads * threadWorkingNum[tIndex] + tIndex + 1;
		for (uint16 j = 1; j < cube_size_y - 1; ++j)
		{
			for (uint16 k = 1; k < cube_size_z - 1; ++k)
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
	float cube_vertex_value[8]; // cube每个顶点的值
	uint8 cube8_flag_index = 0; // 标记cube的顶点状态的flag

	// 创建样本并查找cube顶点在表面内部或外部
	for (uint8 i = 0; i < 8; ++i)
	{
		const uint8* offset = a2fVertexOffset[i];
		const uint32 index = (x_index + offset[0]) * cube_size_yz + (y_index + offset[1]) * cube_size_z + z_index + offset[2];
		cube_vertex_value[i] = data[index];
		if (cube_vertex_value[i] < target_value)
		{
			cube8_flag_index |= 1 << i; //位运算
		}
	}

	if (cube8_flag_index == 0x00 || cube8_flag_index == 0xff) //如果没有三角形需要画	if (edge_flag == 0)
	{
		return;
	}

	// 交点坐标，对于算法中的e1-e12。如果不插值的直接取中点的话，是可以预先计算好的
	const Vec3 subdivie_point = Vec3(x_index, y_index, z_index)* step_size;	//样本点
	Vec3 isosurface_vertices_pos[12];
	Vec3 isosurface_vertices_normal[12];

	uint16 edge_flag = aiCubeEdgeFlags[cube8_flag_index]; // 获取cube的交点flag

	for (uint8 i = 0; i < 12; ++i)
	{
		if (edge_flag & (1 << i))
		{
			// cube一条边的两个顶点的值之差
			const float delta = cube_vertex_value[cube_edges_indices[i][1]] - cube_vertex_value[cube_edges_indices[i][0]];
			const float a_offset = (target_value - cube_vertex_value[cube_edges_indices[i][0]]) / delta; //交点根据标量值进行插值
			//插值出每条边上的交点
			isosurface_vertices_pos[i] = subdivie_point + (cube_vertex_position[cube_edges_indices[i][0]]
				+ cube_edge_direction[i] * a_offset) * step_size;

			const uint32 x1 = x_index * cube_size_yz + y_index * cube_size_z + z_index - cube_size_yz;
			const uint32 x2 = x_index * cube_size_yz + y_index * cube_size_z + z_index + cube_size_yz;
			const uint32 y1 = x_index * cube_size_yz + y_index * cube_size_z + z_index - cube_size_z;
			const uint32 y2 = x_index * cube_size_yz + y_index * cube_size_z + z_index + cube_size_z;
			const uint32 z1 = x_index * cube_size_yz + y_index * cube_size_z + z_index - 1;
			const uint32 z2 = x_index * cube_size_yz + y_index * cube_size_z + z_index + 1;
			isosurface_vertices_normal[i] = Vec3(data[x1] - data[x2], data[y1] - data[y2], data[z1] - data[z2]);
			isosurface_vertices_normal[i].Normalize();
		}
	}

	const uint32 currentCount = threadVerticesCount[threadIndex];
	uint8 triangles_num;
	for (triangles_num = 0; triangles_num < 15; triangles_num += 3)
	{
		if (a2iTriangleConnectionTable[cube8_flag_index][triangles_num] == 0xff) // 无三角形
		{
			break;
		}
		threadVertices[threadIndex][currentCount + triangles_num] = isosurface_vertices_pos[a2iTriangleConnectionTable[cube8_flag_index][triangles_num]];
		threadVertices[threadIndex][currentCount + triangles_num + 1] = isosurface_vertices_pos[a2iTriangleConnectionTable[cube8_flag_index][triangles_num + 1]];
		threadVertices[threadIndex][currentCount + triangles_num + 2] = isosurface_vertices_pos[a2iTriangleConnectionTable[cube8_flag_index][triangles_num + 2]];
		threadNormals[threadIndex][currentCount + triangles_num] = isosurface_vertices_normal[a2iTriangleConnectionTable[cube8_flag_index][triangles_num]];
		threadNormals[threadIndex][currentCount + triangles_num + 1] = isosurface_vertices_normal[a2iTriangleConnectionTable[cube8_flag_index][triangles_num + 1]];
		threadNormals[threadIndex][currentCount + triangles_num + 2] = isosurface_vertices_normal[a2iTriangleConnectionTable[cube8_flag_index][triangles_num + 2]];
	}
	threadVerticesCount[threadIndex] = currentCount + triangles_num;
}