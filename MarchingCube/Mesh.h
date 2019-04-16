#pragma once
#include "Vec3.h"
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

struct Mesh
{
	Vec3* Vertices;
	Vec3* Normals;
	unsigned long VerticesCount = 0;
	void SaveObjToFile(const string& path)const
	{
		// TODO ������Ժ�����
		bool vt = true; //����һ�������vt����ȴ����������Ϊ��assimpû����������ͻ������⡣����
		string space = " ";
		ofstream writeSteam = ofstream(path);
		if (!writeSteam)
		{
			cout << "���ļ�ʧ��";
			return;
		}
		writeSteam << "�������" << VerticesCount << endl;
		writeSteam << "��ĸ���" << VerticesCount / 3 << endl;
		writeSteam << "# ����v" << endl;
		for (unsigned long i = 0; i < VerticesCount; i++)
		{
			writeSteam << "v" << space << Vertices[i].x << space << Vertices[i].y << space << Vertices[i].z << endl;
		}
		writeSteam << endl << endl << endl << "# ����vt" << endl;
		if (vt)
		{
			writeSteam << "vt" << space << 0 << space << 0 << endl;
		}
		writeSteam << endl << endl << endl << "# ����vn" << endl;
		for (unsigned long i = 0; i < VerticesCount; i++)
		{
			writeSteam << "vn" << space << Normals[i].x << space << Normals[i].y << space << Normals[i].z << endl;
		}
		writeSteam << endl << endl << endl << "# ��f" << endl;
		for (unsigned long i = 0; i < VerticesCount / 3; i++)
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
	}
};