#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <fstream>

using namespace std;

int divisior;
bool logs = true;

struct Square {
    int xCoord, yCoord, width;
};

void printGrid(const vector<vector<bool>> &table) {
    if (!logs) return;
    
    cout << "Текущее состояние сетки:" << endl;
    for (int i = 0; i < (int)table.size(); i++) {
        for (int j = 0; j < (int)table.size(); j++) {
            cout << (table[i][j] ? "X " : ". ");
        }
        cout << endl;
    }
    cout << endl;
}

// Вывод информации о квадрате
void printSquare(const Square &square) {
    if (!logs) return;
    
    cout << "Квадрат: (" << square.xCoord << "," << square.yCoord 
         << ") с шириной " << square.width << endl;
}

pair<int,int> findFree(const vector<vector<bool>> &table) { // поиск свободной клетки
    for (int i = 0; i < (int)table.size(); i++) {
        for (int j = 0; j < (int)table.size(); j++) {
            if (!table[i][j]) {
                if (logs) cout << "Найдена свободная клетка в (" << i << "," << j << ")" << endl;
                return {i, j};
            }
        }
    }
    if (logs) cout << "Свободных клеток не найдено!" << endl;
    return {-1, -1};
}

void changeState(vector<vector<bool>> &table, const Square &square, bool state) { // изменение состояния клеток
    if (logs) {
        cout << (state ? "Размещение" : "Удаление") << " квадрата в (" << square.xCoord << "," << square.yCoord << ") с шириной " << square.width << endl;
    }
    
    for (int i = square.xCoord; i < square.xCoord + square.width; i++) {
        for (int j = square.yCoord; j < square.yCoord + square.width; j++) {
            table[i][j] = state;
        }
    }
    
    if (logs) printGrid(table);
}

void setStartSquares(vector<vector<bool>> &table, vector<Square> &squares, int divisior) { // установка начальных квадратов
    if (logs) cout << "\n=== Установка начальных квадратов ===" << endl;
    
    int halfWidth = (divisior + 1) / 2;
    Square topLeft = {0, 0, halfWidth};
    Square topRight = {0, halfWidth, divisior - halfWidth};
    Square bottomLeft = {halfWidth, 0, divisior - halfWidth};

    if (logs) {
        cout << "Начальные квадраты:" << endl;
        printSquare(topLeft);
        printSquare(topRight);
        printSquare(bottomLeft);
    }

    changeState(table, topLeft, true);
    changeState(table, topRight, true);
    changeState(table, bottomLeft, true);

    squares.push_back(topLeft);
    squares.push_back(topRight);
    squares.push_back(bottomLeft);
}   

int calculateMaxWidth(pair<int, int>& topLeft, vector<vector<bool>>& grid) { // вычисление максимальной ширины квадрата
    int width = 0;
    for (int j = topLeft.second; j < divisior; ++j) {
        if (grid[topLeft.first][j]) {
            int result = min(min(width, (divisior + 1) / 2), divisior - topLeft.first);
            if (logs) cout << "Максимальная ширина из (" << topLeft.first << "," << topLeft.second << ") равна " << result << endl;
            return result;
        }
        ++width;
    }
    int result = min(min(width, (divisior + 1) / 2), divisior - topLeft.first);
    if (logs) cout << "Максимальная ширина из (" << topLeft.first << "," << topLeft.second << ") равна " << result << endl;
    return result;
}

void backtracking(vector<vector<bool>> &table, vector<Square> &tempSolution, vector<Square> &bestSolution, int &minLength, int currentLength, int depth = 0) {
    string indent(depth * 2, ' '); 
    
    if (logs) {
        cout << indent << "Поиск с возвратом на глубине " << depth 
             << " (текущая длина: " << currentLength << ", лучшая: " << minLength << ")" << endl;
    }
    
    if (currentLength >= minLength) { // если текущая длина больше минимальной, то выход
        if (logs) cout << indent << "Отсечение: текущая длина >= минимальной длины" << endl;
        return;
    }

    auto cell = findFree(table);
    if (cell.first == -1) { // если не нашли свободной клетки
        if (currentLength < minLength) {
            if (logs) {
                cout << indent << "Найдено новое лучшее решение! Длина: " << currentLength << endl;
                cout << indent << "Квадраты в решении:" << endl;
                for (const auto& square : tempSolution) {
                    cout << indent << "  (" << square.xCoord << "," << square.yCoord 
                         << ") ширина " << square.width << endl;
                }
            }
            minLength = currentLength;
            bestSolution = tempSolution;
        }
        return;
    }

    int maxSide = calculateMaxWidth(cell, table);
    if (logs) cout << indent << "Пробуем квадраты в (" << cell.first << "," << cell.second << ") с шириной от " << maxSide << " до 1" << endl;
    
    for (int side = maxSide; side >= 1; side--) {
        if (logs) cout << indent << "Пробуем ширину " << side << endl;
        
        Square square = {cell.first, cell.second, side};
        changeState(table, square, true);
        tempSolution.push_back({cell.first, cell.second, side});

        backtracking(table, tempSolution, bestSolution, minLength, currentLength + 1, depth + 1);

        if (logs) cout << indent << "Возврат: удаляем квадрат с шириной " << side << endl;
        changeState(table, square, false);
        tempSolution.pop_back();
    }
    
    if (logs) cout << indent << "Закончили перебор всех возможностей на глубине " << depth << endl;
}

void writeTimeToFile(int n, double duration) { // запись времени выполнения в файл
    ofstream file;
    file.open("execution_times.txt", ios::app);
    if (file.is_open()) {
        file << "N = " << n << ": " << fixed << setprecision(6) 
             << duration / 1000000.0 << " ms\n";
        file.close();
    }
}

int main() {
    ofstream file("execution_times.txt", ios::trunc);
    file.close();

    cout << "Выводить подробную информацию? (1 - да, 0 - нет): ";
    cin >> logs;

    int startN = 2;
    int endN = 20;
    
    cout << "Введите начальное N (≥ 2): ";
    cin >> startN;
    cout << "Введите конечное N (≤ 20): ";
    cin >> endN;
    
    startN = max(2, startN);
    endN = min(20, endN);

    for (int N = startN; N <= endN; N++) {
        cout << "\n========= Обработка N = " << N << " =========\n" << endl;
        
        auto start = chrono::high_resolution_clock::now();
        
        divisior = N;
        for (int i = 2; i <= N; i++) {
            if (N % i == 0) {
                divisior = i;
                break;
            }
        }
        
        if (logs) cout << "Используем делитель: " << divisior << endl;
        
        vector<vector<bool>> table(divisior, vector<bool>(divisior, false));
        int minLength = 1e9;
        vector<Square> tempSolution;
        vector<Square> bestSolution;

        setStartSquares(table, tempSolution, divisior);
        backtracking(table, tempSolution, bestSolution, minLength, 3);
        
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
        
        writeTimeToFile(N, duration);

        cout << "\n----- Окончательный результат для N = " << N << " -----" << endl;
        cout << "Количество квадратов: " << bestSolution.size() << endl;
        for (auto &set : bestSolution) {
            cout << set.xCoord * N / divisior + 1 << " " 
                 << set.yCoord * N / divisior + 1 << " " 
                 << set.width * N / divisior << endl;
        }
        cout << endl;
    }
    return 0;
}