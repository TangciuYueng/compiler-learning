#include"NFA.h"

namespace automat {
	/*初始化全部NFA状态总数*/
	int NFA::stateCount = 0;

	/*初始化总NFA转换信息表*/
	vector<NFATrans> NFA::totalTransList = vector<NFATrans>();

	/***************************************************************************
	  函数名称：OperateCreateNew
	  功    能：Thompson算法：新字符创建
	  输入参数：const string newContent：新字符的内容
	  返 回 值：NFAPartInfo：运算后该NFA新的始终态信息
	  说    明：新字符可能有像<letter>、<digit>这种描述性的内容，因此设为string类型
	***************************************************************************/
	NFAPartInfo NFA::OperateCreateNew(const string newContent) {
		//根据状态总数取新的始态和终态，状态总数+2
		const int newStart = stateCount;
		const int newEnd = stateCount + 1;
		stateCount += 2;

		//更新当前NFA的始态、终态、始态列表和终态列表
		this->start = newStart;
		this->end = newEnd;
		this->startList.insert(newStart);
		this->endList.insert(newEnd);

		//在NFA对应图中插入结点
		this->transList.insertVertex(newStart);
		this->transList.insertVertex(newEnd);

		//得到新始终态顶点在图中结点列表的下标
		const int newStartPos = this->transList.getVertexPos(newStart);
		const int newEndPos = this->transList.getVertexPos(newEnd);

		//根据新字符内容给图添加边，并记录到总转换信息表中
		if (newContent == "l") {
			this->transList.insertEdge(newStartPos, newEndPos, "<letter>");
			totalTransList.push_back(NFATrans(newStart, newEnd, "<letter>"));
		}
		else if (newContent == "d") {
			this->transList.insertEdge(newStartPos, newEndPos, "<digit>");
			totalTransList.push_back(NFATrans(newStart, newEnd, "<digit>"));
		}
		else {
			this->transList.insertEdge(newStartPos, newEndPos, newContent);
			totalTransList.push_back(NFATrans(newStart, newEnd, newContent));
		}

		//把新的始终态信息返回
		return NFAPartInfo(newStart, newEnd);
	}

	/***************************************************************************
	  函数名称：OperateOr
	  功    能：Thompson算法：或运算
	  输入参数：const NFAPartInfo part1：第一个NFA部分
	            const NFAPartInfo part2：第二个NFA部分
	  返 回 值：NFAPartInfo：运算后该NFA新的始终态信息
	  说    明：或运算需要把两个NFA部分连接起来
	***************************************************************************/
	NFAPartInfo NFA::OperateOr(const NFAPartInfo part1, const NFAPartInfo part2) {
		//根据状态总数取新的始态和终态，状态总数+2
		const int newStart = stateCount;
		const int newEnd = stateCount + 1;
		stateCount += 2;

		//更新当前NFA的始态、终态
		this->start = newStart;
		this->end = newEnd;

		//在NFA对应图中插入结点
		this->transList.insertVertex(newStart);
		this->transList.insertVertex(newEnd);

		//得到新始终态顶点在图中结点列表的下标
		const int newStartPos = this->transList.getVertexPos(newStart);
		const int newEndPos = this->transList.getVertexPos(newEnd);

		//得到两个部分各自的始终态在图中结点列表的下标
		const int p1sPos = this->transList.getVertexPos(part1.start);
		const int p1ePos = this->transList.getVertexPos(part1.end);
		const int p2sPos = this->transList.getVertexPos(part2.start);
		const int p2ePos = this->transList.getVertexPos(part2.end);

		//给图添加边，并记录到总转换信息表中
		this->transList.insertEdge(newStartPos, p1sPos, "<epsilon>");
		this->transList.insertEdge(newStartPos, p2sPos, "<epsilon>");
		this->transList.insertEdge(p1ePos, newEndPos, "<epsilon>");
		this->transList.insertEdge(p2ePos, newEndPos, "<epsilon>");
		totalTransList.push_back(NFATrans(newStart, part1.start, "<epsilon>"));
		totalTransList.push_back(NFATrans(newStart, part2.start, "<epsilon>"));
		totalTransList.push_back(NFATrans(part1.end, newEnd, "<epsilon>"));
		totalTransList.push_back(NFATrans(part2.end, newEnd, "<epsilon>"));

		//运算后只有新始态一个始态，故清除始态列表，只留下新始态
		this->startList.clear();
		this->startList.insert(newStart);

		//运算后只有新始态一个始态，故清除始态列表，只留下新始态
		this->endList.clear();
		this->endList.insert(newEnd);

		//把新的始终态信息返回
		return NFAPartInfo(newStart, newEnd);
	}

