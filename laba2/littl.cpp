#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <iomanip>

using namespace std;

const float INF = numeric_limits<float>::max();

struct State {
    vector<vector<float>> costMatrix; // Матрица стоимостей
    vector<pair<int, int>> included; // Список включенных ребер
    vector<pair<int, int>> excluded; // Список исключенных ребер
    float lowerBound; // Нижняя граница стоимости решения
    
    
    State(const vector<vector<float>>& matrix, 
          const vector<pair<int, int>>& inc = {}, 
          const vector<pair<int, int>>& exc = {}, 
          float lb = 0) : costMatrix(matrix), included(inc), excluded(exc), lowerBound(lb) {}
    
    bool operator>(const State& other) const {
        return lowerBound > other.lowerBound;
    }
};

// Функция для вывода матрицы стоимостей
void printMatrix(const vector<vector<float>>& matrix, const string& title) {
    cout << title << ":" << endl;
    for (const auto& row : matrix) {
        for (float val : row) {
            if (val == INF)
                cout << "INF ";
            else
                cout << fixed << setprecision(1) << val << " ";
        }
        cout << endl;
    }
    cout << endl;
}

// Алгоритм ближайшего соседа для поиска начального решения
vector<int> nearestNeighborAlgorithm(const vector<vector<float>>& costMatrix) {
    int n = costMatrix.size();
    int startVertex = 0;
    
    cout << "\n--- Выполнение алгоритма ближайшего соседа (АБС) ---" << endl;
    cout << "Начальная вершина: " << startVertex << endl;
    
    vector<int> path;
    vector<bool> visited(n, false);
    
    int currentVertex = startVertex;
    path.push_back(currentVertex);
    visited[currentVertex] = true;
    
    cout << "Путь: " << currentVertex;
    
    for (int i = 0; i < n - 1; i++) {
        float minDist = INF;
        int nextVertex = -1;

        // Ищем ближайшую непосещенную вершину
        for (int j = 0; j < n; j++) {
            if (!visited[j] && costMatrix[currentVertex][j] < minDist && costMatrix[currentVertex][j] < INF) {
                minDist = costMatrix[currentVertex][j];
                nextVertex = j;
            }
        }

        if (nextVertex == -1) {
            cout << "\nНет доступного пути к следующей вершине" << endl;
            return {};
        }
        
        cout << " -> " << nextVertex << " (стоимость: " << minDist << ")";
        
        currentVertex = nextVertex;
        path.push_back(currentVertex);
        visited[currentVertex] = true;
    }

    // Возвращаемся в начальную вершину
    if (costMatrix[currentVertex][startVertex] == INF) {
        cout << "\nНет пути обратно к начальной вершине" << endl;
        return {};
    }
    
    cout << " -> " << startVertex << " (стоимость: " << costMatrix[currentVertex][startVertex] << ")" << endl;
    cout << "--- Завершение АБС ---\n" << endl;
    
    return path;
}

// Редукция матрицы
float reduceMatrix(vector<vector<float>>& matrix) {
    int n = matrix.size();
    float reduction = 0; // нижняя граница

    // Редукция строк
    for (int i = 0; i < n; i++) {
        float minVal = INF;
        for (int j = 0; j < n; j++) {
            if (matrix[i][j] < minVal) {
                minVal = matrix[i][j];
            }
        }
        
        // Вычитаем минимум из всех элементов строки
        if (minVal > 0 && minVal < INF) {
            for (int j = 0; j < n; j++) {
                if (matrix[i][j] < INF) {
                    matrix[i][j] -= minVal;
                }
            }
            reduction += minVal;
        }
    }

    // Редукция столбцов
    for (int j = 0; j < n; j++) {
        float minVal = INF;
        for (int i = 0; i < n; i++) {
            if (matrix[i][j] < minVal) {
                minVal = matrix[i][j];
            }
        }
        
        // Вычитаем минимум из всех элементов столбца
        if (minVal > 0 && minVal < INF) {
            for (int i = 0; i < n; i++) {
                if (matrix[i][j] < INF) {
                    matrix[i][j] -= minVal;
                }
            }
            reduction += minVal;
        }
    }
    
    return reduction;
}

