#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <string>
#include <bitset>
using namespace std;

// Узел дерева
class Node {
public:
	char ch;	// Символ текста
	int freq;	// Частота символа
	Node *left, *right;	// Указатели на последователей
};

// Выделение нового узла
Node* allocNode(char ch, int freq, Node *left, Node *right) {
	Node* node = new Node();	// Выделяем память под новый узел
	// Заполняем её соотв. элементами
	node->ch = ch;
	node->freq = freq;
	node->left = left;
	node->right = right;
	return node;
}

// Тип compare, обеспечивающий строгий слабый порядок (необходим для использования prority_queue)
class compare {
public:
	bool operator()(Node* left, Node* right) {
		// Переопределяем предикат сравнения (для сравнения частоты)
		return left->freq > right->freq;
	}
};

// Кодируем, проходясь по сформированному дереву
void encode(Node* root, string str, unordered_map <char, string> &alphabet) {
 	if (root == nullptr) {return;}	// Проверяем, существует ли вершина
	
	// Если нашли узел без последователей, записываем символ в алфавит (подготовленный map)
	if (!root->left && !root->right) {
		alphabet[root->ch] = str;
	}
	// Рекурсивно проходим по ветвям дерева (начиная с левых), дописывая в формируемый код символа соотв. значения
	encode(root->left, str + "0", alphabet);
	encode(root->right, str + "1", alphabet);
}
// Декодируем
void decode(string path) {
	ofstream file_out("out_text.txt");	// Открываем файл для записи
	ifstream file_coded(path);
	
	// map для сохранения символов (коды символов передаём в качетсве ключа)
	unordered_map <string, char> alphabet;
	
	char ch = ' ';
	string code, text, buffer;
	getline(file_coded, text);
	bitset<8> tmp(text[0]);
	int power = (int)(tmp.to_ulong());
	
	// Заполняем map данными из файла-алфавита
	for (int ind = 0; ind < power; ind++) {
		code = "";
		getline(file_coded, text);
		bool flag = true;
		for (auto i: text) {
			if (flag) {
				ch = i;
				flag = false;
			}
			else if (!flag && i != ' ') {
				code += i;
			}
		}
		// Обработка символа новой строки (\n)
		if (code == "") {
			ch = '\n';
			getline(file_coded, text);
			for (auto i: text) {
				if (i != ' ') {
					code += i;
				}
			}
		}
		if (code != "") {
			alphabet[code] = ch;
		}
	}

	int flag;
	text = "";
	string key = "";
	while (!file_coded.eof()) {
		getline(file_coded, buffer);
		text += buffer;
		text += "\n";
	}
	text.pop_back();
	bitset<8> bit_key (text[0]);
	flag = int(bit_key.to_ulong());
	for (int l = 1; l < text.length(); l++) {
		bitset<8> tmp(text[l]);
		if (l != text.length() - 1) {
			for (int i = 7; i >= 0; i--) {
				if (tmp.test(i)) {
					key += '1';
				}
				else {
					key += '0';
				}
				// Проверяем, существует ли в алфавите элемент с таким ключом
				if (alphabet.find(key) != alphabet.end()) {
					file_out << alphabet[key];
					key = "";
				}
			}
		}
		else if (text[l] != '\n') {
			for (int i = 7; i >= flag; i--) {
				if (tmp.test(i)) {
					key += '1';
				}
				else {
					key += '0';
				}
				// Проверяем, существует ли в алфавите элемент с таким ключом
				if (alphabet.find(key) != alphabet.end()) {
					file_out << alphabet[key];
					key = "";
				}
			}
		}
	}
	// Данная реализация декодирования возможна благодаря тому, что любой код не является префиксом для кода другого символа
	file_coded.close(), file_out.close();
}

