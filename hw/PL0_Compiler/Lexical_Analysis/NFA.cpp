#include"NFA.h"

namespace automat {
	/*��ʼ��ȫ��NFA״̬����*/
	int NFA::stateCount = 0;

	/*��ʼ����NFAת����Ϣ��*/
	vector<NFATrans> NFA::totalTransList = vector<NFATrans>();

	/***************************************************************************
	  �������ƣ�OperateCreateNew
	  ��    �ܣ�Thompson�㷨�����ַ�����
	  ���������const string newContent�����ַ�������
	  �� �� ֵ��NFAPartInfo��������NFA�µ�ʼ��̬��Ϣ
	  ˵    �������ַ���������<letter>��<digit>���������Ե����ݣ������Ϊstring����
	***************************************************************************/
	NFAPartInfo NFA::OperateCreateNew(const string newContent) {
		//����״̬����ȡ�µ�ʼ̬����̬��״̬����+2
		const int newStart = stateCount;
		const int newEnd = stateCount + 1;
		stateCount += 2;

		//���µ�ǰNFA��ʼ̬����̬��ʼ̬�б����̬�б�
		this->start = newStart;
		this->end = newEnd;
		this->startList.insert(newStart);
		this->endList.insert(newEnd);

		//��NFA��Ӧͼ�в�����
		this->transList.insertVertex(newStart);
		this->transList.insertVertex(newEnd);

		//�õ���ʼ��̬������ͼ�н���б���±�
		const int newStartPos = this->transList.getVertexPos(newStart);
		const int newEndPos = this->transList.getVertexPos(newEnd);

		//�������ַ����ݸ�ͼ��ӱߣ�����¼����ת����Ϣ����
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

		//���µ�ʼ��̬��Ϣ����
		return NFAPartInfo(newStart, newEnd);
	}

	/***************************************************************************
	  �������ƣ�OperateOr
	  ��    �ܣ�Thompson�㷨��������
	  ���������const NFAPartInfo part1����һ��NFA����
	            const NFAPartInfo part2���ڶ���NFA����
	  �� �� ֵ��NFAPartInfo��������NFA�µ�ʼ��̬��Ϣ
	  ˵    ������������Ҫ������NFA������������
	***************************************************************************/
	NFAPartInfo NFA::OperateOr(const NFAPartInfo part1, const NFAPartInfo part2) {
		//����״̬����ȡ�µ�ʼ̬����̬��״̬����+2
		const int newStart = stateCount;
		const int newEnd = stateCount + 1;
		stateCount += 2;

		//���µ�ǰNFA��ʼ̬����̬
		this->start = newStart;
		this->end = newEnd;

		//��NFA��Ӧͼ�в�����
		this->transList.insertVertex(newStart);
		this->transList.insertVertex(newEnd);

		//�õ���ʼ��̬������ͼ�н���б���±�
		const int newStartPos = this->transList.getVertexPos(newStart);
		const int newEndPos = this->transList.getVertexPos(newEnd);

		//�õ��������ָ��Ե�ʼ��̬��ͼ�н���б���±�
		const int p1sPos = this->transList.getVertexPos(part1.start);
		const int p1ePos = this->transList.getVertexPos(part1.end);
		const int p2sPos = this->transList.getVertexPos(part2.start);
		const int p2ePos = this->transList.getVertexPos(part2.end);

		//��ͼ��ӱߣ�����¼����ת����Ϣ����
		this->transList.insertEdge(newStartPos, p1sPos, "<epsilon>");
		this->transList.insertEdge(newStartPos, p2sPos, "<epsilon>");
		this->transList.insertEdge(p1ePos, newEndPos, "<epsilon>");
		this->transList.insertEdge(p2ePos, newEndPos, "<epsilon>");
		totalTransList.push_back(NFATrans(newStart, part1.start, "<epsilon>"));
		totalTransList.push_back(NFATrans(newStart, part2.start, "<epsilon>"));
		totalTransList.push_back(NFATrans(part1.end, newEnd, "<epsilon>"));
		totalTransList.push_back(NFATrans(part2.end, newEnd, "<epsilon>"));

		//�����ֻ����ʼ̬һ��ʼ̬�������ʼ̬�б�ֻ������ʼ̬
		this->startList.clear();
		this->startList.insert(newStart);

		//�����ֻ����ʼ̬һ��ʼ̬�������ʼ̬�б�ֻ������ʼ̬
		this->endList.clear();
		this->endList.insert(newEnd);

		//���µ�ʼ��̬��Ϣ����
		return NFAPartInfo(newStart, newEnd);
	}

