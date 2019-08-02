// LZWCoding.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// 本代码来源于萨尼的书中，只是添加注释和略作修改。

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <map>
#include <string>

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

const unsigned short MAX_CODES = 4096, //假设给每个key编码，对应的value就是我们压缩后的代码，如果每个代码都用12位表示，就是4096，也就是最多编码4096
BYTE_SIZE = 8, //将12位按字节输出，每次输出一个字节
EXCESS = 4, //那么必定有4位留待下次输出，就是EXCESS=4的来源
ALPHA = 256, //先将0-255的ASCII码放入我的编码表
MASK1 = 255, //与BYTE_SIZE对应
MASK2 = 15;	//与EXCESS对应

int leftOver;
bool bitsLeftOver = false;
ifstream in;
ofstream out;


//设置输入流和输出流
void setFiles(int argc, char* argv[])
{
	string outputFile, inputFile;
	if (argc >= 2)
	{
		inputFile = argv[1];
	}
	else
	{
		cout << "Enter name of file to compress" << endl;
		cin >> inputFile;
	}

	in.open(inputFile, ios::binary);
	if (in.fail())
	{
		cerr << "Cannot open " << inputFile << endl;
		exit(1);
	}
	outputFile = inputFile + ".txt";
	out.open(outputFile, ios::binary);
}

//先看compress，再看output


void output(long pcode)
{
	cout << pcode << endl;
	int c, d;
	if (bitsLeftOver)
	{
		d = static_cast<int>(pcode & MASK1); //前面字符有4位遗留，先取这次的低8位
		c = static_cast<int>((leftOver << EXCESS)|(pcode >> BYTE_SIZE)); //前一个字符4位遗留的和这次高4位一次组成一个字节输出
		out.put(c);
		out.put(d);
		bitsLeftOver = false;
	}
	else
	{
		leftOver = pcode & MASK2; //没有4位遗留，取低4位
		c = static_cast<int>(pcode >> EXCESS); //取高8位
		out.put(c); //输出高8位
		bitsLeftOver = true; //低4位还没有输出，做个标记
	}
}

//压缩过程
void compress()
{
	map<long, int> h; //key是我们从文本中获取的关键字，value是我们它的代码（要输出的码），书上的代码从0开始，是虽然可以，但难理解，不过还是先按书上走。
	//难理解原因，如我有<key,value>：<a,0>,<b,1>，假设字符串是aab，那么下一个要放入编码表的有<aa,2>，以后要有<aaa...aaa,N),那key要很多个字节怎么办，
	//为了避免这个问题，书上用<0a,2>来代替<aa,2>这样所有key的字节数是一样的，但是<0a,2>和<a,0>是一般逻辑挺难区分的，不过却是可行的
	for (int i = 0;i < ALPHA;i++)
	{
		h.insert(pair<long, int>(i, i));
	}
	int codesUsed = ALPHA;
	int c = in.get();
	if (c != EOF)
	{
		long pcode = c;
		while ((c = in.get()) != EOF)
		{
			long theKey = (pcode << BYTE_SIZE) + c;
			map<long, int>::iterator thePair = h.find(theKey);
			if (thePair == h.end())
			{
				output(pcode); //先输出代码，要说明的是这里输出的pcode不要以为是key，其实key和value是一样的值才可以这样
				if (codesUsed < MAX_CODES)
				{
					h.insert(pair<long, int>((pcode << BYTE_SIZE) | c, codesUsed++));
				}
				pcode = c;
			}
			else
			{
				pcode = thePair->second; //这里的是value，其实output的都是value
			}
		}
		output(pcode);
		//跳到output中去看，回来再看这里就懂了
		if (bitsLeftOver)
		{
			out.put(leftOver << EXCESS);
		}
	}
	cout << "pcodesused:" << codesUsed << endl;
	out.close();
	in.close();
}


int main(int argc,char *argv[])
{
	setFiles(argc, argv); //argc是命令行参数个数，argv是指针数组，命令本身是第一个参数，转到这个函数看详解
	compress();
	return 0;
}


