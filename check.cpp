#include <fstream>
#include <string>
#include <queue>
#include <unordered_map>
using namespace std;

// узел дерева
class Node {
public:
	char ch;		// символ
	int freq;		// частота
	Node *left, *right;	// указатели на дочерние узлы (на последователей)
};

// выделение нового узла
Node* allocNode(char ch, int freq, Node* left, Node* right) {
	Node* node = new Node();	// выделяем память под новый узел
	// заполняем ячейки соотв. элементами
	node->ch = ch;
	node->freq = freq;
	node->left = left;
	node->right = right;

	return node;
}

// тип compare, обеспечивающий строгий слабый порядок (необходим для использования prority_queue)
class compare {
public:
	bool operator()(Node* left, Node* right) {
		// переопределяем предикат сравнения
		return left->freq > right->freq;
	}
};

// проходимся по дереву и сохраняем коды Хаффмана в map
void encode(Node* root, string str, unordered_map <char, string> &alphabet) {
	if (root == nullptr) {return;}

	// если нашли лист (без дочерних узлов)
	if (!root->left && !root->right) {
		alphabet[root->ch] = str;
	}

	encode(root->left, str + "0", alphabet);
	encode(root->right, str + "1", alphabet);
}

// проходимся по дереву и расшифровываем закодированную строку
void decode(Node* root, int &index, string str, ostream &file_out) {  // передаём поток вывода в качестве формального параметра
	if (root == nullptr) {return;}

	// найден лист
	if (!root->left && !root->right) {
		file_out << root->ch;
		return;
	}

	index++;

	if (str[index] == '0') {decode(root->left, index, str, file_out);}
	else {decode(root->right, index, str, file_out);}
}

// строим дерево Хаффмана и декодируем заданный входной текст
void HuffmanTree(string text) {
	ofstream file_out("out_text.txt"), file_coded("encoded_text.txt"), file_codes("Alphabet.txt");
	// подсчитываем частоту появления каждого сивола и сохраняем ее на карте
	unordered_map <char, int> freq;
	// цикл ranged-based-for
	for (char ch: text) {
		freq[ch]++;
	}

	// создаём очередь с приоритетами для хранения узлов дерева Хаффмана
	priority_queue <Node*, vector<Node*>, compare> pr_q;

	// создаём конечный узел для каждого символа и добавляем его в очередь с приоритетами
	for (auto pair: freq) {
		// добавляем элемент в очередь приоритета на основе приоритета элемента из comp
		pr_q.push(allocNode(pair.first, pair.second, nullptr, nullptr));
	}

	// делаем это до тех пор, пока в очереди не окажется более одного узла
	while (pr_q.size() > 1)	{
		// удаляем два узла с наивысшим приоритетом (самая низкая частота) из очереди
		Node *left = pr_q.top(); pr_q.pop();
		Node *right = pr_q.top();	pr_q.pop();
		
		// Создаваём новый узел дерева, в котором они оба будут потомками
		// Приоритет нового узла будет равен сумме их приоритетов
		int sum = left->freq + right->freq;
		// Добавим получившийся узел обратно в очередь
		pr_q.push(allocNode('\0', sum, left, right));
	}

	// root хранит указатель на корень дерева Хаффмана
	Node* root = pr_q.top();

	// проходим по дереву и сохраняем коды Хаффмана в map
	unordered_map <char, string> alphabet;
	encode(root, "", alphabet);

	for (auto pair: alphabet) {
		file_codes << pair.first << " " << pair.second << '\n';
	}

	// выводим закодированную строку
	string str = "";
	for (char ch: text) {str += alphabet[ch];}
	file_coded << str;
	
	// снова проходим по дереву и расшифровываем закодированную строку
	int index = -1;
	while (index < (int)str.size() - 2) {
		decode(root, index, str, file_out);
	}
	file_out.close(), file_coded.close(), file_codes.close();
}

// Алгоритм кодирования Хаффмана
int main() {
	ifstream file_in("in_text.txt");
	string text, a;
	while (!file_in.eof()) {
		getline(file_in,a);
		text += a + '\n';
	};
	text.pop_back();
	HuffmanTree(text);
	file_in.close();
	return 0;
}
