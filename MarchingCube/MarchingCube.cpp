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
		// 循环遍历每个立方体，立方体数由细分数决定
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
	, const uint16& x_index, const uint16& y_index, const uint16& z_index
	, const float& step_size, const float& scale)const
{
	const Vec3 subdivie_point = Vec3(x_index, y_index, z_index)* step_size;	//样本点
	// 交点坐标，对于算法中的e1-e12。如果不插值的直接取中点的话，是可以预先计算好的
	Vec3 isosurface_vertices_pos[12];
	Vec3 isosurface_vertices_normal[12];

	float cube_vertex_value[8]; // cube每个顶点的值
	int cube8_flag_index = 0; // 标记cube的顶点状态的flag
	int edge_flag; // 获取cube的交点flag
	int triangles_num = 0; // cube对应的三角形的数量


	// 创建样本并查找cube顶点在表面内部或外部
	for (uint8 i = 0; i < 8; ++i)
	{
		const uint8 x_offset = a2fVertexOffset[i][0];
		const uint8 y_offset = a2fVertexOffset[i][1];
		const uint8 z_offset = a2fVertexOffset[i][2]; //和上面注释掉的代码功能一致
		cube_vertex_value[i] = GetDataUseXYZ(data, x_index + x_offset, y_index + y_offset, z_index + z_offset);
		if (cube_vertex_value[i] <= target_value)
			cube8_flag_index |= 1 << i; //位运算
	}

	edge_flag = aiCubeEdgeFlags[cube8_flag_index];
	if (edge_flag == 0) //如果没有三角形需要画
	{
		return;
	}

	for (uint8 i = 0; i < 12; ++i) 
	{
		if (edge_flag & (1 << i)) 
		{
			// cube一条边的两个顶点的值之差
			const float delta = cube_vertex_value[cube_edges_indices[i][1]] - cube_vertex_value[cube_edges_indices[i][0]];
			float a_offset = 0.5f; //等值面与cube的边的交点，取cube边的中点
			if (delta != 0.0)
			{
				a_offset = (target_value - cube_vertex_value[cube_edges_indices[i][0]]) / delta; //交点根据标量值进行插值
			}
			//插值出每条边上的交点
			isosurface_vertices_pos[i] = subdivie_point + (cube_vertex_position[cube_edges_indices[i][0]]
				+ cube_edge_direction[i] * a_offset) * scale;
			isosurface_vertices_normal[i] = CalVerticesNormal(x_index, y_index, z_index);
		}
	}

	for (uint8 i = 0; i < 5; ++i)
	{
		if (a2iTriangleConnectionTable[cube8_flag_index][3 * i] < 0) // <0 等价于 ==-1
		{
			break;
		}
		triangles_num++;
	}

	//cube对应三角形的边的索引数量
	const uint8 triangles_edge_indices_num = triangles_num * 3;
	for (uint8 i = 0; i < triangles_edge_indices_num; ++i)
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
		const uint8 offset = 1;
		// 中心差分
		normal.x = GetDataUseXYZ(data, x_index - offset, y_index, z_index) - GetDataUseXYZ(data, x_index + offset, y_index, z_index);
		normal.y = GetDataUseXYZ(data, x_index, y_index - offset, z_index) - GetDataUseXYZ(data, x_index, y_index + offset, z_index);
		normal.z = GetDataUseXYZ(data, x_index, y_index, z_index - offset) - GetDataUseXYZ(data, x_index, y_index, z_index + offset);
		normal.normalize();
	}
	return normal;
}

inline float MarchingCube::GetDataUseXYZ(const float * data, const uint16& x
	, const uint16& y, const uint16& z)const
{
	return data[x * cube_size_yz + y * cube_size_z + z];;
}