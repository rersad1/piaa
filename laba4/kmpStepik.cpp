#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Вычисление префикс-функции
vector<int> computeLPS(const string& pattern) {
    int patternLength = pattern.length();
    vector<int> lps(patternLength, 0);
    
    cout << "--- Вычисление LPS для образца: \"" << pattern << "\" ---" << endl;

    int prefixLength = 0; 
    int position = 1;
    
    if (patternLength == 0) {
        cout << "Образец пустой, LPS не вычисляется." << endl;
        return lps;
    }

    while (position < patternLength) {
        cout << "  Сравниваем pattern[" << position << "] ('" << pattern[position] << "') и pattern[" << prefixLength << "] ('" << pattern[prefixLength] << "')" << endl;
        if (pattern[position] == pattern[prefixLength]) {
            prefixLength++;
            lps[position] = prefixLength;
            cout << "  Символы совпали. lps[" << position << "] = " << lps[position] << endl;
            position++;
        } 
        else {
            cout << "  Символы не совпали." << endl;
            if (prefixLength != 0) {
                prefixLength = lps[prefixLength - 1];
                cout << "  Обновляем prefixLength = " << prefixLength << endl;
            } 
            else {
                lps[position] = 0;
                cout << "  lps[" << position << "] = 0" << endl;
                position++;
            }
        }
    }
    
    cout << "Итоговый LPS массив: [";
    for (size_t i = 0; i < lps.size(); ++i) {
        cout << lps[i] << (i == lps.size() - 1 ? "" : ", ");
    }
    cout << "]" << endl;
    cout << "--- Конец вычисления LPS ---" << endl;
    return lps;
}

// Алгоритм Кнута-Морриса-Пратта для поиска первого вхождения образца в тексте
int kmpSearch(const string& text, const string& pattern) {
    int textLength = text.length();
    int patternLength = pattern.length();
    
    cout << "\n--- Поиск KMP (первое вхождение) ---" << endl;
    cout << "Текст: \"" << text << "\", Образец: \"" << pattern << "\"" << endl;

    if (patternLength == 0) {
        cout << "Образец пустой, возвращаем 0." << endl;
        return 0;
    }
    if (patternLength > textLength) {
        cout << "Образец длиннее текста, возвращаем -1." << endl;
        return -1;
    }

    vector<int> lps = computeLPS(pattern);
    
    int textIndex = 0;    
    int patternIndex = 0; 

    while (textIndex < textLength) {
        cout << "  Сравниваем text[" << textIndex << "] ('" << text[textIndex] << "') и pattern[" << patternIndex << "] ('" << pattern[patternIndex] << "')" << endl;

        if (pattern[patternIndex] == text[textIndex]) {
            cout << "  Символы совпали." << endl;
            textIndex++;
            patternIndex++;
        }
        
        if (patternIndex == patternLength) {
            cout << "  !!! Образец найден по индексу " << textIndex - patternIndex << " !!!" << endl;
            cout << "--- Конец поиска KMP ---" << endl;
            return textIndex - patternIndex; 
        }
        else if (textIndex < textLength && pattern[patternIndex] != text[textIndex]) {
            cout << "  Символы не совпали." << endl;
            if (patternIndex != 0) {
                patternIndex = lps[patternIndex - 1];
                cout << "  Обновляем patternIndex = " << patternIndex << endl;
            } 
            else {
                cout << "  Увеличиваем textIndex." << endl;
                textIndex++;
            }
        }
    }
    
    cout << "Образец не найден. Возвращаем -1." << endl;
    cout << "--- Конец поиска KMP ---" << endl;
    return -1;
}

// Проверка, является ли строка A циклическим сдвигом строки B
int isCyclicShift(const string& A, const string& B) {
    cout << "\n--- Проверка циклического сдвига ---" << endl;
    cout << "Строка A: \"" << A << "\", Строка B: \"" << B << "\"" << endl;

    if (A.length() != B.length()) {
        cout << "Длины строк не совпадают. Результат: -1" << endl;
        cout << "--- Конец проверки ---" << endl;
        return -1;
    }

    if (A.length() == 0) {
        cout << "Обе строки пустые. Результат: 0" << endl;
        cout << "--- Конец проверки ---" << endl;
        return 0;
    }

    string BB = B + B;
    cout << "Создана удвоенная строка BB: \"" << BB << "\"" << endl;
    cout << "Ищем строку A в строке BB..." << endl;
    int index = kmpSearch(BB, A);
    
    if (index != -1) {
        int shift = (B.length() - index) % B.length();
        cout << "A найдена в BB. Сдвиг: " << shift << endl;
        cout << "--- Конец проверки ---" << endl;
        return shift;
    }
    
    cout << "A не найдена в BB. Результат: -1" << endl;
    cout << "--- Конец проверки ---" << endl;
    return -1;
}

int main() {
    string A, B;

    getline(cin, A);
    getline(cin, B);

    int result = isCyclicShift(A, B);
    cout << "Итоговый результат: " << result << endl; 
    
    return 0;
}