	/***************************************************************************
	  �������ƣ�OperateAnd
	  ��    �ܣ�Thompson�㷨��������
	  ���������const NFAPartInfo part1����һ��NFA����
				const NFAPartInfo part2���ڶ���NFA����
	  �� �� ֵ��NFAPartInfo��������NFA�µ�ʼ��̬��Ϣ
	  ˵    ������������Ҫ������NFA������������
	***************************************************************************/
	NFAPartInfo NFA::OperateAnd(const NFAPartInfo part1, const NFAPartInfo part2) {
		//��ȡ��һ���ֵ���̬�͵ڶ����ֵ�ʼ̬��ͼ�н���б���±�
		const int p1ePos = this->transList.getVertexPos(part1.end);
		const int p2sPos = this->transList.getVertexPos(part2.start);

		//��ͼ��ӱߣ�����¼����ת����Ϣ����
		this->transList.insertEdge(p1ePos, p2sPos, "<epsilon>");
		totalTransList.push_back(NFATrans(part1.end, part2.start, "<epsilon>"));

		//���µ�ǰNFA��ʼ̬����̬
		this->start = part1.start;
		this->end = part2.end;

		//���Ӻ��һ���ֵ���̬��������̬�����ڶ����ֵ�ʼ̬Ҳ������ʼ̬�ˣ��Ӷ�Ӧ���б���ɾȥ
		this->startList.erase(part2.start);
		this->endList.erase(part1.end);

		//���µ�ʼ��̬��Ϣ����
		return NFAPartInfo(part1.start, part2.end);
	}

	/***************************************************************************
	  �������ƣ�OperateClosure
	  ��    �ܣ�Thompson�㷨���հ�����
	  ���������const NFAPartInfo part��NFA����
	  �� �� ֵ��NFAPartInfo��������NFA�µ�ʼ��̬��Ϣ
	  ˵    �����հ�����ֻ��Ҫ��һ��NFA���ֽ��бհ�����
	***************************************************************************/
	NFAPartInfo NFA::OperateClosure(const NFAPartInfo part) {
		//����״̬����ȡ�µ�ʼ̬����̬��״̬����+2
		const int newStart = stateCount;
		const int newEnd = stateCount + 1;
		stateCount += 2;

		//���µ�ǰNFA��ʼ̬����̬
		this->start = newStart;
		this->end = newEnd;

		//��NFA��Ӧͼ�в�����
		this->transList.insertVertex(newStart);
		this->transList.insertVertex(newEnd);

		//�õ���ʼ��̬������ͼ�н���б���±�
		const int newStartPos = this->transList.getVertexPos(newStart);
		const int newEndPos = this->transList.getVertexPos(newEnd);

		//�õ��ò��ֵ�ʼ��̬��ͼ�н���б���±�
		const int psPos = this->transList.getVertexPos(part.start);
		const int pePos = this->transList.getVertexPos(part.end);

		//��ͼ��ӱߣ�����¼����ת����Ϣ����
		this->transList.insertEdge(newStartPos, psPos, "<epsilon>");
		this->transList.insertEdge(pePos, psPos, "<epsilon>");
		this->transList.insertEdge(pePos, newEndPos, "<epsilon>");
		this->transList.insertEdge(newStartPos, newEndPos, "<epsilon>");
		totalTransList.push_back(NFATrans(newStart, part.start, "<epsilon>"));
		totalTransList.push_back(NFATrans(part.end, part.start, "<epsilon>"));
		totalTransList.push_back(NFATrans(part.end, newEnd, "<epsilon>"));
		totalTransList.push_back(NFATrans(newStart, newEnd, "<epsilon>"));

		//�����ֻ����ʼ̬һ��ʼ̬�������ʼ̬�б�ֻ������ʼ̬
		this->startList.clear();
		this->startList.insert(newStart);

		//�����ֻ����ʼ̬һ��ʼ̬�������ʼ̬�б�ֻ������ʼ̬
		this->endList.clear();
		this->endList.insert(newEnd);

		//���µ�ʼ��̬��Ϣ����
		return NFAPartInfo(newStart, newEnd);
	}

