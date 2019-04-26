#include <windows.h>
#include "MarchingCube.h"

const float* readFromFilePath(const string& path, const long& size)
{
	float *data = new float[size];
	ifstream readStream = ifstream(path, ifstream::binary);
	if (!readStream)
	{
		cout << "打开" << path << "文件失败" << endl;
	}
	readStream.read((char*)data, size * sizeof(float));
	return data;
}

int main()
{
	LARGE_INTEGER start; LARGE_INTEGER end; LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	const long size = 160 * 160 * 200;

	QueryPerformanceCounter(&start);
		const float * data = readFromFilePath("./pet_raw.dat", size);
	QueryPerformanceCounter(&end);
	printf("Load data: %d ms\n", (int)((end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart));

	QueryPerformanceCounter(&start);
		MarchingCube mc(data, 200, 160, 160);
	QueryPerformanceCounter(&end);
	printf("Init MarchingCube: %d ms\n", (int)((end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart));

	QueryPerformanceCounter(&start);
		mc.MarchingCubeCore(600000.0f);
	QueryPerformanceCounter(&end);
	printf("MarchingCube: %d ms\n", (int)((end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart));

	QueryPerformanceCounter(&start);
		Mesh m = mc.ToMesh();
	QueryPerformanceCounter(&end);
	printf("To Mesh: %d ms\n", (int)((end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart));
	
	QueryPerformanceCounter(&start);
		m.SaveObjToFile("./output.obj");
	QueryPerformanceCounter(&end);
	printf("Mesh Save Model: %d ms\n", (int)((end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart));
	mc.DestoryMarchingCube();
	delete data;
	getchar();
	return 0;
}