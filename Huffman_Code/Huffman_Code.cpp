#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <string>
using namespace std;

// узел дерева
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

// Декодируем, проходясь по дереву, и записываем декодированные символы в файл out_text
// Для этого передаём поток вывода в качестве формального параметра
void decode(Node* root, int &index, string str, ostream &file_out) {
 	if (root == nullptr) {return;}	// Проверяем, существует ли вершина
	// Если нашли узел без последователей, записываем соотв. символ в файл
	if (!root->left && !root->right) {
		file_out << root->ch;
		return;
	}
	
	index++;
	// Рекурсивно проходим по ветвям дерева (в зависимости от рассматриваемого символа)
	if (str[index] == '0') {
		decode(root->left, index, str, file_out);
	}
	else {
		decode(root->right, index, str, file_out);
	}
	
}

// Дерево Хаффмана
void HuffmanTree(string text) {
	ofstream file_codes("Alphabet.txt"), file_coded("encoded_text.txt"), file_out("out_text.txt");	// Открываем файлы для записи
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
	
	// выводим закодированный текст
	string str = "";
	for (char ch: text) {
		str += alphabet[ch];
	}
	file_coded << str;

	// Сохраним вид нашего алфавита в файл Alphabet (для визуализации) (циклом ranged-based for)
	for (auto pair: alphabet) {
		file_codes << pair.first << " " << pair.second << '\n';	// Обращаемся к первому и второму полю alphabet соответственно
	}
	
	// Вводим индекс для поэлементной работы со строкой закодированных символов
	int index = -1;
	// Выполняем декодирование текста
	while (index < (int)str.size() - 2) {
		decode(root, index, str, file_out);
	}
	
	file_codes.close(), file_coded.close(), file_out.close();	// Закрываем файлы
}

int main() {
	ifstream file_in("in_text.txt");	// Открываем файл для чтения
	string text, tmp;
	// Пока не достигли конца файла, построчно считываем данные
	while (!file_in.eof()) {
		getline(file_in, tmp);	// Т.к. функция getline() считывает до символа конца строки,
		text += tmp + '\n';	// Не включая их, дополняем текст этим элементов
	};
	text.pop_back();	// Исключаем лишний символ конца строки (\n)
  	file_in.close();	// Закрываем файл
	
  return 0;
}
