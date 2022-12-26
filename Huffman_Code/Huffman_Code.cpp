#include <iostream>
#include <fstream>
#include <queue>
#include <map>
#include <string>
// узел бинарного дерева
class Node() {
  
};
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
  // открываем файл для чтения
  ifstream file_in("in_text.txt");
  string text, tmp;
  // пока не достигли конца файла, построчно считываем данные
	while (!file_in.eof()) {
		getline(file_in, tmp);  // т.к. функция getline() считывает до символа конца строки,
		text += tmp + '\n';     // не включая их, дополняем текст этим элементов
	};
	text.pop_back();          // исключаем лишний символ конца строки (\n)
  file_in.close();          // закрываем файл
  return 0;
}
