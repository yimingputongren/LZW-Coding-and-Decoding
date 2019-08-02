// LZWCoding.cpp : ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ��������
// ��������Դ����������У�ֻ�����ע�ͺ������޸ġ�

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <map>
#include <string>
#include <algorithm>

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::map;
using std::pair;
using std::string;
using std::copy;

const unsigned short MAX_CODES = 4096, //�����ÿ��key���룬��Ӧ��value��������ѹ����Ĵ��룬���ÿ�����붼��12λ��ʾ������4096��Ҳ����������4096
BYTE_SIZE = 8, //��12λ���ֽ������ÿ�����һ���ֽ�
EXCESS = 4, //��ô�ض���4λ�����´����������EXCESS=4����Դ
ALPHA = 256, //�Ƚ�0-255��ASCII������ҵı����
MASK = 15;	//��EXCESS��Ӧ

pair<int, char>ht[MAX_CODES];
char s[MAX_CODES];
int size;
int leftOver;
bool bitsLeftOver = false;
ifstream in;
ofstream out;


//�����������������
void setFiles(int argc, char* argv[])
{
	int tmp;
	string outputFile, inputFile;
	if (argc >= 2)
	{
		inputFile = argv[1];
	}
	else
	{
		cout << "Enter name of file to decompress" << endl;
		cin >> inputFile;
	}

	in.open(inputFile, ios::binary);
	if (in.fail())
	{
		cerr << "Cannot open " << inputFile << endl;
		exit(1);
	}
	tmp = inputFile.find_last_of(".");
	for (int i = 0;i <= tmp;i++)
	{
		outputFile += inputFile.at(i);
	}

	out.open(outputFile, ios::binary);
}


//�ȿ�compress���ٿ�output

void output(int code)
{
	size = -1;
	while (code >= ALPHA)
	{
		code = ht[code].first;
	}
	s[++size] = code;
	for (int i = size;i >= 0;i--)
	{
		out.put(s[i]);
	}
}

//�����������Ӧ��һ������ȡ12λ

bool getCode(int& code)
{
	int c, d;
	if ((c = in.get()) == EOF)
	{
		return false;
	}
	if (bitsLeftOver)
	{
		code = (leftOver << BYTE_SIZE) | c;
	}
	else
	{
		d = in.get();
		code = (c << EXCESS) | (d >> EXCESS);
		leftOver = d & MASK;
	}
	bitsLeftOver = !bitsLeftOver;
	return true;
}

//ѹ������
void decompress()
{
	int codesUsed = ALPHA;
	int pcode, ccode;

	if (getCode(pcode))
	{
		s[0] = pcode;
		out.put(s[0]);
		size = 0;
		while (getCode(ccode))
		{
			if (ccode < codesUsed)    //˵��ccode�Ѿ����ҵ������
			{
				output(ccode);
				if (codesUsed < MAX_CODES)
				{
					ht[codesUsed].first = pcode; //��Ҫ���Ϊʲô�������ģ�����˼�������
					ht[codesUsed++].second = s[size]; //�������ʱ��Ĺؼ��֣�20λ���ֳ������֣���һ���ֵ�12λ����ǰ���ı������һ����Ĵ��룩��Ϊkey��
					//�ڶ����ֵ�8λ��Ϊvalue
				}
			}
			else
			{
				ht[codesUsed].first = pcode;
				ht[codesUsed++].second = s[size];
				output(ccode);
			}
			pcode = ccode;
		}

	}

	out.close();
	in.close();
}


int main(int argc, char* argv[])
{
	setFiles(argc, argv); //argc�������в���������argv��ָ�����飬������ǵ�һ��������ת��������������
	decompress();
	return 0;
}


