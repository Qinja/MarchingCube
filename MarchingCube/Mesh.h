#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "Vec3.h"
using namespace std;

struct Mesh
{
	Vec3* Vertices;
	Vec3* Normals;
	uint32 VerticesCount = 0;
	void SaveObjToFile(const string& path)const
	{
		// TODO 这个坑以后再填
		string space = " ";
		bool vt = true; //生成一个多余的vt纹理却不用它，因为用assimp没有这个分量就会有问题。。。
		ofstream writeSteam = ofstream(path);
		if (!writeSteam)
		{
			cout << "打开文件失败";
			return;
		}
		writeSteam << "顶点个数" << VerticesCount << endl;
		writeSteam << "面的个数" << VerticesCount / 3 << endl;
		writeSteam << "# 顶点v" << endl;
		for (uint32 i = 0; i < VerticesCount; i++)
		{
			writeSteam << "v" << space << Vertices[i].x << space << Vertices[i].y << space << Vertices[i].z << endl;
		}
		writeSteam << endl << endl << endl << "# 纹理vt" << endl;
		if (vt)
		{
			writeSteam << "vt" << space << 0 << space << 0 << endl;
		}
		writeSteam << endl << endl << endl << "# 法线vn" << endl;
		for (uint32 i = 0; i < VerticesCount; i++)
		{
			writeSteam << "vn" << space << Normals[i].x << space << Normals[i].y << space << Normals[i].z << endl;
		}
		writeSteam << endl << endl << endl << "# 面f" << endl;
		for (uint32 i = 0; i < VerticesCount / 3; i++)
		{
			if (vt)
			{
				writeSteam << "f" << space
					<< 3 * i + 1 << "/" << 1 << "/" << 3 * i + 1 << space
					<< 3 * i + 2 << "/" << 1 << "/" << 3 * i + 2 << space
					<< 3 * i + 3 << "/" << 1 << "/" << 3 * i + 3 << endl;
			}
			else
			{
				writeSteam << "f" << space
					<< 3 * i + 1 << "//" << 3 * i + 1 << space
					<< 3 * i + 2 << "//" << 3 * i + 2 << space
					<< 3 * i + 3 << "//" << 3 * i + 3 << endl;
			}
		}
		writeSteam.flush();
	}
};