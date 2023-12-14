#include"NFA.h"

namespace automat {
	int NFA::stateCount = 0;
	vector<NFATrans> NFA::totalTransList = vector<NFATrans>();

	void NFA::ShowTotalTransList() {
		for (NFATrans trans : totalTransList) {
			cout << trans.start << "--" << trans.recv << "-->" << trans.end << endl;
		}
	}

	NFAPartInfo NFA::OperateCreateNew(const string newContent) {
		const int newStart = stateCount;
		const int newEnd = stateCount + 1;
		this->start = newStart;
		this->end = newEnd;
		stateCount += 2;
		this->startList.insert(newStart);
		this->endList.insert(newEnd);
		this->transList.insertVertex(newStart);
		this->transList.insertVertex(newEnd);

		const int newStartVertex = this->transList.getVertexPos(newStart);
		const int newEndVertex = this->transList.getVertexPos(newEnd);

		if (newContent == "l") {
			this->transList.insertEdge(newStartVertex, newEndVertex, "<letter>");
			totalTransList.push_back(NFATrans(newStart, newEnd, "<letter>"));
		}
		else if (newContent == "d") {
			this->transList.insertEdge(newStartVertex, newEndVertex, "<digit>");
			totalTransList.push_back(NFATrans(newStart, newEnd, "<digit>"));
		}
		else {
			this->transList.insertEdge(newStartVertex, newEndVertex, newContent);
			totalTransList.push_back(NFATrans(newStart, newEnd, newContent));
		}

		return NFAPartInfo(newStart, newEnd);
	}

	NFAPartInfo NFA::OperateOr(const NFAPartInfo part1, const NFAPartInfo part2) {
		const int newStart = stateCount;
		const int newEnd = stateCount + 1;
		stateCount += 2;
		this->start = newStart;
		this->end = newEnd;
		this->transList.insertVertex(newStart);
		this->transList.insertVertex(newEnd);

		const int newStartVertex = this->transList.getVertexPos(newStart);
		const int newEndVertex = this->transList.getVertexPos(newEnd);
		const int p1sVertex = this->transList.getVertexPos(part1.start);
		const int p1eVertex = this->transList.getVertexPos(part1.end);
		const int p2sVertex = this->transList.getVertexPos(part2.start);
		const int p2eVertex = this->transList.getVertexPos(part2.end);

		this->transList.insertEdge(newStartVertex, p1sVertex, "<epsilon>");
		this->transList.insertEdge(newStartVertex, p2sVertex, "<epsilon>");
		this->transList.insertEdge(p1eVertex, newEndVertex, "<epsilon>");
		this->transList.insertEdge(p2eVertex, newEndVertex, "<epsilon>");
		totalTransList.push_back(NFATrans(newStart, part1.start, "<epsilon>"));
		totalTransList.push_back(NFATrans(newStart, part2.start, "<epsilon>"));
		totalTransList.push_back(NFATrans(part1.end, newEnd, "<epsilon>"));
		totalTransList.push_back(NFATrans(part2.end, newEnd, "<epsilon>"));

		this->startList.erase(part1.start);
		this->startList.erase(part2.start);
		this->startList.insert(newStart);
		this->endList.erase(part1.end);
		this->endList.erase(part2.end);
		this->endList.insert(newEnd);
		return NFAPartInfo(newStart, newEnd);
	}

	NFAPartInfo NFA::OperateAnd(const NFAPartInfo part1, const NFAPartInfo part2) {
		const int p1eVertex = this->transList.getVertexPos(part1.end);
		const int p2sVertex = this->transList.getVertexPos(part2.start);

		this->transList.insertEdge(p1eVertex, p2sVertex, "<epsilon>");
		totalTransList.push_back(NFATrans(part1.end, part2.start, "<epsilon>"));
		this->start = part1.start;
		this->end = part2.end;
		this->startList.erase(part2.start);
		this->endList.erase(part1.end);
		return NFAPartInfo(part1.start, part2.end);
	}

	NFAPartInfo NFA::OperateClosure(const NFAPartInfo part) {
		const int newStart = stateCount;
		const int newEnd = stateCount + 1;
		stateCount += 2;
		this->start = newStart;
		this->end = newEnd;
		this->transList.insertVertex(newStart);
		this->transList.insertVertex(newEnd);

		const int newStartVertex = this->transList.getVertexPos(newStart);
		const int newEndVertex = this->transList.getVertexPos(newEnd);
		const int psVertex = this->transList.getVertexPos(part.start);
		const int peVertex = this->transList.getVertexPos(part.end);

		this->transList.insertEdge(newStartVertex, psVertex, "<epsilon>");
		this->transList.insertEdge(peVertex, psVertex, "<epsilon>");
		this->transList.insertEdge(peVertex, newEndVertex, "<epsilon>");
		this->transList.insertEdge(newStartVertex, newEndVertex, "<epsilon>");
		totalTransList.push_back(NFATrans(newStart, part.start, "<epsilon>"));
		totalTransList.push_back(NFATrans(part.end, part.start, "<epsilon>"));
		totalTransList.push_back(NFATrans(part.end, newEnd, "<epsilon>"));
		totalTransList.push_back(NFATrans(newStart, newEnd, "<epsilon>"));

		this->startList.clear();
		this->endList.clear();
		this->startList.insert(newStart);
		this->endList.insert(newEnd);
		return NFAPartInfo(newStart, newEnd);
	}

	NFAPartInfo NFA::GetInfo()const {
		return NFAPartInfo(this->start, this->end);
	}

	const GraphList<int, string>* NFA::GetTransList()const {
		return &this->transList;
	}

	const set<int>* NFA::GetEndList()const {
		return &this->endList;
	}

	const map<int, NFAEndInfo>* NFA::GetEndInfoMap()const {
		return &this->endInfoMap;
	}

	void NFA::SetEndInfoMap(map<int, NFAEndInfo>& endInfoMap) {
		this->endInfoMap.clear();

		for (auto info : endInfoMap) {
			this->endInfoMap.emplace(info);
		}
	}

	void NFA::MakeTotalNFA(const int newStart, set<int>& newEndList) {
		this->start = newStart;
		this->startList.clear();
		this->startList.insert(newStart);
		this->endList = newEndList;

		for (NFATrans rule : totalTransList) {
			if (this->transList.getVertexPos(rule.start) == -1)
				this->transList.insertVertex(rule.start);
			if (this->transList.getVertexPos(rule.end) == -1)
				this->transList.insertVertex(rule.end);

			this->transList.insertEdge(this->transList.getVertexPos(rule.start), this->transList.getVertexPos(rule.end), rule.recv);
		}
	}

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