#include <iostream>
#include <fstream>
#include <unordered_map>
#include <set>
#include <string>
#include <array>
using namespace std;

// Тип compare (необходим для изменения параметров сортировки в set)
struct comp {
    template<typename T>
    bool operator()(const T &l, const T &r) const
    {
        if (l.second != r.second) {
            return l.second > r.second;
        }
 
        return l.first > r.first;
    }
};

// Кодируем
double compressor(int in_len, string text, set<pair<char, double>, comp> set) {
	// Формируем map с полями под границы интервалов (в качестве ключей - символы) 
	unordered_map <char, array<int, 2>> segment;
	array<int, 2> interval;
	int l = 0;
	cout << "\n\tChecking ranges\n";
	for (auto pair: set) {
		interval = {l, l + pair.second};
		cout << interval.at(0) << ' ' << interval.at(1) << "\n";
    		segment[pair.first].at(0) = interval.at(0);
		segment[pair.first].at(1) = interval.at(1);
    		l = l + pair.second;
	}
	// Реализуем дальнейшие шаги алгоритма
	char ch;
   	int  newRight, newLeft, left = 0, right = 1;
	cout << "\n\tCheck boarders:\n";
    	for (int i = 0; i < in_len; i++) {
		ch = text[i];
		cout << "old left: " << left << " old right: " << right << "\n";
        	newLeft = left + segment[ch].at(0) * (right - left);
		cout << "new left: " << newLeft;
		newRight = left + segment[ch].at(1) * (right - left);
		cout << " new right: " << newRight << "\n\n";
        	right = newRight;
		left = newLeft;
	}
    return (left + right) / 2;
}

void decompressor() {
	ofstream file_out("out_text.txt");
	unordered_map <char, array<int, 2>> segment;
	array<int, 2> interval;
	int l = 0;
	cout << "\n\tChecking ranges\n";
	for (auto pair: set) {
		interval = {l, l + pair.second};
		cout << interval.at(0) << ' ' << interval.at(1) << "\n";
    		segment[pair.first].at(0) = interval.at(0);
		segment[pair.first].at(1) = interval.at(1);
    		l = l + pair.second;
	}

    	string tmp = "";
    	for (int i = 0; i < in_len - 1; i++) {
		for (auto pair: segment) {
            		if (code >= segment[pair.first].at(0) && code < segment[pair.first].at(1)) {
                		tmp += pair.first;
                		code = (code - segment[pair.first].at(0)) / (segment[pair.first].at(1) - segment[pair.first].at(0));
                		break;
			}
		}
	}
	if (tmp != "") {
		file_out << tmp;
	}
	else {
		cout << "\nSomething went wrong\n";
	}
	file_out.close();
}
}

void Alphabet(string text) {
	ofstream file_codes("Alphabet.txt"), file_coded("encoded_text.txt");	// Открываем файлы для записи
	// map для сохранения частоты символов
	unordered_map <char, int> freq;
	// map для сохранения вероятностей символов
	unordered_map <char, double> alphabet;
	// Сохраняем исходную длину текста
	int in_len = text.length();
	file_codes << in_len << "\n";
	// Подсчитываем частоту символов (циклом ranged-based for)
	for (char ch: text) {
		freq[ch]++;
		alphabet[ch] = freq[ch];
	}
	// Перезаписываем в set для дальнейшего использования в отсортированном виде
	set<pair<char, double>, comp> set(alphabet.begin(), alphabet.end());
	// Выводим алфавит в соотв. файл
	for (auto const &pair: set) {
        	file_codes << pair.first << ' ' << pair.second << "\n";
    	}
	// Кодируем
	file_coded << compressor(in_len, text, set);
	cout << "\nFile was succesfully encoded.\n";
	
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
		Alphabet(text);
	}
	// Выполняем декодирование
	else if (act == "2") {
		ifstream file_codes("Alphabet.txt");
		int in_len, coded_text = stoi(text);
				
		unordered_map <char, int> alphabet;
		
		char ch = ' ';
		string code, buffer;

		getline(file_codes, buffer);
		in_len = stoi(buffer);
		
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
				alphabet[ch] = stoi(code);
			}
		}

		set<pair<char, int>, comp> set(alphabet.begin(), alphabet.end());
		
		file_codes.close();	// Закрываем файл-алфавит
		// Выполняем декодирование текста
		decompressor(in_len, coded_text, set);
		cout << "\nFile was succesfully decoded.\n";
	}
  return 0;
}
