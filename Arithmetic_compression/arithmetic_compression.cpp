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
// Функция переноса найденных битов в файл
void BitsPlusFollow(int bit, int bits_to_follow, ofstream& file_coded) {
	file_coded << bit;
	for (; bits_to_follow > 0; bits_to_follow--) {
		if (bit == 0) {
			file_coded << 1;
		}
		else {
			file_coded << 0;
		}
	}
}

// Кодируем
void compressor(string text, set<pair<char, int>, comp> set) {
	ofstream file_coded("encoded_text.txt");
	unordered_map <char, int> ch;	// Первый и второй столбцы таблицы
	unordered_map <int, int> freq_;	// Первый и третий столбцы таблицы
	unordered_map <int, int> b;	// Первый и четвёртый столбцы таблицы
	unordered_map <int, array<int, 2>> boarders;

	ch[0] = NULL;
	freq_[0] = NULL;
	b[0] = 0;
	
	int l = 0, j = 1;
	for (auto pair: set) {
    		ch[pair.first] = j;
		freq_[j] = pair.second;
		b[j++] = l + pair.second;
    		l += pair.second;
	}
    	int left = 0, right = 65535, i = 0, delitel = b[j-1];
	boarders[0].at(0) = left;
	boarders[0].at(1) = right;
	int First_qtr = (right + 1) / 4;	// 16384
	int Half = First_qtr * 2;	// 32768
	int Third_qtr = First_qtr * 3;	// 49152
	int bits_to_follow = 0;	// Сколько битов сбрасывать

    	for (char index: text) {
		j = ch[index];	// Считываем символ, в таблице находим его индекс
		i++;
		cout << "step №" << i << " j ==" << j << "\n";
		boarders[i].at(0) = boarders[i-1].at(0) + b[j-1] * (boarders[i-1].at(1) - boarders[i-1].at(0)) / delitel;
		boarders[i].at(1) = boarders[i-1].at(0) + b[j] * (boarders[i-1].at(1) - boarders[i-1].at(0)) / delitel - 1 ;
		// Обрабатываем варианты переполнения
		for (;;) {
			if (boarders[i].at(1) < Half) {
				BitsPlusFollow(0, bits_to_follow, file_coded);
			}
			else if (boarders[i].at(0) >= Half) {
				BitsPlusFollow(1, bits_to_follow, file_coded);
				boarders[i].at(0) -= Half;
				boarders[i].at(1) -= Half;
			}
			else if (boarders[i].at(0) >= First_qtr && boarders[i].at(1) < Third_qtr) {
				bits_to_follow++;
				boarders[i].at(0) -= First_qtr;
				boarders[i].at(1) -= First_qtr;
			}
			else {
				break;
			}
			boarders[i].at(0) += boarders[i].at(0);
			boarders[i].at(1) += boarders[i].at(1) + 1;
		}
	}
	file_coded.close();
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
	ofstream file_codes("Alphabet.txt");	// Открываем файл для записи
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
	file_codes.close();	// Закрываем файл
	// Кодируем
	compressor(in_len, text, set);
	cout << "\nFile was succesfully encoded.\n";
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
