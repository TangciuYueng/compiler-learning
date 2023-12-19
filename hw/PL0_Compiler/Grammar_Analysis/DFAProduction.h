#ifndef _DFA_PRODUCTION_H_
#define _DFA_PRODUCTION_H_
#include<iostream>
#include<string>
#include<vector>
#include<set>
#include"Element.h"
using namespace std;

namespace grammar {
	/*LR(1)��Ŀ*/
	struct DFAProduction {
		string left;            //��
		vector<Element> right;  //�Ҳ�
		int point;              //Բ��λ��
		set<string> outlook;    //չ������

		DFAProduction(string left, vector<Element>& right, set<string>& outlook) : left(left), point(0), outlook(outlook) {
			vector<Element>::iterator iter = right.begin();

			while (iter != right.end()) {  //LR(1)��Ŀ��û�п��֣���Ҫȥ��
				if (iter->word != "EPSILON")
					this->right.push_back(*iter);

				iter++;
			}
		}

		bool begin() {
			return this->point == 0;
		}

		bool end() {
			return this->point == this->right.size();
		}

		bool nextEnd() {
			return this->end() || this->point + 1 == this->right.size();
		}

		void add() {
			if (!this->end())
				this->point++;
		}

		Element GetPoint() {
			if (!this->end())
				return this->right[this->point];

			return Element("ERROR", true);
		}

		friend bool operator==(const DFAProduction& left, const DFAProduction& right) {
			return left.left == right.left && left.right == right.right && left.point == right.point && left.outlook == right.outlook;
		}

		friend bool operator<(const DFAProduction& left, const DFAProduction& right) {
			if (left.left != right.left) {
				return left.left < right.left;
			}
			else if (left.right != right.right) {
				return left.right < right.right;
			}
			else if (left.outlook != right.outlook) {
				return left.outlook < right.outlook;
			}
			else {
				return left.point < right.point;
			}
		}

		friend ostream& operator<<(ostream& out, const DFAProduction& production) {
			out << production.left << " --> ";

			vector<Element>::const_iterator iter = production.right.begin();
			while (iter != production.right.end()) {
				if (iter - production.right.begin() == production.point) {
					out << "��" << iter->word << ' ';
				}
				else {
					out << iter->word << ' ';
				}

				iter++;
			}

			if (production.point == production.right.size()) {
				out << "��";
			}

			out << '(';
			for (string outlook : production.outlook) {
				out << outlook << ' ';
			}
			out << ')';

			return out;
		}
	};
}

#endif