	/***************************************************************************
	  函数名称：OperateAnd
	  功    能：Thompson算法：与运算
	  输入参数：const NFAPartInfo part1：第一个NFA部分
				const NFAPartInfo part2：第二个NFA部分
	  返 回 值：NFAPartInfo：运算后该NFA新的始终态信息
	  说    明：与运算需要把两个NFA部分连接起来
	***************************************************************************/
	NFAPartInfo NFA::OperateAnd(const NFAPartInfo part1, const NFAPartInfo part2) {
		//获取第一部分的终态和第二部分的始态在图中结点列表的下标
		const int p1ePos = this->transList.getVertexPos(part1.end);
		const int p2sPos = this->transList.getVertexPos(part2.start);

		//给图添加边，并记录到总转换信息表中
		this->transList.insertEdge(p1ePos, p2sPos, "<epsilon>");
		totalTransList.push_back(NFATrans(part1.end, part2.start, "<epsilon>"));

		//更新当前NFA的始态、终态
		this->start = part1.start;
		this->end = part2.end;

		//连接后第一部分的终态不再是终态，而第二部分的始态也不再是始态了，从对应的列表中删去
		this->startList.erase(part2.start);
		this->endList.erase(part1.end);

		//把新的始终态信息返回
		return NFAPartInfo(part1.start, part2.end);
	}

	/***************************************************************************
	  函数名称：OperateClosure
	  功    能：Thompson算法：闭包运算
	  输入参数：const NFAPartInfo part：NFA部分
	  返 回 值：NFAPartInfo：运算后该NFA新的始终态信息
	  说    明：闭包运算只需要对一个NFA部分进行闭包运算
	***************************************************************************/
	NFAPartInfo NFA::OperateClosure(const NFAPartInfo part) {
		//根据状态总数取新的始态和终态，状态总数+2
		const int newStart = stateCount;
		const int newEnd = stateCount + 1;
		stateCount += 2;

		//更新当前NFA的始态、终态
		this->start = newStart;
		this->end = newEnd;

		//在NFA对应图中插入结点
		this->transList.insertVertex(newStart);
		this->transList.insertVertex(newEnd);

		//得到新始终态顶点在图中结点列表的下标
		const int newStartPos = this->transList.getVertexPos(newStart);
		const int newEndPos = this->transList.getVertexPos(newEnd);

		//得到该部分的始终态在图中结点列表的下标
		const int psPos = this->transList.getVertexPos(part.start);
		const int pePos = this->transList.getVertexPos(part.end);

		//给图添加边，并记录到总转换信息表中
		this->transList.insertEdge(newStartPos, psPos, "<epsilon>");
		this->transList.insertEdge(pePos, psPos, "<epsilon>");
		this->transList.insertEdge(pePos, newEndPos, "<epsilon>");
		this->transList.insertEdge(newStartPos, newEndPos, "<epsilon>");
		totalTransList.push_back(NFATrans(newStart, part.start, "<epsilon>"));
		totalTransList.push_back(NFATrans(part.end, part.start, "<epsilon>"));
		totalTransList.push_back(NFATrans(part.end, newEnd, "<epsilon>"));
		totalTransList.push_back(NFATrans(newStart, newEnd, "<epsilon>"));

		//运算后只有新始态一个始态，故清除始态列表，只留下新始态
		this->startList.clear();
		this->startList.insert(newStart);

		//运算后只有新始态一个始态，故清除始态列表，只留下新始态
		this->endList.clear();
		this->endList.insert(newEnd);

		//把新的始终态信息返回
		return NFAPartInfo(newStart, newEnd);
	}

