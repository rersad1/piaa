#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Вычисление префикс-функции
vector<int> computeLPS(const string& pattern) {
    int patternLength = pattern.length();
    vector<int> lps(patternLength, 0);
    
    cout << "--- Вычисление LPS для образца: \"" << pattern << "\" ---" << endl;

    int prefixLength = 0; // Длина предыдущего наибольшего префикса-суффикса
    int position = 1;
    
    cout << "Инициализация: lps[0] = 0, prefixLength = 0, position = 1" << endl;

    while (position < patternLength) {
        cout << "  position = " << position << ", prefixLength = " << prefixLength << endl;
        cout << "  Сравниваем pattern[" << position << "] ('" << pattern[position] << "') и pattern[" << prefixLength << "] ('" << pattern[prefixLength] << "')" << endl;
        if (pattern[position] == pattern[prefixLength]) {
            prefixLength++;
            lps[position] = prefixLength;
            cout << "  Символы совпали. prefixLength = " << prefixLength << ", lps[" << position << "] = " << lps[position] << endl;
            position++;
        } 
        else {
            cout << "  Символы не совпали." << endl;
            if (prefixLength != 0) {
                cout << "  prefixLength != 0. Обновляем prefixLength = lps[" << prefixLength - 1 << "] = " << lps[prefixLength - 1] << endl;
                prefixLength = lps[prefixLength - 1];
            } 
            else {
                lps[position] = 0;
                cout << "  prefixLength == 0. lps[" << position << "] = 0" << endl;
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

// Алгоритм Кнута-Морриса-Пратта для поиска всех вхождений образца в тексте
vector<int> kmpSearch(const string& text, const string& pattern) {
    int textLength = text.length();
    int patternLength = pattern.length();
    vector<int> occurrences;
    
    cout << "\n--- Поиск KMP ---" << endl;
    cout << "Текст: \"" << text << "\"" << endl;
    cout << "Образец: \"" << pattern << "\"" << endl;

    if (patternLength == 0) {
        cout << "Образец пустой, возвращаем пустой список вхождений." << endl;
        return occurrences;
    }

    vector<int> lps = computeLPS(pattern);
    
    int textIndex = 0; // индекс для текста
    int patternIndex = 0; // индекс для образца
    
    cout << "Начало поиска: textIndex = 0, patternIndex = 0" << endl;

    while (textIndex < textLength) {
        cout << "  textIndex = " << textIndex << ", patternIndex = " << patternIndex << endl;
        if (patternIndex < patternLength) { // Добавлена проверка, чтобы избежать выхода за пределы pattern
             cout << "  Сравниваем text[" << textIndex << "] ('" << text[textIndex] << "') и pattern[" << patternIndex << "] ('" << pattern[patternIndex] << "')" << endl;
        }


        // Текущие символы совпадают
        if (patternIndex < patternLength && pattern[patternIndex] == text[textIndex]) {
            cout << "  Символы совпали." << endl;
            textIndex++;
            patternIndex++;
        }
        
        // Образец найден
        if (patternIndex == patternLength) {
            cout << "  !!! Образец найден по индексу " << textIndex - patternIndex << " !!!" << endl;
            occurrences.push_back(textIndex - patternIndex); // Сохраняем начальный индекс
            cout << "  Обновляем patternIndex = lps[" << patternIndex - 1 << "] = " << lps[patternIndex - 1] << " для поиска следующего совпадения." << endl;
            patternIndex = lps[patternIndex - 1]; // Ищем следующее совпадение
        }
        // Несовпадение после patternIndex совпадений
        else if (textIndex < textLength && (patternIndex == patternLength || pattern[patternIndex] != text[textIndex])) { // Условие (patternIndex == patternLength) добавлено для случая, когда образец найден и patternIndex сброшен
            cout << "  Символы не совпали (или конец образца достигнут после совпадения)." << endl;
            if (patternIndex != 0) {
                cout << "  patternIndex != 0. Обновляем patternIndex = lps[" << patternIndex - 1 << "] = " << lps[patternIndex - 1] << endl;
                patternIndex = lps[patternIndex - 1];
            }
            else {
                cout << "  patternIndex == 0. Увеличиваем textIndex." << endl;
                textIndex++;
            }
        }
    }
    
    cout << "--- Конец поиска KMP ---" << endl;
    return occurrences;
}

int main() {
    string pattern, text;

    cout << "Введите образец: ";
    getline(cin, pattern);
    cout << "Введите текст: ";
    getline(cin, text);

    vector<int> occurrences = kmpSearch(text, pattern);

    if (occurrences.empty()) {
        cout << "-1" << endl;
    } 
    else {
        for (size_t resultIndex = 0; resultIndex < occurrences.size(); resultIndex++) {
            cout << occurrences[resultIndex];
            if (resultIndex < occurrences.size() - 1) {
                cout << ",";
            }
        }
        cout << endl;
    }
    
    return 0;
}