#include "iostream"
#include "fstream"
#include "sstream"
#include "string"
#include "ArgumentManager.h"
using namespace std;
//=====================================================================STACK======================================================================
template<class T>
struct node
{
	T f1;
	node <T>*next;
};
template<class T>
class sstack
{
public:
	node <T> *top;
	sstack()
	{
		this->top = NULL;
	}
	bool isempty();
	void push(T);
	T peek();
	T pop();
	void print();
	void reverse();
};
template<class T>
T sstack<T>::peek()
{
	return top->f1;
}
template<class T>
bool sstack<T>::isempty()
{
	if (top == NULL)
		return true;
	else return false;
}
template<class T>
void sstack<T>::reverse()
{
	T x = 0;
	sstack<T> reverse;
	while (!isempty())
	{
		pop(x);
		reverse.push(x);
	}
	top = reverse.top;
}
template<class T>
void sstack<T>::push(T v)
{
	node <T> *temp = new node<T>;
	temp->f1 = v;
	temp->next = top;
	top = temp;
}
template<class T>
T sstack<T>::pop()
{
	T v;
	if (!isempty())
	{
		v = top->f1;
		top = top->next;
	}
	return v;
}
template<class T>
void sstack<T>::print()
{
	node <T> *temp = top;
	while (temp != NULL)
	{
		cout << temp->f1 << endl;
		temp = temp->next;
	}
}
bool error(ofstream& ofs)
{
	ofs << "error" << endl;
	return false;
}
bool checkBalance(string &str, ofstream &ofs)
{
	sstack<char> *bstack = new sstack<char>();
	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == ' ')
		{
			str.erase(i, 1);
			i--;
			continue;
		}
		if (str[0] == '/' || str[0] == '*')
			return error(ofs);
		if (str[str.length() - 1] == '+' || str[str.length() - 1] == '-' || str[str.length() - 1] == '*' || str[str.length() - 1] == '/')
			return error(ofs);
		if (isdigit(str[i]))
			if (str[i + 1] == '(')
				return error(ofs);
		if (!isdigit(str[i]) && str[i] != '(' && str[i] != ')' && str[i] != '+' && str[i] != '-' && str[i] != '*' && str[i] != '/')
			return error(ofs);
		else if (str[i] != '(' && str[i] != ')' && !isdigit(str[i]))
		{
			if (str[i + 1] == ')' || str[i + 1] == '+' || str[i + 1] == '-' || str[i + 1] == '*' || str[i + 1] == '/')
				return error(ofs);
		}
		else if (str[i] == '(')
		{
			if (str[i + 1] == '/' || str[i + 1] == '*' || str[i + 1] == ')')
				return error(ofs);
			bstack->push(str[i]);
		}
		else if (str[i] == ')')
		{
			if (str[i + 1] == '(' || isdigit(str[i + 1]))
			{
				return error(ofs);
			}
			char x = '0';
			if (!bstack->isempty())
				x = bstack->peek();
			if (x == '(')
				bstack->pop();
			else return error(ofs);
		}
	}
	if (!bstack->isempty())
		return error(ofs);
	else return true;
}
//=============================================================INFIX TO POSTFIX=====================================================================
int opValue(char &a)
{
	switch (a)
	{
	case'+': 
	case'-': return 1;
	case'*':
	case'/': return 2;
	}
}
bool opX_larger_opY(char&x, char&y)
{
	int a = opValue(x);
	int b = opValue(y);
	if (a >= b)
		return true;
	else return false;		
}
void Flush(string &postfix, sstack<char> *opstack)
{
	char x;
	do
	{
		x = opstack->pop();
		if (x != '(')
			postfix = postfix + x + " ";
	} while (x != '(' && !opstack->isempty());
}
string convertNegative(string &str)
{
	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == '+' || str[i] == '-')
		{
			if (i == 0)
				str.insert(i, "0");
			else if (str[i + 1] == '(' || isdigit(str[i + 1]))
			{
				if (!isdigit(str[i - 1]))
				{
					if (str[i - 1] == ')')
						str.insert(i, "+0");
					else
						str.insert(i, "0");
				}
			}
		}
	}
	return str;
}
void appendNum(string &num, string &postfix)
{
	if (!num.empty())
	{
		long long x = stoll(num);
		postfix = postfix + to_string(x) + " ";
		num = "";
	}
}
void pushOp(string &num, string &postfix, string &line, sstack<char>*opstack, int i)
{
	appendNum(num, postfix);
	if (line[i] == ')')
		Flush(postfix, opstack);
	else
	{
		if (!opstack->isempty() && opstack->peek() != '(')
		{
			char x = opstack->peek();
			if (opX_larger_opY(x, line[i]))
			{
				opstack->pop();
				postfix = postfix + x + " ";
			}
		}
		opstack->push(line[i]);
	}
}
string infix_to_postfix(string &line, sstack<char> *opstack)
{
	convertNegative(line);
	string postfix = "", num = "";
	for (int i = 0; i < line.length(); i++)
	{
		if (!isdigit(line[i]))
			pushOp(num, postfix, line, opstack, i);
		else
			num = num + line[i];
		if (i == line.length() - 1)
		{
			appendNum(num, postfix);
			if (!opstack->isempty())
				Flush(postfix, opstack);
		}
	}
	return postfix;
}
//=============================================================POSTFIX EVALUATION==================================================================
long long evaluation(string &postfix, bool &divbyzero)
{
	sstack<long long> *longstack = new sstack<long long>();
	string temp;
	long long num = 0, cal = 1;
	stringstream ss(postfix);
	while (getline(ss, temp, ' '))
	{
		if (!isdigit(temp[0]))
		{
			long long x = longstack->pop(), y = longstack->pop();
			if (temp[0] == '/' && x == 0)
			{
				divbyzero = true;
				return 123455679;
			}
			switch (temp[0])
			{
			case'+': cal = y + x;
				break;
			case'-': cal = y - x;
				break;
			case'*': cal = y * x;
				break;
			case'/': cal = y / x;
				break;
			}
			longstack->push(cal);
		}
		else
		{
			num = stoll(temp);
			longstack->push(num);
		}
	}
	return longstack->peek();
}
//=================================================================================================================================================
int main(int argc, char *argv[])
{
	if (argc != 3 && argc != 2)
	{
		cout << "invalid arguments" << endl;
		return 1;
	}
	ArgumentManager am(argc, argv);
	string input = am.get("input");
	string output = am.get("output");
	ifstream ifs(input);
	ofstream ofs(output);
	string line, num, op;
	sstack<char> *opstack = new sstack<char>();
	if (ifs.peek() == ifstream::traits_type::eof())
		ofs << "error" << endl;
	else
	{
		while (!ifs.eof())
		{
			getline(ifs, line);
			if (line.empty())
				continue;
			if (line[line.size() - 1] == '\r')
				line = line.substr(0, line.size() - 1);
			if (line.empty())
				continue;
			else if (!line.empty())
			{
				string ans = line;
				if (checkBalance(line, ofs) == false)
					continue;
				string postfix = infix_to_postfix(line, opstack);
				//cout << postfix << endl;
				bool divbyzero = false;
				long long x = evaluation(postfix, divbyzero);
				if (divbyzero == false)
				{
					ans = ans + "=" + to_string(x);
					ofs << ans << endl;
				}
				else ofs << "error" << endl;
			}
		}
		long long k = -254 * (+3658 + 1254 / 222) + 8956;
		cout << k << endl;
		return 0;
	}
}

/*

int tree::min()
{
	node *cu=root;
	if isempty()
		return -1
	if (cu->left == NULL)
		return cu->data;
	while (cu->left != NULL)
		cu=cu->left;
		return cu->data;
}

int tree::recmin(node *cu)
{
	if (isempty())
		return -1;
	if (cu->left == NULL)
		return cu->data;
	else
		return recmin (cu->left);
}
bool tree::search(node *cu, int key)
{
	if (cu==NULL)
		return false;
	if (cu->data == key)
		return true;
	else if (key < cu->data)
		return search(cu->left, key);
	else if (key > cu->data)
		return search(cu->right, key);
}
void tree::insert(int key)
{
	if (cu == NULL)
	{
		node *temp = new node(key);
		temp->left=temp->right=NULL;
		cu = temp;
	}
	if (cu->data == key)
	{
		cout << "cannot insert duplicate";
		exit (1);
	}
	else if (key < cu->data)
		return insert(cu->left, key)
	else
		return insert(cu->right, key);
}
void tree::inorder(node *cu)
{
	if (cu == NULL)
		return;
	inorder(cu->left);
	cout << cu->data << " ";
	inorder(cu->right);
}


remove function
*/