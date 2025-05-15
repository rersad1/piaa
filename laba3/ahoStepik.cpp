#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <algorithm>
#include <set>

using namespace std;

// Структура узла Trie для автомата Ахо-Корасика
struct TrieNode {
    map<char, int> children; // Переходы по символам
    int fail;                // Суффиксная ссылка (failure link)
    vector<pair<int, int>> output; // Список (индекс подстроки, длина) для терминальных узлов
    bool is_terminal;        // Является ли узел терминальным (конец подстроки)
    int output_link;         // Выходная ссылка (output link)
    TrieNode() : fail(0), is_terminal(false), output_link(0) {}
};

vector<TrieNode> nodes; // Все узлы Trie
vector<int> fail_depth_memo;    // Сохранение глубины суффиксных ссылок
vector<int> output_depth_memo;  // Сохранение глубины выходных ссылок

// Рекурсивно вычисляет глубину суффиксной ссылки для узла
int get_fail_depth(int node_idx) {
    if (node_idx == 0) return 0;
    if (fail_depth_memo[node_idx] != -1) return fail_depth_memo[node_idx];
    fail_depth_memo[node_idx] = 1 + get_fail_depth(nodes[node_idx].fail);
    return fail_depth_memo[node_idx];
}

// Рекурсивно вычисляет глубину выходной ссылки для узла
int get_output_depth(int node_idx) {
    if (node_idx == 0 || nodes[node_idx].output_link == 0) return 0;
    if (output_depth_memo[node_idx] != -1) return output_depth_memo[node_idx];
    output_depth_memo[node_idx] = 1 + get_output_depth(nodes[node_idx].output_link);
    return output_depth_memo[node_idx];
}

// Вычисляет максимальные глубины суффиксных и выходных ссылок во всём Trie
pair<int, int> calculate_max_depths() {
    int max_fail_depth = 0, max_output_depth = 0;
    int num_nodes = nodes.size();
    if (num_nodes == 0) return {0,0};

    fail_depth_memo.assign(num_nodes, -1);
    output_depth_memo.assign(num_nodes, -1);

    for (int i = 1; i < num_nodes; ++i) {
        max_fail_depth = max(max_fail_depth, get_fail_depth(i));
        max_output_depth = max(max_output_depth, get_output_depth(i));
    }
    return {max_fail_depth, max_output_depth};
}

// Разбивает шаблон на подстроки между джокерами, возвращает пары (подстрока, смещение)
vector<pair<string, int>> split_pattern(const string& pattern, char wildcard) {
    cout << "Разбиение шаблона \"" << pattern << "\" с джокером '" << wildcard << "'\n";
    vector<pair<string, int>> subpatterns_with_offsets;
    string current_subpattern;
    int start_offset = -1;
    for (int i = 0; i < pattern.length(); ++i) {
        if (pattern[i] == wildcard) {
            if (!current_subpattern.empty()) {
                cout << "Найдена подстрока: \"" << current_subpattern << "\" со смещением " << start_offset << "\n";
                subpatterns_with_offsets.push_back({current_subpattern, start_offset});
                current_subpattern = "";
                start_offset = -1;
            }
        }
        else {
            if (current_subpattern.empty()) {
                start_offset = i;
            }
            current_subpattern += pattern[i];
        }
    }
    if (!current_subpattern.empty()) {
        cout << "Найдена подстрока: \"" << current_subpattern << "\" со смещением " << start_offset << "\n";
        subpatterns_with_offsets.push_back({current_subpattern, start_offset});
    }
    cout << "Разбиение завершено.\n\n";
    return subpatterns_with_offsets;
}

// Добавляет подстроку в Trie, отмечая терминальный узел и его индекс
void add_string(const string& subpattern, int subpattern_index) {
    cout << "Добавляю подстроку " << subpattern_index << " (0-based): \"" << subpattern << "\"\n";
    int current_node_idx = 0;
    int subpattern_len = subpattern.length();
    for (char ch : subpattern) {
        if (nodes[current_node_idx].children.find(ch) == nodes[current_node_idx].children.end()) {
            int nxt_node_idx = nodes.size();
            cout << "  Создаю узел " << nxt_node_idx << " для символа '" << ch
                 << "' от узла " << current_node_idx << "\n";
            nodes[current_node_idx].children[ch] = nxt_node_idx;
            nodes.emplace_back();
        }
        current_node_idx = nodes[current_node_idx].children[ch];
        cout << "  Перешёл в узел " << current_node_idx << " после символа '" << ch << "'\n";
    }
    nodes[current_node_idx].is_terminal = true;
    nodes[current_node_idx].output.emplace_back(subpattern_index, subpattern.length());
    cout << "Подстрока " << subpattern_index << " завершена в узле " << current_node_idx << "\n\n";
}

