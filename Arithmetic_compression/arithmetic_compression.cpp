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
	ofstream file_coded("encoded_text.bin");
	unordered_map <char, int> ch;	// Первый и второй столбцы таблицы
	unordered_map <int, int> freq_;	// Первый и третий столбцы таблицы
	unordered_map <int, int> b;	// Первый и четвёртый столбцы таблицы
	unordered_map <int, array<int, 2>> boarders;

	freq_[0] = 0;
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
	int First_qtr = (right + 1) / 4;
	int Half = First_qtr * 2;
	int Third_qtr = First_qtr * 3;
	int bits_to_follow = 0;	// Сколько битов сбрасывать

    for (char index: text) {
		j = ch[index];	// Считываем символ, находим его индекс
		i++;
		boarders[i].at(0) = boarders[i-1].at(0) + b[j-1] * (boarders[i-1].at(1) - boarders[i-1].at(0)) / delitel;
		boarders[i].at(1) = boarders[i-1].at(0) + b[j] * (boarders[i-1].at(1) - boarders[i-1].at(0)) / delitel - 1;
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

void decompressor(string text, set<pair<char, int>, comp> set) {
	ofstream file_out("out_text.txt");
	unordered_map <int, char> ch;	// Первый и второй столбцы таблицы
	unordered_map <int, int> freq_;	// Первый и третий столбцы таблицы
	unordered_map <int, int> b;	// Первый и четвёртый столбцы таблицы
	unordered_map <int, array<int, 2>> boarders;
	
	ch[0] = NULL;
	freq_[0] = 0;
	b[0] = 0;
	
	int l = 0, j = 1;
	for (auto pair: set) {
    	ch[j] = pair.first;
		freq_[j] = pair.second;
		b[j++] = l + pair.second;
    	l += pair.second;
	}
    int left = 0, right = 65535, i = 0, delitel = b[j-1];
	boarders[0].at(0) = left;
	boarders[0].at(1) = right;
	int First_qtr = (right + 1) / 4;
	int Half = First_qtr * 2;
	int Third_qtr = First_qtr * 3;

	int value, freq, text_index;
	bitset<16> bit_value;
	bitset<16> sum_bit;
	
	for (text_index = 0; text_index < 16 && text_index < text.length(); text_index++) {
		if (text[text_index] == '1') {
			bit_value.set(15-text_index);
		}
	}
	cout << "bit_value: " << bit_value << "\n";
	value = (int)(bit_value.to_ulong());
	cout << "value: " << value << "\n\n";
	
    for (int i = 1; i < text.length() - text_index; i++) {
		freq = ((value - boarders[i-1].at(0) + 1) * delitel - 1) / (boarders[i-1].at(1) - boarders[i-1].at(0) + 1);
		for (j = 1; b[j] <= freq; j++);	// Поиск символа
		cout << "check j: " << j << "\n\n";
		boarders[i].at(0) = boarders[i-1].at(0) + b[j-1] * (boarders[i-1].at(1) - boarders[i-1].at(0)) / delitel;
		boarders[i].at(1) = boarders[i-1].at(0) + b[j] * (boarders[i-1].at(1) - boarders[i-1].at(0)) / delitel - 1;
		// Обрабатываем варианты переполнения
		for (;;) {
			if (boarders[i].at(1) < Half) {;}	// Ничего
			else if (boarders[i].at(0) >= Half) {
				boarders[i].at(0) -= Half;
				boarders[i].at(1) -= Half;
				value -= Half;
			}
			else if ((boarders[i].at(0) >= First_qtr) && (boarders[i].at(1) < Third_qtr)) {
				boarders[i].at(0) -= First_qtr;
				boarders[i].at(1) -= First_qtr;
				value -= First_qtr;
			}
			else {
				break;
			}
			boarders[i].at(0) += boarders[i].at(0);
			boarders[i].at(1) += boarders[i].at(1) + 1;
			sum_bit.reset();
			if (text[text_index++] == '1') {
				sum_bit.set(0);
			}
			cout << "value check 1 = " << value << "\n";
			bit_value = bitset<16>(value);
			cout << "bit_value check 1 = " << bit_value << "\n";
			cout << "sum_bit = " << sum_bit << "\n";
			if ((sum_bit.test(0) == false && bit_value.test(0) == false) || (sum_bit.test(0) == true && bit_value.test(0) == true)) {
				bit_value <<= 1;
			}
			else {
				bit_value <<= 1;
				bit_value.set(0);
			}
			cout << "bit_value check 2 = " << bit_value << "\n";
			value = (int)(bit_value.to_ulong());
			cout << "value check 2 = " << value << "\n\n";
		}
		file_out << ch[j];
	}
	file_out.close();
}

void Alphabet(string text) {
	ofstream file_codes("Alphabet.txt");	// Открываем файл для записи
	// map для сохранения частоты символов
	unordered_map <char, int> freq;
	// map для сохранения вероятностей символов
	unordered_map <char, double> alphabet;
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
	compressor(text, set);
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
				
		unordered_map <char, int> alphabet;
		
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
				alphabet[ch] = stoi(code);
			}
		}

		set<pair<char, int>, comp> set(alphabet.begin(), alphabet.end());
		
		file_codes.close();	// Закрываем файл-алфавит
		// Выполняем декодирование текста
		decompressor(text, set);
		cout << "\nFile was succesfully decoded.\n";
	}
  return 0;
}
