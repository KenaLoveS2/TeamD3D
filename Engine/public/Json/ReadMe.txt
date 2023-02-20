
# Json ����
Json�� VisualStudio Code�� ���� ���� ���մϴ�.
VS Code�� Json ������ ���� Ctrl+K+F�� ������ ���� ���ϰ� ���ĵ˴ϴ�.

# Json ����
����, �ҷ����Ⱑ �ʿ��� cpp��
	#include "Json/json.hpp"
	#include <fstream>
�� �����մϴ�.

## Json ����
1. �켱 Json �ֻ��� ��ü�� �ϳ� ����� �ݴϴ�.
	Json	jTest;

2. Json ��ü �ȿ� ������ �����Ӱ�, �ڷ����� ������� �߰� �մϴ�.
	�迭�� ������ �����ϸ�, ��İ��� ���� for������ ���� �ϳ��ϳ� ���鼭 �����߽��ϴ�.
	float4�ε� ������ �ٷ� ���������� �𸣰ھ��~

	int a = 27;
	string str = "�ֿ���";
	float4x4	matWorld;

	jTest["Age"] = a;
	jTest["Name"] = str;

	float	fElement = 0.f;
	for (int i = 0; i < 16; ++i)
	{
		fElement = 0.f;
		memcpy(&fElement, ((float*)&matWorld + i, sizeof(float));
		jTest["Transform State"].push_back(fElement);		// �迭 ����. �����̳��� ������ ����մϴ�. �̷��� �ϸ� Transform State���� 16���� float ���� ����˴ϴ�.
	}

3. Json ��ü�� �ٸ� Json ��ü�� ������ �� �ֽ��ϴ�.

	Json	jChild;
	
	jChild["Math"] = "A+";
	jChild["English"] = "A+";

	jTest["Grade"].push_back(jChild);

4. ���� ����

	ofstream	file("../Bin/Data/Student Info/Chil.json");
	file << jTest;
	file.close();

## Json �ҷ�����
1. �ҷ��� Json ��ü�� ����� �ݴϴ�.
	Json	jLoadTest;
	Json	jLoadChild;

2. ���� ����
	ifstream	file("../Bin/Data/Student Info/Chil.json");
	file >> jLoadTest;
	file.close();

3. ���� �ҷ��� ������ ����� �ҷ��ɴϴ�.
	int		age;
	string	name;
	float4x4	matWorld;
	string	math;
	string english;

	jLoadTest["Age"].get_to<int>(age);
	jLoadTest["Name"].get_to<string>(name);
	
	int k = 0;
	for (float fElement : jLoadTest["Transform State"])	// Json ��ü�� ������� for�� ����� �����մϴ�.
		memcpy(((float*)&matWorld) + (i++), &fElement, sizeof(float));

	for (auto jGrade : jLoadTest["Grade"])
	{
		jGrade["Math"].get_to<string>(math);
		jGrade["English"].get_to<string>(english);
	}

�� �� ������ Json �⺻ �����̰�, �����ϸ鼭 ���� �ʾҽ��ϴ�. �Ƹ� ��ȯ�̰� �̸����� �� ������ ������ �� �������~