	/***************************************************************************
	  函数名称：GetInfo
	  功    能：返回该NFA的始终态信息
	  输入参数：无
	  返 回 值：NFAPartInfo：该NFA的始终态信息
	  说    明：无
	***************************************************************************/
	NFAPartInfo NFA::GetInfo()const {
		return NFAPartInfo(this->start, this->end);
	}

	/***************************************************************************
	  函数名称：GetTransList
	  功    能：返回该NFA对应的图
	  输入参数：无
	  返 回 值：const GraphList<int, string>*：指向该NFA对应图的指针
	  说    明：无
	***************************************************************************/
	const GraphList<int, string>* NFA::GetTransList()const {
		return &this->transList;
	}

	/***************************************************************************
	  函数名称：GetEndList
	  功    能：返回该NFA的终态列表
	  输入参数：无
	  返 回 值：const set<int>*：指向该NFA终态列表的指针
	  说    明：无
	***************************************************************************/
	const set<int>* NFA::GetEndList()const {
		return &this->endList;
	}

	/***************************************************************************
	  函数名称：GetEndInfoMap
	  功    能：返回该NFA终态信息表
	  输入参数：无
	  返 回 值：const map<int, NFAEndInfo>*：指向该NFA终态信息表的指针
	  说    明：无
	***************************************************************************/
	const map<int, NFAEndInfo>* NFA::GetEndInfoMap()const {
		return &this->endInfoMap;
	}

	/***************************************************************************
	  函数名称：MakeTotalNFA
	  功    能：构建最终NFA
	  输入参数：const int newStart：新的始态
	            set<int>& newEndList：新的终态列表
				map<int, NFAEndInfo>&：新的终态信息表
	  返 回 值：无
	  说    明：仿照LEX构建的总NFA，仅一个始态
	***************************************************************************/
	void NFA::MakeTotalNFA(const int newStart, set<int>& newEndList, map<int, NFAEndInfo>& newEndInfoMap) {
		//更新当前NFA的始态、始态列表、终态列表（有多个终态了，无法更新终态）
		this->start = newStart;
		this->startList.clear();
		this->startList.insert(newStart);
		this->endList = newEndList;

		//根据总状态转移表构建NFA对应的图
		for (NFATrans rule : totalTransList) {
			if (this->transList.getVertexPos(rule.start) == -1)  //若起始态不存在，添加结点
				this->transList.insertVertex(rule.start);
			if (this->transList.getVertexPos(rule.end) == -1)    //若终态不存在，添加结点
				this->transList.insertVertex(rule.end);

			//添加边
			this->transList.insertEdge(this->transList.getVertexPos(rule.start), this->transList.getVertexPos(rule.end), rule.recv);
		}

		//更新终态信息表
		this->endInfoMap.clear();
		for (auto info : newEndInfoMap) {
			this->endInfoMap.emplace(info);
		}
	}

	/***************************************************************************
	  函数名称：Show
	  功    能：显示NFA信息
	  输入参数：无
	  返 回 值：无
	  说    明：调试用
	***************************************************************************/
	void NFA::Show()const {
		cout << "-----------------------NFA-----------------------\n"
			<< "start: " << this->start << endl
			<< "end: " << this->end << endl
			<< "NFA::stateCount: " << stateCount << endl;

		cout << "startList: ";
		set<int>::iterator setIter = this->startList.begin();
		while (setIter != this->startList.end()) {
			cout << *setIter << ' ';
			setIter++;
		}
		cout << endl;

		cout << "endList: ";
		setIter = this->endList.begin();
		while (setIter != this->endList.end()) {
			cout << *setIter << ' ';
			setIter++;
		}
		cout << endl;

		cout << "transList:" << endl;
		cout << this->transList;

		cout << "endInfoMap:" << endl;
		auto iter = this->endInfoMap.begin();
		while (iter != this->endInfoMap.end()) {
			cout << "状态：" << (*iter).first << endl
				<< "正规式：" << (*iter).second.regular << endl
				<< "优先级：" << (*iter).second.priority << endl;

			iter++;
		}
	}
}