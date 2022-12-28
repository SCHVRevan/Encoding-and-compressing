#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <string>
using namespace std;

// узел дерева
class Node() {
public:
	char ch;	// Символ текста
	int freq;	// Частота символа
	Node *left, *right;	// Указатели на последователей
};

// Выделение нового узла
Node *allocNode(char ch, int freq, Node *left, Node *right) {
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

// кодируем
void encode() {
 	
}
// декодируем
void decode() {
 	
}
// Дерево Хаффмана
void HuffmanTree(string text) {
	// map для сохранения частоты символов
	unordered_map <char, int> freq;

	// Подсчитываем частоту символов (цикл ranged-based for)
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
		Node *left = pr_q.top(); pr_q.pop();
		Node *right = pr_q.top(); pr_q.pop();
		
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
}

int main() {
	// Открываем файл для чтения
	ifstream file_in("in_text.txt");
	// Открываем файлы для записи
	ofstream file_out("out_text.txt"), file_coded("encoded_text.txt"), file_codes("Alphabet.txt");
	string text, tmp;
	// Пока не достигли конца файла, построчно считываем данные
	while (!file_in.eof()) {
		getline(file_in, tmp);	// Т.к. функция getline() считывает до символа конца строки,
		text += tmp + '\n';	// Не включая их, дополняем текст этим элементов
	};
	text.pop_back();	// Исключаем лишний символ конца строки (\n)
  	file_in.close(), file_out.close(), file_coded.close(), file_codes.close();	// Закрываем файлы
	
  return 0;
}
