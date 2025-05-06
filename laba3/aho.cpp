#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <tuple>
#include <algorithm>

using namespace std;

struct TrieNode {
    map<char, int> children;
    int fail;
    vector<pair<int, int>> output;
    bool is_terminal;
    int output_link;
    TrieNode() : fail(0), is_terminal(false), output_link(0) {}
};

vector<TrieNode> nodes;
vector<int> fail_depth_memo;
vector<int> output_depth_memo;

int get_fail_depth(int node_idx) {
    if (node_idx == 0) return 0;
    if (fail_depth_memo[node_idx] != -1) return fail_depth_memo[node_idx];
    fail_depth_memo[node_idx] = 1 + get_fail_depth(nodes[node_idx].fail);
    return fail_depth_memo[node_idx];
}

int get_output_depth(int node_idx) {
    if (node_idx == 0 || nodes[node_idx].output_link == 0) return 0;
    if (output_depth_memo[node_idx] != -1) return output_depth_memo[node_idx];
    output_depth_memo[node_idx] = 1 + get_output_depth(nodes[node_idx].output_link);
    return output_depth_memo[node_idx];
}

pair<int, int> calculate_max_depths() {
    int max_fail_depth = 0, max_output_depth = 0;
    int num_nodes = nodes.size();
    fail_depth_memo.assign(num_nodes, -1);
    output_depth_memo.assign(num_nodes, -1);
    for (int i = 1; i < num_nodes; ++i) {
        max_fail_depth = max(max_fail_depth, get_fail_depth(i));
        max_output_depth = max(max_output_depth, get_output_depth(i));
    }
    return {max_fail_depth, max_output_depth};
}

void add_string(const string& pattern, int pattern_index) {
    cout << "Добавляю шаблон " << pattern_index << ": \"" << pattern << "\"\n";
    int current = 0;
    for (char ch : pattern) {
        if (!nodes[current].children.count(ch)) {
            int nxt = nodes.size();
            cout << "  Создаю узел " << nxt << " для символа '" << ch
                 << "' от узла " << current << "\n";
            nodes[current].children[ch] = nxt;
            nodes.emplace_back();
        }
        current = nodes[current].children[ch];
        cout << "  Перешёл в узел " << current << " после символа '" << ch << "'\n";
    }
    nodes[current].output.emplace_back(pattern_index, pattern.length());
    nodes[current].is_terminal = true;
    cout << "  Шаблон " << pattern_index
         << " завершён в узле " << current << "\n\n";
}

void build_failure_links() {
    cout << "Построение суффиксных ссылок...\n";
    queue<int> q;
    for (auto& p : nodes[0].children) {
        q.push(p.second);
    }
    while (!q.empty()) {
        int cur = q.front(); q.pop();
        int f = nodes[cur].fail;
        if (f != 0) {
            if (nodes[f].is_terminal)
                nodes[cur].output_link = f;
            else
                nodes[cur].output_link = nodes[f].output_link;
        }
        for (auto& p : nodes[cur].children) {
            char c = p.first;
            int nxt = p.second;
            q.push(nxt);
            int ff = nodes[cur].fail;
            while (ff && !nodes[ff].children.count(c))
                ff = nodes[ff].fail;
            if (nodes[ff].children.count(c))
                nodes[nxt].fail = nodes[ff].children[c];
            else
                nodes[nxt].fail = 0;
            cout << "  Узел " << nxt
                 << ": ссылка на суффикс -> " << nodes[nxt].fail
                 << ", ссылка на выходной -> " << nodes[nxt].output_link << "\n";
        }
    }
    cout << "Суффиксные ссылки построены.\n\n";
}

vector<pair<int,int>> search_text(const string& text) {
    cout << "Поиск в тексте: \"" << text << "\"\n";
    vector<pair<int,int>> results;
    int current = 0;
    for (int i = 0; i < (int)text.size(); ++i) {
        char ch = text[i];
        cout << "При позиции " << i+1 << " символ '" << ch
             << "', текущий узел = " << current;
        while (current && !nodes[current].children.count(ch))
            current = nodes[current].fail;
        if (nodes[current].children.count(ch))
            current = nodes[current].children[ch];
        cout << " -> перешёл в узел " << current << "\n";
        int tmp = current;
        while (tmp) {
            if (nodes[tmp].is_terminal) {
                for (auto& m : nodes[tmp].output) {
                    int idx = m.first, len = m.second;
                    int pos = i - len + 2;
                    cout << "  Найдено совпадение: шаблон " << idx
                         << " начинается с позиции " << pos << "\n";
                    results.emplace_back(pos, idx);
                }
            }
            tmp = nodes[tmp].output_link;
        }
    }
    cout << "\nПоиск завершён.\n\n";
    return results;
}

void merge(vector<pair<int,int>>& v, int l, int m, int r) {
    int n1 = m-l+1, n2 = r-m;
    vector<pair<int,int>> L(v.begin()+l, v.begin()+l+n1);
    vector<pair<int,int>> R(v.begin()+m+1, v.begin()+m+1+n2);
    int i=0,j=0,k=l;
    while (i<n1 && j<n2)
        v[k++] = (L[i].first < R[j].first ||
                  (L[i].first==R[j].first && L[i].second<=R[j].second))
                 ? L[i++] : R[j++];
    while (i<n1) v[k++]=L[i++];
    while (j<n2) v[k++]=R[j++];
}

void mergeSort(vector<pair<int,int>>& v, int l, int r) {
    if (l<r) {
        int m = l + (r-l)/2;
        mergeSort(v, l, m);
        mergeSort(v, m+1, r);
        merge(v, l, m, r);
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string text;
    cin >> text;
    int n; 
    cin >> n;

    nodes.emplace_back();
    vector<string> patterns(n);
    for (int i = 0; i < n; ++i) {
        cin >> patterns[i];
        add_string(patterns[i], i+1);
    }

    build_failure_links();

    auto depths = calculate_max_depths();
    cout << "Максимальная глубина суффиксных ссылок: " << depths.first << "\n";
    cout << "Максимальная глубина выходных ссылок: " << depths.second << "\n\n";

    auto matches = search_text(text);
    if (!matches.empty())
        mergeSort(matches, 0, matches.size()-1);

    cout << "Совпадения:\n";
    for (auto& m : matches)
        cout << m.first << " " << m.second << "\n";

    return 0;
}