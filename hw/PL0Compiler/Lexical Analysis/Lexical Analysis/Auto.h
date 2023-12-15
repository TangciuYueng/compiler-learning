#pragma once
#ifndef _AUTO_H_
#define _AUTO_H_
#include<set>
#include<string>
#include"GraphList.h"
#include"GraphList.cpp"  //ģ����Ҫ��cppҲ��������
using namespace std;
using namespace toolkit;

namespace automat {
	class Auto {
	protected:
		set<int> startList;  //��ʼ̬�б�
		set<int> endList;    //��ֹ̬�б�
		GraphList<int, string> transList;  //״̬ת��ͼ

		Auto() : transList(GraphList<int, string>(true, 512)) {}
	};
}

#endif