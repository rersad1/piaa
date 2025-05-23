#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

void printTable(const vector<vector<int>>& editDistance, const string& S, const string& T, const string& title) {
    int n = S.length();
    int m = T.length();
    
    cout << title << endl;
    cout << "    ";
    for (int j = 0; j <= m; j++) {
        if (j == 0) cout << "  ";
        else cout << T[j-1] << " ";
    }
    cout << endl;
    
    for (int i = 0; i <= n; i++) {
        if (i == 0) cout << "  ";
        else cout << S[i-1] << " ";
        for (int j = 0; j <= m; j++) {
            cout << editDistance[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void fillLevenshteinTable(vector<vector<int>>& editDistance, const string& S, const string& T) {
    int n = S.length();
    int m = T.length();
    
    // Инициализация первой строки и столбца
    for (int j = 1; j <= m; j++) {
        editDistance[0][j] = j;
    }
    for (int i = 1; i <= n; i++) {
        editDistance[i][0] = i;
    }
    
    cout << "Заполнение таблицы расстояния Левенштейна:" << endl;
    
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (S[i-1] == T[j-1]) {
                editDistance[i][j] = editDistance[i-1][j-1];
                cout << "Символы '" << S[i-1] << "' совпадают, берем диагональ: " << editDistance[i][j] << endl;
            } 
            else {
                int replace = editDistance[i-1][j-1] + 1;  // замена
                int insert = editDistance[i][j-1] + 1;     // вставка
                int delete_op = editDistance[i-1][j] + 1;  // удаление
                
                editDistance[i][j] = min({replace, insert, delete_op});
                
                cout << "Символы '" << S[i-1] << "' и '" << T[j-1] << "' не совпадают:" << endl;
                cout << "  Замена: " << replace << ", Вставка: " << insert << ", Удаление: " << delete_op << endl;
                cout << "  Минимум: " << editDistance[i][j] << endl;
            }
        }
    }
    cout << endl;
}

void fillSubstringTable(vector<vector<int>>& editDistance, const string& S, const string& T, int& maxLength, int& endingPosS, int& endingPosT) {
    int n = S.length();
    int m = T.length();
    
    cout << "Заполнение таблицы для поиска общей подстроки:" << endl;
    
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (S[i-1] == T[j-1]) {
                editDistance[i][j] = editDistance[i-1][j-1] + 1;
                cout << "Символы '" << S[i-1] << "' совпадают, editDistance[" << i << "][" << j << "] = " << editDistance[i][j] << endl;
                
                // Обновляем максимум и позицию
                if (editDistance[i][j] > maxLength) {
                    maxLength = editDistance[i][j];
                    endingPosS = i;
                    endingPosT = j;
                    cout << "  Новый максимум: " << maxLength << " на позиции (" << i << "," << j << ")" << endl;
                }
            } 
            else {
                editDistance[i][j] = 0;
                cout << "Символы '" << S[i-1] << "' и '" << T[j-1] << "' не совпадают, editDistance[" << i << "][" << j << "] = 0" << endl;
            }
        }
    }
    cout << endl;
}

void printSubstringResult(const string& S, const string& T, int maxLength, int endingPosS, int endingPosT) {
    cout << "Результат поиска наибольшей общей подстроки:" << endl;
    cout << "Длина наибольшей общей подстроки: " << maxLength << endl;
    
    if (maxLength > 0) {
        string commonSubstring = S.substr(endingPosS - maxLength, maxLength);
        cout << "Наибольшая общая подстрока: \"" << commonSubstring << "\"" << endl;
        cout << "Позиция в строке S: " << (endingPosS - maxLength) << "-" << (endingPosS - 1) << endl;
        cout << "Позиция в строке T: " << (endingPosT - maxLength) << "-" << (endingPosT - 1) << endl;
    } 
    else {
        cout << "Общих подстрок не найдено" << endl;
    }
    cout << endl;
}

int main() {
    string S, T;
    cin >> S >> T;
    
    int n = S.length();
    int m = T.length();

    cout << "Строка S: " << S << endl;
    cout << "Строка T: " << T << endl;
    cout << endl;

    cout << "========== РАССТОЯНИЕ ЛЕВЕНШТЕЙНА ==========" << endl;
    vector<vector<int>> editDistance(n + 1, vector<int>(m + 1, 0));
    
    // Инициализация для Левенштейна
    for (int j = 0; j <= m; j++) editDistance[0][j] = j;
    for (int i = 0; i <= n; i++) editDistance[i][0] = i;

    printTable(editDistance, S, T, "Начальная таблица для расстояния Левенштейна:");
    fillLevenshteinTable(editDistance, S, T);
    printTable(editDistance, S, T, "Итоговая таблица расстояния Левенштейна:");
    cout << "Расстояние редактирования (Левенштейна): " << editDistance[n][m] << endl;
    cout << endl;

    cout << "========== НАИБОЛЬШАЯ ОБЩАЯ ПОДСТРОКА ==========" << endl;
    vector<vector<int>> substringeditDistance(n + 1, vector<int>(m + 1, 0));
    
    int maxLength = 0;
    int endingPosS = 0;
    int endingPosT = 0;

    printTable(substringeditDistance, S, T, "Начальная таблица для поиска подстроки:");
    fillSubstringTable(substringeditDistance, S, T, maxLength, endingPosS, endingPosT);
    printTable(substringeditDistance, S, T, "Итоговая таблица поиска подстроки:");
    printSubstringResult(S, T, maxLength, endingPosS, endingPosT);

    return 0;
}