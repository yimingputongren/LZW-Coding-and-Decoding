// LZWCoding.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// 本代码来源于萨尼的书中，只是添加注释和略作修改。

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

const unsigned short MAX_CODES = 4096, //假设给每个key编码，对应的value就是我们压缩后的代码，如果每个代码都用12位表示，就是4096，也就是最多编码4096
BYTE_SIZE = 8, //将12位按字节输出，每次输出一个字节
EXCESS = 4, //那么必定有4位留待下次输出，就是EXCESS=4的来源
ALPHA = 256, //先将0-255的ASCII码放入我的编码表
MASK = 15;	//与EXCESS对应

pair<int, char>ht[MAX_CODES];
char s[MAX_CODES];
int size;
int leftOver;
bool bitsLeftOver = false;
ifstream in;
ofstream out;


//设置输入流和输出流
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


//先看compress，再看output

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

//与编码过程相对应，一个代码取12位

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

//压缩过程
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
			if (ccode < codesUsed)    //说明ccode已经在我的码表里
			{
				output(ccode);
				if (codesUsed < MAX_CODES)
				{
					ht[codesUsed].first = pcode; //不要奇怪为什么是这样的，它的思想很巧妙
					ht[codesUsed++].second = s[size]; //将编码的时候的关键字（20位）分成两部分，第一部分的12位（当前的文本除最后一个外的代码）作为key，
					//第二部分的8位作为value
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
	setFiles(argc, argv); //argc是命令行参数个数，argv是指针数组，命令本身是第一个参数，转到这个函数看详解
	decompress();
	return 0;
}