// Функция для вычисления нижней границы на основе минимального остовного дерева (MST)
float calculateMSTBound(const vector<vector<float>>& matrix) {
    int n = matrix.size();

    if (n <= 2) return 0;
    
    const int excludedVertex = 0; // Исключаем вершину 0 из MST

    vector<bool> visited(n, false);
    vector<float> minEdge(n, INF);
    visited[excludedVertex] = true;

    int startVertex = 1;
    for (int j = 0; j < n; j++) {
        if (j != excludedVertex) {
            minEdge[j] = matrix[startVertex][j];
        }
    }
    visited[startVertex] = true;
    
    float mstWeight = 0;

    for (int i = 0; i < n-2; i++) {
        int nextVertex = -1;
        float minWeight = INF;
        
        for (int j = 0; j < n; j++) {
            if (!visited[j] && minEdge[j] < minWeight) {
                minWeight = minEdge[j];
                nextVertex = j;
            }
        }
        
        if (nextVertex == -1 || minWeight == INF) break;
        
        visited[nextVertex] = true;
        mstWeight += minWeight;

        for (int j = 0; j < n; j++) {
            if (!visited[j] && matrix[nextVertex][j] < minEdge[j]) {
                minEdge[j] = matrix[nextVertex][j];
            }
        }
    }

    float min1 = INF, min2 = INF;
    for (int i = 1; i < n; i++) {
        if (matrix[excludedVertex][i] < min1) {
            min2 = min1;
            min1 = matrix[excludedVertex][i];
        } 
        else if (matrix[excludedVertex][i] < min2) {
            min2 = matrix[excludedVertex][i];
        }
    }
    
    if (min1 < INF) mstWeight += min1;
    if (min2 < INF) mstWeight += min2;
    
    return mstWeight;
}

// Функция для поиска ребра для ветвления (ребро с максимальной суммой минимальных элементов в строке и столбце)
pair<int, int> findBranchingEdge(const vector<vector<float>>& matrix) {
    int n = matrix.size();
    float maxCost = -1;
    pair<int, int> edge = {-1, -1};
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (matrix[i][j] == 0) {
                float rowMin2 = INF, colMin2 = INF;
                
                // Ищем второй минимальный элемент в строке
                for (int k = 0; k < n; k++) {
                    if (k != j && matrix[i][k] < rowMin2)
                        rowMin2 = matrix[i][k];
                    if (k != i && matrix[k][j] < colMin2)
                        colMin2 = matrix[k][j];
                }
                
                if (rowMin2 == INF) rowMin2 = 0;
                if (colMin2 == INF) colMin2 = 0;
                
                float cost = rowMin2 + colMin2;
                if (cost > maxCost) {
                    maxCost = cost;
                    edge = {i, j};
                }
            }
        }
    }
    
    return edge;
}

// Функция для поиска пути в графе с помощью DFS
bool dfs(const vector<vector<int>>& graph, int node, int end, vector<bool>& visited, vector<int>& path) {
    visited[node] = true;
    path.push_back(node);
    
    if (node == end)
        return true;
    
    for (int next : graph[node]) {
        if (!visited[next]) {
            if (dfs(graph, next, end, visited, path))
                return true;
        }
    }
    
    path.pop_back();
    return false;
}

// Функция для поиска пути между двумя вершинами
bool findPath(const vector<pair<int, int>>& edges, int start, int end, vector<int>& path, int n) {
    vector<vector<int>> graph(n);
    for (const auto& edge : edges) {
        graph[edge.first].push_back(edge.second);
    }
    
    path.clear();
    vector<bool> visited(n, false);
    
    return dfs(graph, start, end, visited, path);
}

// Функция для поиска ребра, которое нужно исключить, чтобы избежать подциклов
pair<int, int> findEdgeToExclude(const vector<pair<int, int>>& included, int from, int to, int n) {
    vector<int> path;
    if (findPath(included, to, from, path, n)) {
        if (path.size() >= 2) {
            return {path[path.size()-2], from};
        }
    }
    
    return {to, from};
}