	/***************************************************************************
	  �������ƣ�GetInfo
	  ��    �ܣ����ظ�NFA��ʼ��̬��Ϣ
	  �����������
	  �� �� ֵ��NFAPartInfo����NFA��ʼ��̬��Ϣ
	  ˵    ������
	***************************************************************************/
	NFAPartInfo NFA::GetInfo()const {
		return NFAPartInfo(this->start, this->end);
	}

	/***************************************************************************
	  �������ƣ�GetTransList
	  ��    �ܣ����ظ�NFA��Ӧ��ͼ
	  �����������
	  �� �� ֵ��const GraphList<int, string>*��ָ���NFA��Ӧͼ��ָ��
	  ˵    ������
	***************************************************************************/
	const GraphList<int, string>* NFA::GetTransList()const {
		return &this->transList;
	}

	/***************************************************************************
	  �������ƣ�GetEndList
	  ��    �ܣ����ظ�NFA����̬�б�
	  �����������
	  �� �� ֵ��const set<int>*��ָ���NFA��̬�б��ָ��
	  ˵    ������
	***************************************************************************/
	const set<int>* NFA::GetEndList()const {
		return &this->endList;
	}

	/***************************************************************************
	  �������ƣ�GetEndInfoMap
	  ��    �ܣ����ظ�NFA��̬��Ϣ��
	  �����������
	  �� �� ֵ��const map<int, NFAEndInfo>*��ָ���NFA��̬��Ϣ���ָ��
	  ˵    ������
	***************************************************************************/
	const map<int, NFAEndInfo>* NFA::GetEndInfoMap()const {
		return &this->endInfoMap;
	}

	/***************************************************************************
	  �������ƣ�MakeTotalNFA
	  ��    �ܣ���������NFA
	  ���������const int newStart���µ�ʼ̬
	            set<int>& newEndList���µ���̬�б�
				map<int, NFAEndInfo>&���µ���̬��Ϣ��
	  �� �� ֵ����
	  ˵    ��������LEX��������NFA����һ��ʼ̬
	***************************************************************************/
	void NFA::MakeTotalNFA(const int newStart, set<int>& newEndList, map<int, NFAEndInfo>& newEndInfoMap) {
		//���µ�ǰNFA��ʼ̬��ʼ̬�б���̬�б��ж����̬�ˣ��޷�������̬��
		this->start = newStart;
		this->startList.clear();
		this->startList.insert(newStart);
		this->endList = newEndList;

		//������״̬ת�Ʊ���NFA��Ӧ��ͼ
		for (NFATrans rule : totalTransList) {
			if (this->transList.getVertexPos(rule.start) == -1)  //����ʼ̬�����ڣ���ӽ��
				this->transList.insertVertex(rule.start);
			if (this->transList.getVertexPos(rule.end) == -1)    //����̬�����ڣ���ӽ��
				this->transList.insertVertex(rule.end);

			//��ӱ�
			this->transList.insertEdge(this->transList.getVertexPos(rule.start), this->transList.getVertexPos(rule.end), rule.recv);
		}

		//������̬��Ϣ��
		this->endInfoMap.clear();
		for (auto info : newEndInfoMap) {
			this->endInfoMap.emplace(info);
		}
	}

	/***************************************************************************
	  �������ƣ�Show
	  ��    �ܣ���ʾNFA��Ϣ
	  �����������
	  �� �� ֵ����
	  ˵    ����������
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
			cout << "״̬��" << (*iter).first << endl
				<< "����ʽ��" << (*iter).second.regular << endl
				<< "���ȼ���" << (*iter).second.priority << endl;

			iter++;
		}
	}
}