// Дерево Хаффмана
void HuffmanTree(string text) {
	ofstream file_coded("encoded_text.txt");	// Открываем файлы для записи
	// map для сохранения частоты символов
	unordered_map <char, int> freq;

	// Подсчитываем частоту символов (циклом ranged-based for)
	for (char ch: text) {
		freq[ch]++;
	}

	// Очередь с приоритетами
	priority_queue <Node*, vector<Node*>, compare> pr_q;

	// Создадим узлы бинарного дерева для каждого знака и добавим их в очередь, используя частоту в качестве приоритета
	for (auto pair: freq) {
		// Добавляем элемент в очередь с приоритетами на основе приоритета элемента из comp
		pr_q.push(allocNode(pair.first, pair.second, nullptr, nullptr));
	}

	// Связывание узлов
	while (pr_q.size() > 1)	{
		// Удаляем из очереди два узла с наивысшим приоритетом (самой низкой частотой) (сохраняя указатели на удаляемые узлы)
		Node *left = pr_q.top();
		pr_q.pop();
		Node *right = pr_q.top();
		pr_q.pop();
		
		// Создаваём новый узел дерева, в котором они оба будут потомками
		// Приоритет нового узла будет равен сумме их приоритетов
		int sum = left->freq + right->freq;
		// Добавим получившийся узел обратно в очередь (в качестве символа передаём нулевой символ)
		pr_q.push(allocNode('\0', sum, left, right));
	}

	// Сохраним указатель на вершину (корень) дерева
	Node *root = pr_q.top();
	
	// map для сохранения кодов символов (неупорядоченный в связи с его потенциальным выигрышем во времени записи/удаления)
	unordered_map <char, string> alphabet;
	
	// Выполняем кодирование текста
	encode(root, "", alphabet);
	cout << "\nFile was succesfully encoded.\n";
	
	int power;
	string alphab;
	// Сохраним вид нашего алфавита в файл
	for (auto pair: alphabet) {
		power++;
		// Обращаемся к первому и второму полю alphabet соответственно
		alphab += pair.first;
		alphab += ' ';
		alphab += pair.second;
		alphab += "\n";
	}
	
	// Выводим закодированный текст
	int flag = 0;
	string str = "";
	bitset<8> code_bit;
	bitset<8> bit_power(power);

	for (char ch: text) {
		for (char code: alphabet[ch]) {
			flag++;
			code_bit <<= 1;
			if (flag % 8 == 0) {
				if (code == '1') {
					code_bit.set(0);
				}
				str += char(code_bit.to_ulong());
				code_bit.reset();
				flag = 0;
			}
			else if (code == '1') {
					code_bit.set(0);
			}
		}
	}
	flag = 8 - flag;
	if (flag % 8 != 0) {
		for (int i = 0; i < flag; i++) {
			code_bit <<= 1;
		}
	}
	else {
		flag = 0;
	}
	bitset<8> code_flag(flag);
	file_coded << char(bit_power.to_ulong()) << "\n" << alphab << char(code_flag.to_ulong()) << str << char(code_bit.to_ulong());
	
	file_coded.close();	// Закрываем файлы
}

int main() {
	string text, tmp, path, act;
	// Запрашиваем путь к файлу (сам файл, если он находится в одной директории с исполняемой программой)
	cout << "Please, enter the path to the file: ";
	cin >> path;
	ifstream file_in(path);	// Открываем файл для чтения
	// Проверяем, существует ли файл/открыт ли к нему доступ
	if(!file_in) {
		cout << "\nThe file does not exist or access to it is denied. Please, try again.\n";
		return 0;
	}
	// Запрашиваем действие, которое необходимо выполнить
	cout << "\nYou will not be able to decode the file without its primary encoding!\n\nEnter 1 if you want to encode file, enter 2 in other way: ";
	cin >> act;
	// Проверяем, корректно ли введён индекс действия
	if (act != "1" && act != "2") {
		cout << "\nAn incorrect value was entered. Please, try again.\n";
		return 0;
	}
	// Выполняем кодирование (если поступил соотв. запрос)
	if (act == "1") {
		// Пока не достигли конца файла, построчно считываем данные
		while (!file_in.eof()) {
			getline(file_in, tmp);	// Т.к. функция getline() считывает до символа конца строки,
			text += tmp + '\n';	// Не включая их, дополняем текст этим элементом
		};
		file_in.close();	// Закрываем файл
		text.pop_back();	// Исключаем лишний символ конца строки (\n)
		HuffmanTree(text);
	}
	// Выполняем декодирование
	else if (act == "2") {
		// Выполняем декодирование текста
		decode(path);
		cout << "\nFile was succesfully decoded.\n";
	}
  return 0;
}
