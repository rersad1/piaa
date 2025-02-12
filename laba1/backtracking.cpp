#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <fstream>

using namespace std;

int divisior;

struct Square {
    int xCoord, yCoord, width;
};

pair<int,int> findFree(const vector<vector<bool>> &table) { // поиск свободной клетки
    for (int i = 0; i < (int)table.size(); i++) {
        for (int j = 0; j < (int)table.size(); j++) {
            if (!table[i][j]) {
                return {i, j};
            }
        }
    }
    return {-1, -1};
}

void changeState(vector<vector<bool>> &table, const Square &square, bool state) { // изменение состояния клеток
    for (int i = square.xCoord; i < square.xCoord + square.width; i++) {
        for (int j = square.yCoord; j < square.yCoord + square.width; j++) {
            table[i][j] = state;
        }
    }
}

void setStartSquares(vector<vector<bool>> &table, vector<Square> &squares, int divisior) { // установка начальных квадратов
    int halfWidth = (divisior + 1) / 2;
    Square topLeft = {0, 0, halfWidth};
    Square topRight = {0, halfWidth, divisior - halfWidth};
    Square bottomLeft = {halfWidth, 0, divisior - halfWidth};

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
            return min(min(width, (divisior + 1) / 2), divisior - topLeft.first);
        }
        ++width;
    }
    return min(min(width, (divisior + 1) / 2), divisior - topLeft.first);
}


void backtracking(vector<vector<bool>> &table, vector<Square> &tempSolution, vector<Square> &bestSolution, int &minLength, int currentLength) {
    if (currentLength >= minLength) { // если текущая длина больше минимальной, то выход
        return;
    }

    auto cell = findFree(table);
    if (cell.first == -1) { // если не нашли свободной клетки
        if (currentLength < minLength) {
            minLength = currentLength;
            bestSolution = tempSolution;
        }
        return;
    }

    int maxSide = calculateMaxWidth(cell, table);
    for (int side = maxSide; side >= 1; side--) {
        Square square = {cell.first, cell.second, side};
        changeState(table, square, true);
        tempSolution.push_back({cell.first, cell.second, side});

        backtracking(table, tempSolution, bestSolution, minLength, currentLength + 1);

        changeState(table, square, false);
        tempSolution.pop_back();
    }
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

    for (int N = 2; N <= 20; N++) {
        auto start = chrono::high_resolution_clock::now();
        
        divisior = N;
        for (int i = 2; i <= N; i++) {
            if (N % i == 0) {
                divisior = i;
                break;
            }
        }
        vector<vector<bool>> table(divisior, vector<bool>(divisior, false));
        int minLength = 1e9;
        vector<Square> tempSolution;
        vector<Square> bestSolution;

        setStartSquares(table, tempSolution, divisior);
        backtracking(table, tempSolution, bestSolution, minLength, 3);
        
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
        
        writeTimeToFile(N, duration);

        cout << "N = " << N << ":\n";
        cout << bestSolution.size() << endl;
        for (auto &set : bestSolution) {
            cout << set.xCoord * N / divisior + 1 << " " 
                 << set.yCoord * N / divisior + 1 << " " 
                 << set.width * N / divisior << endl;
        }
        cout << endl;
    }
    return 0;
}