// Строит суффиксные и выходные ссылки для всех узлов Trie (BFS)
void build_failure_links() {
    cout << "Построение суффиксных и выходных ссылок...\n";
    queue<int> q;
    // Для детей корня суффиксная и выходная ссылки указывают на корень
    for (const auto& pair : nodes[0].children) {
        int child_node_idx = pair.second;
        q.push(child_node_idx);
        nodes[child_node_idx].fail = 0;
        nodes[child_node_idx].output_link = 0;
         cout << "  Узел " << child_node_idx << " (ребенок корня): ссылка на суффикс -> 0" << ", ссылка на выходной -> 0\n";
    }

    // BFS по Trie
    while (!q.empty()) {
        int current_node_idx = q.front();
        q.pop();

        for (const auto& pair : nodes[current_node_idx].children) {
            char key = pair.first;
            int next_node_idx = pair.second;
            q.push(next_node_idx);

            // Ищем суффиксную ссылку для текущего перехода
            int fail_node_idx = nodes[current_node_idx].fail;
            while (fail_node_idx != 0 && nodes[fail_node_idx].children.find(key) == nodes[fail_node_idx].children.end()) {
                fail_node_idx = nodes[fail_node_idx].fail;
            }

            if (nodes[fail_node_idx].children.count(key)) {
                nodes[next_node_idx].fail = nodes[fail_node_idx].children.at(key);
            }
            else {
                nodes[next_node_idx].fail = 0;
            }

            // Выходная ссылка: если fail-узел терминальный, иначе копируем его выходную ссылку
            int fn = nodes[next_node_idx].fail;
            if (nodes[fn].is_terminal) {
                nodes[next_node_idx].output_link = fn;
            }
            else {
                nodes[next_node_idx].output_link = nodes[fn].output_link;
            }
            cout << "  Узел " << next_node_idx << ": ссылка на суффикс -> " << nodes[next_node_idx].fail << ", ссылка на выходной -> " << nodes[next_node_idx].output_link << "\n";
        }
    }
    cout << "Ссылки построены.\n\n";
}

