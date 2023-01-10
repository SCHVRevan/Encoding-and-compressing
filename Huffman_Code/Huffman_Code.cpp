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
void decode(string text, unordered_map <string, char> &alphabet) {
	ofstream file_out("out_text.txt");	// Открываем файл для записи
	int flag;
	string key = "";
	bitset<8> tmp(text[0]);
	flag = int(tmp.to_ulong());
	cout << "flag: " << flag << "\n";
	for (int l = 1; l < text.length(); l++) {
		bitset<8> tmp(text[l]);
		cout << tmp << "\n";
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
		else {
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
	file_out.close();
}

// Дерево Хаффмана
void HuffmanTree(string text) {
	ofstream file_codes("Alphabet.txt"), file_coded("encoded_text.txt");	// Открываем файлы для записи
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
	
	// Сохраним вид нашего алфавита в файл Alphabet (циклом ranged-based for)
	for (auto pair: alphabet) {
		file_codes << pair.first << " " << pair.second << '\n';	// Обращаемся к первому и второму полю alphabet соответственно
	}
	
	// Выводим закодированный текст
	string str = "";
	int flag = 0;
	bitset<8> code_bit;

	for (char ch: text) {
		for (char code: alphabet[ch]) {
			flag++;
			code_bit <<= 1;
			if (flag % 8 == 0) {
				if (code == '1') {
					code_bit.set(0);
				}
				cout << code_bit << "\n";
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
	file_coded << char(code_flag.to_ulong()) << str << char(code_bit.to_ulong());
	
	file_codes.close(), file_coded.close();	// Закрываем файлы
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
	// Пока не достигли конца файла, построчно считываем данные
	while (!file_in.eof()) {
		getline(file_in, tmp);	// Т.к. функция getline() считывает до символа конца строки,
		text += tmp + '\n';	// Не включая их, дополняем текст этим элементом
	};
	file_in.close();	// Закрываем файл
	text.pop_back();	// Исключаем лишний символ конца строки (\n)
	// Выполняем кодирование (если поступил соотв. запрос)
	if (act == "1") {
		HuffmanTree(text);
	}
	// Выполняем декодирование
	else if (act == "2") {
		ifstream file_codes("Alphabet.txt");
		// map для сохранения символов (коды символов передаём в качетсве ключа)
		unordered_map <string, char> alphabet;

		char ch = ' ';
		string code, buffer;
		// Заполняем map данными из файла-алфавита
		while (!file_codes.eof()) {
			code = "";
			getline(file_codes, buffer);
			bool flag = true;
			for (auto i: buffer) {
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
				getline(file_codes, buffer);
				for (auto i: buffer) {
					if (i != ' ') {
						code += i;
					}
				}
			}
			if (code != "") {
				alphabet[code] = ch;
			}
		}
		file_codes.close();	// Закрываем файл-алфавит
		// Выполняем декодирование текста
		decode(text, alphabet);
		cout << "\nFile was succesfully decoded.\n";
	}
  return 0;
}
