// ============================================================
//  Sudoku Solver (C++17)
//  - 2D array representation of the 9x9 grid
//  - Backtracking algorithm with row / column / 3x3 box checks
//  - Interactive console GUI for input & display
//
//  Build:  g++ -std=c++17 -O2 sudoku_solver.cpp -o sudoku
//  Run:    ./sudoku
// ============================================================

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <chrono>
#include <thread>

using namespace std;

const int N = 9;              // grid size
using Grid = vector<vector<int>>;

// -------------------- Rendering --------------------
void printGrid(const Grid& g) {
    cout << "\n    ";
    for (int c = 1; c <= N; ++c) cout << c << (c % 3 == 0 ? "   " : " ");
    cout << "\n  +-------+-------+-------+\n";
    for (int r = 0; r < N; ++r) {
        cout << r + 1 << " | ";
        for (int c = 0; c < N; ++c) {
            if (g[r][c] == 0) cout << ". ";
            else              cout << g[r][c] << " ";
            if ((c + 1) % 3 == 0) cout << "| ";
        }
        cout << "\n";
        if ((r + 1) % 3 == 0) cout << "  +-------+-------+-------+\n";
    }
    cout << "\n";
}

// -------------------- Validity --------------------
bool isSafe(const Grid& g, int row, int col, int num) {
    for (int i = 0; i < N; ++i) {
        if (g[row][i] == num) return false;              // row
        if (g[i][col] == num) return false;              // column
    }
    int br = (row / 3) * 3;
    int bc = (col / 3) * 3;
    for (int r = br; r < br + 3; ++r)                    // 3x3 sub-grid
        for (int c = bc; c < bc + 3; ++c)
            if (g[r][c] == num) return false;
    return true;
}

// Verify the initial puzzle contains no rule violations.
bool isValidPuzzle(const Grid& g) {
    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            int v = g[r][c];
            if (v == 0) continue;
            // Temporarily blank the cell and check safety of v at (r,c).
            Grid tmp = g;
            tmp[r][c] = 0;
            if (!isSafe(tmp, r, c, v)) return false;
        }
    }
    return true;
}

// -------------------- Solver --------------------
bool findEmpty(const Grid& g, int& row, int& col) {
    for (row = 0; row < N; ++row)
        for (col = 0; col < N; ++col)
            if (g[row][col] == 0) return true;
    return false;
}

long long backtrackSteps = 0;

bool solve(Grid& g) {
    int row, col;
    if (!findEmpty(g, row, col)) return true;   // solved
    for (int num = 1; num <= 9; ++num) {
        ++backtrackSteps;
        if (isSafe(g, row, col, num)) {
            g[row][col] = num;
            if (solve(g)) return true;
            g[row][col] = 0;                    // undo
        }
    }
    return false;
}

// -------------------- Input --------------------
void trim(string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    if (a == string::npos) { s.clear(); return; }
    s = s.substr(a, b - a + 1);
}

// Load an 81-character string ('.' or '0' for blanks) into the grid.
bool loadFromString(Grid& g, const string& raw) {
    string s;
    for (char ch : raw) if (!isspace((unsigned char)ch)) s.push_back(ch);
    if ((int)s.size() != 81) return false;
    for (int i = 0; i < 81; ++i) {
        char ch = s[i];
        int v;
        if (ch == '.' || ch == '0') v = 0;
        else if (ch >= '1' && ch <= '9') v = ch - '0';
        else return false;
        g[i / 9][i % 9] = v;
    }
    return true;
}

