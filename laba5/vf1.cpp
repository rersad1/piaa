#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    int replaceCost, insertCost, deleteCost;
    cin >> replaceCost >> insertCost >> deleteCost;
    
    string A, B;
    cin >> A >> B;
    
    int n = A.length();
    int m = B.length();

    cout << "Строка A: " << A << endl;
    cout << "Строка B: " << B << endl;
    cout << "Стоимости: замена=" << replaceCost << ", вставка=" << insertCost << ", удаление=" << deleteCost << endl;
    cout << endl;

    vector<vector<int>> editDistance(n + 1, vector<int>(m + 1, 0));

    // Инициализация первой строки
    for (int j = 1; j <= m; j++) {
        editDistance[0][j] = editDistance[0][j-1] + insertCost;
    }

    // Инициализация первого столбца
    for (int i = 1; i <= n; i++) {
        editDistance[i][0] = editDistance[i-1][0] + deleteCost;
    }

    cout << "Начальная таблица расстояний:" << endl;
    cout << "    ";
    for (int j = 0; j <= m; j++) {
        if (j == 0) cout << "  ";
        else cout << B[j-1] << " ";
    }
    cout << endl;
    
    for (int i = 0; i <= n; i++) {
        if (i == 0) cout << "  ";
        else cout << A[i-1] << " ";
        for (int j = 0; j <= m; j++) {
            cout << editDistance[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (A[i-1] == B[j-1]) {
                editDistance[i][j] = editDistance[i-1][j-1];
                cout << "Символы '" << A[i-1] << "' совпадают, берем диагональ: " << editDistance[i][j] << endl;
            } 
            else {
                int replace = editDistance[i-1][j-1] + replaceCost;
                int insert = editDistance[i][j-1] + insertCost;
                int delete_op = editDistance[i-1][j] + deleteCost;
                
                editDistance[i][j] = min({replace, insert, delete_op});
                
                cout << "Символы '" << A[i-1] << "' и '" << B[j-1] << "' не совпадают:" << endl;
                cout << "  Замена: " << replace << ", Вставка: " << insert << ", Удаление: " << delete_op << endl;
                cout << "  Минимум: " << editDistance[i][j] << endl;
            }
        }
    }
    
    cout << endl << "Итоговая таблица расстояний:" << endl;
    cout << "    ";
    for (int j = 0; j <= m; j++) {
        if (j == 0) cout << "  ";
        else cout << B[j-1] << " ";
    }
    cout << endl;
    
    for (int i = 0; i <= n; i++) {
        if (i == 0) cout << "  ";
        else cout << A[i-1] << " ";
        for (int j = 0; j <= m; j++) {
            cout << editDistance[i][j] << " ";
        }
        cout << endl;
    }
    
    cout << endl << "Минимальное расстояние редактирования: " << editDistance[n][m] << endl;

    return 0;
}