// Алгоритм Литтла для решения задачи коммивояжера
vector<pair<int, int>> littleAlgorithm(vector<vector<float>> costMatrix) {
    int n = costMatrix.size();
    
    cout << "\n--- Выполнение алгоритма Литтла ---" << endl;
    
    // Запрещаем петли (путь из города в самого себя)
    for (int i = 0; i < n; i++) {
        costMatrix[i][i] = INF;
    }
    
    cout << "Начальная матрица стоимости:" << endl;
    printMatrix(costMatrix, "Исходная матрица");
    
    // Выполняем редукцию матрицы
    float matrixReduction = reduceMatrix(costMatrix);
    cout << "Значение редукции матрицы: " << matrixReduction << endl;
    
    printMatrix(costMatrix, "Матрица после редукции");
    
    // Вычисляем нижнюю границу с использованием MST
    float mstBound = calculateMSTBound(costMatrix);
    
    float startLowerBound = matrixReduction + mstBound;
    cout << "Начальная нижняя граница: " << startLowerBound << endl;
    
    // Создаем приоритетную очередь для хранения состояний
    priority_queue<State, vector<State>, greater<State>> pq;
    pq.push(State(costMatrix, {}, {}, startLowerBound));
    
    int iterations = 0;
    const int MAX_ITERATIONS_TO_SHOW = 3;

    while (!pq.empty()) {
        State current = pq.top();
        pq.pop();
        
        iterations++;
        bool showDetails = (iterations <= MAX_ITERATIONS_TO_SHOW);
        
        if (showDetails) {
            cout << "\nИтерация " << iterations << ":" << endl;
            cout << "Текущая нижняя граница: " << current.lowerBound << endl;
            cout << "Включенные ребра: ";
            for (const auto& edge : current.included) {
                cout << "(" << edge.first << "," << edge.second << ") ";
            }
            cout << endl;
        }
        
        // Если включили все ребра, то нашли решение
        if (current.included.size() == n) {
            cout << "Найдено полное решение!" << endl;
            cout << "Итоговые ребра: ";
            for (const auto& edge : current.included) {
                cout << "(" << edge.first << "," << edge.second << ") ";
            }
            cout << "\n--- Завершение алгоритма Литтла ---\n" << endl;
            return current.included;
        }
        
        // Выбираем ребро для ветвления
        pair<int, int> edge = findBranchingEdge(current.costMatrix);
        int i = edge.first, j = edge.second;
        
        if (i == -1 || j == -1) {
            if (showDetails) cout << "Не найдено ребро для ветвления, пропускаем" << endl;
            continue;
        }
        
        if (showDetails) cout << "Выбрано ребро для ветвления: (" << i << "," << j << ")" << endl;
        
        // Включаем ребро (i,j)
        vector<vector<float>> includeMatrix = current.costMatrix;
        vector<pair<int, int>> newIncluded = current.included;
        newIncluded.push_back({i, j});
        
        if (showDetails) cout << "Ветвь 1: Включаем ребро (" << i << "," << j << ")" << endl;
        
        // Исключаем ребро, которое может образовать подцикл
        pair<int, int> excludeEdge = findEdgeToExclude(current.included, i, j, n);
        
        // Запрещаем образование подциклов и использование вершины повторно
        for (int k = 0; k < n; k++) {
            includeMatrix[i][k] = INF;
            includeMatrix[k][j] = INF;
        }
        includeMatrix[j][i] = INF;
        includeMatrix[excludeEdge.first][excludeEdge.second] = INF;
        
        // Вычисляем нижнюю границу для ветви с включенным ребром
        float includeReduction = reduceMatrix(includeMatrix);
        float includeMST = calculateMSTBound(includeMatrix);
        float includeLowerBound = current.lowerBound + includeReduction + includeMST;
        
        if (showDetails) {
            cout << "  Дополнительная редукция: " << includeReduction << endl;
            cout << "  Оценка МОД: " << includeMST << endl;
            cout << "  Новая нижняя граница: " << includeLowerBound << endl;
        }
        
        pq.push(State(includeMatrix, newIncluded, current.excluded, includeLowerBound));
        
        // Исключаем ребро (i,j)
        vector<vector<float>> excludeMatrix = current.costMatrix;
        excludeMatrix[i][j] = INF;
        
        if (showDetails) cout << "Ветвь 2: Исключаем ребро (" << i << "," << j << ")" << endl;
        
        // Вычисляем нижнюю границу для ветви с исключенным ребром
        float excludeReduction = reduceMatrix(excludeMatrix);
        float excludeMST = calculateMSTBound(excludeMatrix);
        float excludeLowerBound = current.lowerBound + excludeReduction + excludeMST;
        
        if (showDetails) {
            cout << "  Дополнительная редукция: " << excludeReduction << endl;
            cout << "  Оценка МОД: " << excludeMST << endl;
            cout << "  Новая нижняя граница: " << excludeLowerBound << endl;
        }
        
        vector<pair<int, int>> newExcluded = current.excluded;
        newExcluded.push_back({i, j});
        
        pq.push(State(excludeMatrix, current.included, newExcluded, excludeLowerBound));
    }
    
    cout << "Решение не найдено\n" << endl;
    return {};
}