Grid inputRowByRow() {
    Grid g(N, vector<int>(N, 0));
    cout << "\nEnter each of the 9 rows. Use digits 1-9, and '.' or '0' for blanks.\n";
    cout << "Example row:  53..7....\n\n";
    for (int r = 0; r < N; ++r) {
        while (true) {
            cout << "Row " << r + 1 << ": ";
            string line;
            if (!getline(cin, line)) { cerr << "Input closed.\n"; exit(1); }
            trim(line);
            // strip inner spaces
            string s;
            for (char ch : line) if (!isspace((unsigned char)ch)) s.push_back(ch);
            if ((int)s.size() != 9) { cout << "  ! Need exactly 9 characters.\n"; continue; }
            bool ok = true;
            for (int c = 0; c < 9; ++c) {
                char ch = s[c];
                if (ch == '.' || ch == '0')      g[r][c] = 0;
                else if (ch >= '1' && ch <= '9') g[r][c] = ch - '0';
                else { ok = false; break; }
            }
            if (!ok) { cout << "  ! Only digits 1-9, '.' or '0' allowed.\n"; continue; }
            break;
        }
    }
    return g;
}

Grid inputOneLine() {
    Grid g(N, vector<int>(N, 0));
    cout << "\nPaste all 81 characters on one line (use '.' or '0' for blanks):\n> ";
    string line;
    getline(cin, line);
    if (!loadFromString(g, line)) {
        cout << "Invalid input, using an empty grid instead.\n";
        g.assign(N, vector<int>(N, 0));
    }
    return g;
}

Grid loadSample() {
    Grid g(N, vector<int>(N, 0));
    string sample =
        "53..7...."
        "6..195..."
        ".98....6."
        "8...6...3"
        "4..8.3..1"
        "7...2...6"
        ".6....28."
        "...419..5"
        "....8..79";
    loadFromString(g, sample);
    return g;
}

// -------------------- Menu --------------------
int menu() {
    cout << "\n================ SUDOKU SOLVER =================\n";
    cout << " 1) Enter puzzle row by row\n";
    cout << " 2) Paste puzzle as a single 81-char line\n";
    cout << " 3) Load built-in sample puzzle\n";
    cout << " 4) Quit\n";
    cout << "------------------------------------------------\n";
    cout << "Choice: ";
    string s; getline(cin, s); trim(s);
    if (s.size() == 1 && s[0] >= '1' && s[0] <= '4') return s[0] - '0';
    return 0;
}

int main() {
    cout << "  ____            _       _          ____        _\n";
    cout << " / ___| _   _  __| | ___ | | ___   _/ ___|  ___ | |_   _____ _ __\n";
    cout << " \\___ \\| | | |/ _` |/ _ \\| |/ / | | \\___ \\ / _ \\| \\ \\ / / _ \\ '__|\n";
    cout << "  ___) | |_| | (_| | (_) |   <| |_| |___) | (_) | |\\ V /  __/ |\n";
    cout << " |____/ \\__,_|\\__,_|\\___/|_|\\_\\\\__,_|____/ \\___/|_| \\_/ \\___|_|\n";
    cout << "                       9x9 backtracking solver\n";

    while (true) {
        int choice = menu();
        Grid g;
        if      (choice == 1) g = inputRowByRow();
        else if (choice == 2) g = inputOneLine();
        else if (choice == 3) g = loadSample();
        else if (choice == 4) { cout << "Goodbye!\n"; return 0; }
        else { cout << "Invalid choice.\n"; continue; }

        cout << "\n---- Input Puzzle ----";
        printGrid(g);

        if (!isValidPuzzle(g)) {
            cout << "!! Puzzle violates Sudoku rules (duplicate in row/col/box).\n";
            continue;
        }

        backtrackSteps = 0;
        auto t0 = chrono::high_resolution_clock::now();
        bool ok = solve(g);
        auto t1 = chrono::high_resolution_clock::now();
        double ms = chrono::duration<double, milli>(t1 - t0).count();

        if (ok) {
            cout << "---- Solution ----";
            printGrid(g);
            cout << "Solved in " << fixed << setprecision(3) << ms
                 << " ms  |  backtracking steps: " << backtrackSteps << "\n";
        } else {
            cout << "No solution exists for the given puzzle.\n";
        }
    }
    return 0;
}