// Поиск всех вхождений подстрок (без учёта джокеров) в тексте
vector<pair<int, int>> search_text(const string& text) {
    cout << "Поиск подстрок в тексте: \"" << text << "\"\n";
    vector<pair<int, int>> sub_matches;
    int current_node_idx = 0;

    for (int i = 0; i < text.length(); ++i) {
        char ch = text[i];
        cout << "Текст поз. " << i << " (1-based: " << i+1 << ") символ '" << ch << "', текущий узел = " << current_node_idx;

        // Переход по суффиксным ссылкам, если нет перехода по символу
        while (current_node_idx != 0 && nodes[current_node_idx].children.find(ch) == nodes[current_node_idx].children.end()) {
            current_node_idx = nodes[current_node_idx].fail;
            cout << " --(fail)--> " << current_node_idx;
        }

        // Если есть переход по символу, переходим
        if (nodes[current_node_idx].children.count(ch)) {
            current_node_idx = nodes[current_node_idx].children.at(ch);
            cout << " --'" << ch << "'--> " << current_node_idx;
        }
        cout << "\n";

        // Проверяем терминальные узлы по цепочке fail-ссылок
        int temp_node_idx = current_node_idx;
        while (temp_node_idx != 0) {
            if (nodes[temp_node_idx].is_terminal) {
                for (const auto& match_info : nodes[temp_node_idx].output) {
                    int subpattern_original_idx = match_info.first;
                    int subpattern_len = match_info.second;
                    int start_pos_0_based = i - subpattern_len + 1;
                    cout << "  Найдено совпадение подстроки: индекс " << subpattern_original_idx << " (длина " << subpattern_len << ")" << " в тексте с позиции " << start_pos_0_based << " (1-based: " << start_pos_0_based + 1 << ")\n";
                    sub_matches.emplace_back(start_pos_0_based, subpattern_original_idx);
                }
            }
            temp_node_idx = nodes[temp_node_idx].fail;
        }
    }
    cout << "\nПоиск подстрок завершён.\n\n";
    return sub_matches;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    string text;
    cin >> text;
    cout << "Входной текст: \"" << text << "\"\n";
    string pattern;
    cin >> pattern;
    cout << "Входной шаблон: \"" << pattern << "\"\n";
    char wildcard_char;
    cin >> wildcard_char;
    cout << "Символ джокера: '" << wildcard_char << "'\n\n";

    nodes.emplace_back(); // Корень Trie

    // Разбиваем шаблон на подстроки между джокерами
    vector<pair<string, int>> subpatterns_with_offsets = split_pattern(pattern, wildcard_char);
    int num_subpatterns = subpatterns_with_offsets.size();
    int pattern_len = pattern.length();
    int text_len = text.length();

    // Обработка случая, когда шаблон состоит только из джокеров
    if (num_subpatterns == 0 && pattern_len > 0) {
         bool all_wildcards = true;
         for(char c_pat : pattern) {
             if (c_pat != wildcard_char) {
                 all_wildcards = false;
                 break;
             }
         }
         if (all_wildcards) {
            cout << "Шаблон состоит только из джокеров.\n";
            if (pattern_len <= text_len) {
                cout << "Найденные позиции (1-based):\n";
                for (int i = 0; i <= text_len - pattern_len; ++i) {
                    cout << i + 1 << "\n";
                }
            } else {
                cout << "Шаблон длиннее текста, совпадений нет.\n";
            }
             // Выводим глубины для пустого Trie
            auto depths_wc = calculate_max_depths();
            cout << "\nМаксимальная глубина суффиксных ссылок: " << depths_wc.first << "\n";
            cout << "Максимальная глубина выходных ссылок: " << depths_wc.second << "\n\n";
            return 0;
         }
    }

    // Если нет ни одной подстроки для поиска
    if (num_subpatterns == 0 && !(pattern_len > 0 && all_of(pattern.begin(), pattern.end(), [&](char c_pat){ return c_pat == wildcard_char; }))) {
        cout << "Не найдено непустых подстрок для построения автомата (и это не случай 'все джокеры').\n";
        auto depths_empty = calculate_max_depths();
        cout << "\nМаксимальная глубина суффиксных ссылок: " << depths_empty.first << "\n";
        cout << "Максимальная глубина выходных ссылок: " << depths_empty.second << "\n\n";
        cout << "Итоговые совпадения (1-based):\n";
        return 0;
    }

    // Добавляем все подстроки в Trie
    for (int i = 0; i < num_subpatterns; ++i) {
        if (!subpatterns_with_offsets[i].first.empty()) {
            add_string(subpatterns_with_offsets[i].first, i);
        }
    }

    // Строим суффиксные и выходные ссылки
    build_failure_links();

    // Выводим максимальные глубины ссылок
    auto depths = calculate_max_depths();
    cout << "Максимальная глубина суффиксных ссылок: " << depths.first << "\n";
    cout << "Максимальная глубина выходных ссылок: " << depths.second << "\n\n";

    // Ищем все вхождения подстрок в тексте
    vector<pair<int, int>> sub_matches = search_text(text);

    // Для каждого совпадения подстроки вычисляем потенциальное начало полного шаблона
    map<int, vector<int>> potential_starts;
    cout << "Обработка найденных подстрок для сборки полного шаблона...\n";
    for (const auto& match : sub_matches) {
        int start_pos_sub_0_based = match.first;
        int sub_idx_0_based = match.second;
        int offset_in_pattern = subpatterns_with_offsets[sub_idx_0_based].second;
        int potential_full_start_0_based = start_pos_sub_0_based - offset_in_pattern;
        cout << "Подстрока " << sub_idx_0_based << " (\"" << subpatterns_with_offsets[sub_idx_0_based].first<< "\") найдена в тексте с поз. " << start_pos_sub_0_based
            << ". Смещение в шаблоне: " << offset_in_pattern
            << ". Потенциальное начало полного шаблона: " << potential_full_start_0_based << "\n";

        // Проверяем, не выходит ли шаблон за границы текста
        if (potential_full_start_0_based >= 0 && potential_full_start_0_based + pattern_len <= text_len) {
            potential_starts[potential_full_start_0_based].push_back(sub_idx_0_based);
            cout << "Добавлено к потенциальным стартам для поз. " << potential_full_start_0_based << "\n";
        } 
        else {
            cout << "Недопустимое потенциальное начало (выходит за границы текста).\n";
        }
    }
    cout << "Обработка подстрок завершена.\n\n";

    // Проверяем, для каких стартовых позиций найдены все подстроки шаблона
    vector<int> final_match_positions;
    cout << "Проверка потенциальных стартов полного шаблона...\n";
    for (const auto& pair_map_item : potential_starts) {
        int full_start_0_based = pair_map_item.first;
        const vector<int>& found_indices_vec = pair_map_item.second;
        cout << "Проверка для старта в тексте с поз. " << full_start_0_based << ":\n";
        cout << "Найденные индексы подстрок (0-based): ";
        for(int idx : found_indices_vec) cout << idx << " ";
        cout << "\n";

        set<int> unique_indices(found_indices_vec.begin(), found_indices_vec.end());
        cout << "Уникальные индексы подстрок: ";
        for(int idx : unique_indices) cout << idx << " ";
        cout << "(всего " << unique_indices.size() << ")\n";

        // Если найдены все подстроки, добавляем позицию в результат
        if (unique_indices.size() == num_subpatterns) {
            final_match_positions.push_back(full_start_0_based + 1); // 1-based позиция
            cout << "Все " << num_subpatterns << " подстроки найдены! Добавлено совпадение: " << full_start_0_based + 1 << "\n";
        } 
        else {
            cout << "Не все подстроки (" << unique_indices.size() << "/" << num_subpatterns << ") найдены для этого старта.\n";
        }
    }
    cout << "Проверка завершена.\n\n";

    sort(final_match_positions.begin(), final_match_positions.end());
    
    cout << "Итоговые совпадения:\n";

    if (final_match_positions.empty()) {
        cout << "Совпадений не найдено.\n";
    }
    
    for (int pos : final_match_positions) {
        cout << pos << "\n";
    }

    return 0;
}