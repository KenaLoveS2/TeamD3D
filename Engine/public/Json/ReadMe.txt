
# Json 열람
Json은 VisualStudio Code로 보는 것이 편합니다.
VS Code로 Json 파일을 열고 Ctrl+K+F를 누르면 보기 편하게 정렬됩니다.

# Json 사용법
저장, 불러오기가 필요한 cpp에
	#include "Json/json.hpp"
	#include <fstream>
을 참조합니다.

## Json 저장
1. 우선 Json 최상위 객체를 하나 만들어 줍니다.
	Json	jTest;

2. Json 객체 안에 변수를 자유롭게, 자료형에 상관없이 추가 합니다.
	배열도 저장이 가능하며, 행렬같은 경우는 for문으로 원소 하나하나 돌면서 저장했습니다.
	float4로도 저장이 바로 가능한지는 모르겠어요~

	int a = 27;
	string str = "최원준";
	float4x4	matWorld;

	jTest["Age"] = a;
	jTest["Name"] = str;

	float	fElement = 0.f;
	for (int i = 0; i < 16; ++i)
	{
		fElement = 0.f;
		memcpy(&fElement, ((float*)&matWorld + i, sizeof(float));
		jTest["Transform State"].push_back(fElement);		// 배열 저장. 컨테이너의 구조랑 비슷합니다. 이렇게 하면 Transform State에는 16개의 float 값이 저장됩니다.
	}

3. Json 객체는 다른 Json 객체를 포함할 수 있습니다.

	Json	jChild;
	
	jChild["Math"] = "A+";
	jChild["English"] = "A+";

	jTest["Grade"].push_back(jChild);

4. 파일 쓰기

	ofstream	file("../Bin/Data/Student Info/Chil.json");
	file << jTest;
	file.close();

## Json 불러오기
1. 불러올 Json 객체를 만들어 줍니다.
	Json	jLoadTest;
	Json	jLoadChild;

2. 파일 열기
	ifstream	file("../Bin/Data/Student Info/Chil.json");
	file >> jLoadTest;
	file.close();

3. 값을 불러올 변수를 만들고 불러옵니다.
	int		age;
	string	name;
	float4x4	matWorld;
	string	math;
	string english;

	jLoadTest["Age"].get_to<int>(age);
	jLoadTest["Name"].get_to<string>(name);
	
	int k = 0;
	for (float fElement : jLoadTest["Transform State"])	// Json 객체는 범위기반 for문 사용이 가능합니다.
		memcpy(((float*)&matWorld) + (i++), &fElement, sizeof(float));

	for (auto jGrade : jLoadTest["Grade"])
	{
		jGrade["Math"].get_to<string>(math);
		jGrade["English"].get_to<string>(english);
	}

딱 이 정도가 Json 기본 사용법이고, 응용하면서 쓰진 않았습니다. 아마 중환이가 이리저리 잘 쓰던거 같은데 난 여기까지~