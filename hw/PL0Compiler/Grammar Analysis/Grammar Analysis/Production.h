#ifndef _PRODUCTION_H_
#define _PRODUCTION_H_
#include<iostream>
#include<string>
#include<vector>
#include"Element.h"
using namespace std;

struct Production {
	string left;
	vector<Element> right;

	Production(string left, vector<Element> right) : left(left) {
		if (right.size() == 0)
			this->right.push_back(Element("EPSILON", true));
		else
			this->right = right;
	}

	friend ostream& operator<<(ostream& out, const Production& production) {
		out << production.left << " -> ";

		for (Element element : production.right) {
			out << element.word << ' ';
		}

		return out;
	}

	friend bool operator==(const Production& left, const Production& right) {
		return left.left == right.left && left.right == right.right;
	}

	friend bool operator!=(const Production& left, const Production& right) {
		return left.left != right.left || left.right != right.right;
	}

	friend bool operator<(const Production& left, const Production& right) {
		if (left.left != right.left) {
			return left.left < right.left;
		}

		return left.right < right.right;
	}

	friend bool operator>(const Production& left, const Production& right) {
		if (left.left != right.left) {
			return left.left > right.left;
		}

		return left.right > right.right;
	}

	friend bool operator<=(const Production& left, const Production& right) {
		if (left.left != right.left) {
			return left.left <= right.left;
		}

		return left.right <= right.right;
	}

	friend bool operator>=(const Production& left, const Production& right) {
		if (left.left != right.left) {
			return left.left >= right.left;
		}

		return left.right >= right.right;
	}
};

#endif
