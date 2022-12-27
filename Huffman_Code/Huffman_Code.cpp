#include <iostream>
#include <fstream>
#include <queue>
#include <map>
#include <string>

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

// кодируем
void encode() {
 	
}
// декодируем
void decode() {
 	
}
// дерево Хаффмана (для декодирования шифртекста)
void HuffmanTree() {
 	
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
