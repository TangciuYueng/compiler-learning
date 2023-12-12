#pragma once
#ifndef _AUTO_H_
#define _AUTO_H_
#include<set>
#include<string>
#include"D:/Tool Kit/GraphList/GraphList.h"
using namespace std;

namespace automat {
	class Auto {
	protected:
		set<int> startList;  //起始态列表
		set<int> endList;    //终止态列表
		GraphList<int, string> transList;  //状态转换图

		Auto() : transList(GraphList<int, string>(true, 512)) {}
	};
}

#endif