// Функция для преобразования списка ребер в путь
vector<int> edgesToPath(const vector<pair<int, int>>& edges, int n) {
    vector<int> path;
    if (edges.empty()) return path;
    
    vector<int> next(n, -1);
    for (const auto& edge : edges) {
        next[edge.first] = edge.second;
    }
    
    int city = 0;
    for (int i = 0; i < n; i++) {
        path.push_back(city);
        city = next[city];
    }
    
    return path;
}

// Функция для вычисления общей стоимости пути
float calculateTotalCost(const vector<int>& path, const vector<vector<float>>& originalCostMatrix) {
    float totalCost = 0;
    int n = path.size();
    
    for (int i = 0; i < n; i++) {
        int from = path[i];
        int to = path[(i + 1) % n];
        totalCost += originalCostMatrix[from][to];
    }
    
    return totalCost;
}

int main() {
    int n;
    cout << "Введите количество городов: ";
    cin >> n;
    
    cout << "Введите матрицу стоимостей (используйте -1 для отсутствия пути):" << endl;
    vector<vector<float>> costMatrix(n, vector<float>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cin >> costMatrix[i][j];
            if (costMatrix[i][j] == -1) {
                costMatrix[i][j] = INF;
            }
        }
    }
    
    vector<vector<float>> originalCostMatrix = costMatrix;

    vector<int> absPath = nearestNeighborAlgorithm(originalCostMatrix);

    vector<pair<int, int>> littleSolution = littleAlgorithm(costMatrix);
    vector<int> littlePath = edgesToPath(littleSolution, n);
    
    float absCost = 0;
    float littleCost = 0;
    
    cout << "\n--- Результаты ---" << endl;
    
    cout << "Алгоритм ближайшего соседа (АБС):" << endl;
    if (!absPath.empty()) {
        cout << "Путь: ";
        for (int i = 0; i < absPath.size(); i++) {
            cout << absPath[i];
            if (i < absPath.size() - 1) cout << " -> ";
        }
        cout << " -> " << absPath[0] << endl;
        
        absCost = calculateTotalCost(absPath, originalCostMatrix);
        cout << "Общая стоимость: " << fixed << setprecision(1) << absCost << endl;
    } 
    else {
        cout << "Маршрут не найден" << endl;
    }
    
    cout << "\nАлгоритм Литтла:" << endl;
    if (!littlePath.empty()) {
        cout << "Путь: ";
        for (int i = 0; i < littlePath.size(); i++) {
            cout << littlePath[i];
            if (i < littlePath.size() - 1) cout << " -> ";
        }
        cout << " -> " << littlePath[0] << endl;
        
        littleCost = calculateTotalCost(littlePath, originalCostMatrix);
        cout << "Общая стоимость: " << fixed << setprecision(1) << littleCost << endl;
    } 
    else {
        cout << "Маршрут не найден" << endl;
    }
    
    // Если оба алгоритма нашли решение, выводим сравнение
    if (!absPath.empty() && !littlePath.empty()) {
        float ratio = absCost / littleCost;
        cout << "\nОтношение АБС/Литтл: " << fixed << setprecision(2) << ratio << endl;
    }
    
    return 0;
}