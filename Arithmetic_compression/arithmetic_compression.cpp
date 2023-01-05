#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
using namespace std;

void compressor() {
 	
}

void decompressor() {
	
}

void Arithmetic(string text) {
	ofstream file_codes("Alphabet.txt");	// Открываем файл для записи
  
	unordered_map <char, int> freq;
	unordered_map <char, double> alphabet;

	int in_len = text.length();;

	for (char ch: text) {
		freq[ch]++;
		alphabet[ch] = (int)freq[ch] / in_len;
	}

	// Сохраним вид нашего алфавита в файл Alphabet (циклом ranged-based for)
	for (auto pair: alphabet) {
		file_codes << pair.first << " " << pair.second << '\n';	// Обращаемся к первому и второму полю alphabet соответственно
	}
	file_codes.close();	// Закрываем файл
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
		Arithmetic(text);
	}
	// Выполняем декодирование
	else if (act == "2") {
		cout << "2 was chosen";
		ifstream file_codes("Alphabet.txt");
		
		file_codes.close();	// Закрываем файл-алфавит
	}
  return 0;
}
