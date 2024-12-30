#include <vector>
#include <ctime>
#include <cstdio>

using namespace std;
int count2win = 5;
int Size = 6;
vector<vector<int>> current_board(Size, vector<int>(Size));
vector<vector<int>> s(Size, vector<int>(Size));
vector<vector<int>> q(Size, vector<int>(Size));
constexpr int winningMove = 9999999;
int openFour = 8888888;
int twoThrees = 7777777;
bool drawPos = false;
int nPos[4];
int dirA[4];
int w[] = { 0, 20, 17, 15.4, 14, 10 };
int machSq = 1;
int userSq = -1;

bool hasNeighbors(int i, int j, const vector<vector<int>>& f, int Size) {
    if (j > 0 && f[i][j - 1] != 0) return true; // Bên trái
    if (j + 1 < Size && f[i][j + 1] != 0) return true; // Bên phải

    if (i > 0) { // Kiểm tra hàng trên
        if (f[i - 1][j] != 0) return true; // Trên
        if (j > 0 && f[i - 1][j - 1] != 0) return true; // Trên trái
        if (j + 1 < Size && f[i - 1][j + 1] != 0) return true; // Trên phải
    }

    if (i + 1 < Size) { // Kiểm tra hàng dưới
        if (f[i + 1][j] != 0) return true; // Dưới
        if (j > 0 && f[i + 1][j - 1] != 0) return true; // Dưới trái
        if (j + 1 < Size && f[i + 1][j + 1] != 0) return true; // Dưới phải
    }

    return false; // Không có ô lân cận nào
}

// - 1. xét các ô liền nhau
// - 2. retrun 3 trường hợp mà nước tiếp theo:
// 1. hiển nhiên thắng (5 nc liên tiếp)
// 2. mở 4 (4 nc liên tiếp ko bị chặn)
// 3. 2 threes (cờ đôi)
int winningPos(int i, int j, int player) {
    int test3 = 0;
    int test4 = 0;
    int limit = count2win; // Number of connected squares required to win
    int consecutive_cell, move_offset, positive_offset, negative_offset;
    bool side1, side2;

    // Horizontal check
    consecutive_cell = 1;
    move_offset = 1;
    while (j + move_offset < Size && current_board[i][j + move_offset] == player) {
        consecutive_cell++;
        move_offset++;
    }
    positive_offset = move_offset;
    move_offset = 1;
    while (j - move_offset >= 0 && current_board[i][j - move_offset] == player) {
        consecutive_cell++;
        move_offset++;
    }
    negative_offset = move_offset;

    if (consecutive_cell > (limit - 1)) {
        return winningMove;
    }

    side1 = (j + positive_offset < Size && current_board[i][j + positive_offset] == 0);
    side2 = (j - negative_offset >= 0 && current_board[i][j - negative_offset] == 0);

    if (consecutive_cell == (limit - 1) && (side1 || side2)) test3++;
    if (side1 && side2) {
        if (consecutive_cell == (limit - 1)) test4 = 1;
        if (consecutive_cell == (limit - 2)) test3++;
    }

    // Vertical check
    consecutive_cell = 1;
    move_offset = 1;
    while (i + move_offset < Size && current_board[i + move_offset][j] == player) {
        consecutive_cell++;
        move_offset++;
    }
    positive_offset = move_offset;
    move_offset = 1;
    while (i - move_offset >= 0 && current_board[i - move_offset][j] == player) {
        consecutive_cell++;
        move_offset++;
    }
    negative_offset = move_offset;

    if (consecutive_cell > (limit - 1)) {
        return winningMove;
    }

    side1 = (i + positive_offset < Size && current_board[i + positive_offset][j] == 0);
    side2 = (i - negative_offset >= 0 && current_board[i - negative_offset][j] == 0);

    if (consecutive_cell == (limit - 1) && (side1 || side2)) test3++;
    if (side1 && side2) {
        if (consecutive_cell == (limit - 1)) test4 = 1;
        if (consecutive_cell == (limit - 2)) test3++;
    }

    // Diagonal (top-left to bottom-right)
    consecutive_cell = 1;
    move_offset = 1;
    while (i + move_offset < Size && j + move_offset < Size && current_board[i + move_offset][j + move_offset] == player) {
        consecutive_cell++;
        move_offset++;
    }
    positive_offset = move_offset;
    move_offset = 1;
    while (i - move_offset >= 0 && j - move_offset >= 0 && current_board[i - move_offset][j - move_offset] == player) {
        consecutive_cell++;
        move_offset++;
    }
    negative_offset = move_offset;

    if (consecutive_cell > (limit - 1)) {
        return winningMove;
    }

    side1 = (i + positive_offset < Size && j + positive_offset < Size && current_board[i + positive_offset][j + positive_offset] == 0);
    side2 = (i - negative_offset >= 0 && j - negative_offset >= 0 && current_board[i - negative_offset][j - negative_offset] == 0);

    if (consecutive_cell == (limit - 1) && (side1 || side2)) test3++;
    if (side1 && side2) {
        if (consecutive_cell == (limit - 1)) test4 = 1;
        if (consecutive_cell == (limit - 2)) test3++;
    }

    // Diagonal (bottom-left to top-right)
    consecutive_cell = 1;
    move_offset = 1;
    while (i + move_offset < Size && j - move_offset >= 0 && current_board[i + move_offset][j - move_offset] == player) {
        consecutive_cell++;
        move_offset++;
    }
    positive_offset = move_offset;
    move_offset = 1;
    while (i - move_offset >= 0 && j + move_offset < Size && current_board[i - move_offset][j + move_offset] == player) {
        consecutive_cell++;
        move_offset++;
    }
    negative_offset = move_offset;

    if (consecutive_cell > (limit - 1)) {
        return winningMove;
    }

    side1 = (i + positive_offset < Size && j - positive_offset >= 0 && current_board[i + positive_offset][j - positive_offset] == 0);
    side2 = (i - negative_offset >= 0 && j + negative_offset < Size && current_board[i - negative_offset][j + negative_offset] == 0);

    if (consecutive_cell == (limit - 1) && (side1 || side2)) test3++;
    if (side1 && side2) {
        if (consecutive_cell == (limit - 1)) test4 = 1;
        if (consecutive_cell == (limit - 2)) test3++;
    }

    if (test4) return openFour;
    if (test3 >= 2) return twoThrees;
    return -1;
}

int evaluatePos(vector<vector<int>>& a, int player) {
    int maxA = -1;
    drawPos = true;
    int limit = count2win; // Number of connected squares required to win
    int minM, minN, maxM, maxN, A1, A2, A3, A4, m;

    for (int i = 0; i < Size; i++) {
        for (int j = 0; j < Size; j++) {
            // Compute "value" a[i][j] of the (i, j) move
            if (current_board[i][j] != 0) {
                a[i][j] = -1;
                continue;
            }
            if (hasNeighbors(i, j, a, a.size()) == 0) {
                a[i][j] = -1;
                continue;
            }

            int wp = winningPos(i, j, player); // trường hợp rất nguy hiểm phải đánh 
            if (wp > 0) {
                a[i][j] = wp;
            }
            else {
                // lấy tọa độ 4 hướng dọc xa nhất có thể đánh đc 
                minM = max(0, i - (limit - 1));
                minN = max(0, j - (limit - 1));
                maxM = min(Size, i + limit);
                maxN = min(Size, j + limit);

                nPos[0] = 1; // consecutive
                A1 = 0;
                m = 1;
                while (j + m < maxN && current_board[i][j + m] != -player) {
                    nPos[0]++;
                    A1 += w[m] * current_board[i][j + m];
                    m++;
                }
                if (j + m >= Size || current_board[i][j + m] == -player)
                    A1 -= (current_board[i][j + m - 1] == player) ? (w[limit] * player) : 0;

                m = 1;
                while (j - m >= minN && current_board[i][j - m] != -player) {
                    nPos[0]++;
                    A1 += w[m] * current_board[i][j - m];
                    m++;
                }
                if (j - m < 0 || current_board[i][j - m] == -player)
                    A1 -= (current_board[i][j - m + 1] == player) ? (w[limit] * player) : 0;

                if (nPos[0] > (limit - 1)) drawPos = false;

                nPos[1] = 1;
                A2 = 0;
                m = 1;
                while (i + m < maxM && current_board[i + m][j] != -player) {
                    nPos[1]++;
                    A2 += w[m] * current_board[i + m][j];
                    m++;
                }
                if (i + m >= Size || current_board[i + m][j] == -player)
                    A2 -= (current_board[i + m - 1][j] == player) ? (w[limit] * player) : 0;

                m = 1;
                while (i - m >= minM && current_board[i - m][j] != -player) {
                    nPos[1]++;
                    A2 += w[m] * current_board[i - m][j];
                    m++;
                }
                if (i - m < 0 || current_board[i - m][j] == -player)
                    A2 -= (current_board[i - m + 1][j] == player) ? (w[limit] * player) : 0;

                if (nPos[1] > (limit - 1)) drawPos = false;

                nPos[2] = 1;
                A3 = 0;
                m = 1;
                while (i + m < maxM && j + m < maxN && current_board[i + m][j + m] != -player) {
                    nPos[2]++;
                    A3 += w[m] * current_board[i + m][j + m];
                    m++;
                }
                if (i + m >= Size || j + m >= Size || current_board[i + m][j + m] == -player)
                    A3 -= (current_board[i + m - 1][j + m - 1] == player) ? (w[limit] * player) : 0;

                m = 1;
                while (i - m >= minM && j - m >= minN && current_board[i - m][j - m] != -player) {
                    nPos[2]++;
                    A3 += w[m] * current_board[i - m][j - m];
                    m++;
                }
                if (i - m < 0 || j - m < 0 || current_board[i - m][j - m] == -player)
                    A3 -= (current_board[i - m + 1][j - m + 1] == player) ? (w[limit] * player) : 0;

                if (nPos[2] > (limit - 1)) drawPos = false;

                nPos[3] = 1;
                A4 = 0;
                m = 1;
                while (i + m < maxM && j - m >= minN && current_board[i + m][j - m] != -player) {
                    nPos[3]++;
                    A4 += w[m] * current_board[i + m][j - m];
                    m++;
                }
                if (i + m >= Size || j - m < 0 || current_board[i + m][j - m] == -player)
                    A4 -= (current_board[i + m - 1][j - m + 1] == player) ? (w[limit] * player) : 0;

                m = 1;
                while (i - m >= minM && j + m < maxN && current_board[i - m][j + m] != -player) {
                    nPos[3]++;
                    A4 += w[m] * current_board[i - m][j + m];
                    m++;
                }
                if (i - m < 0 || j + m >= Size || current_board[i - m][j + m] == -player)
                    A4 -= (current_board[i - m + 1][j + m - 1] == player) ? (w[limit] * player) : 0;

                if (nPos[3] > (limit - 1)) drawPos = false;

                dirA[0] = (nPos[0] > (limit - 1)) ? A1 * A1 : 0;
                dirA[1] = (nPos[1] > (limit - 1)) ? A2 * A2 : 0;
                dirA[2] = (nPos[2] > (limit - 1)) ? A3 * A3 : 0;
                dirA[3] = (nPos[3] > (limit - 1)) ? A4 * A4 : 0;

                A1 = 0;
                A2 = 0;
                for (int k = 1; k < limit; k++) {
                    if (dirA[k] >= A1) {
                        A2 = A1;
                        A1 = dirA[k];
                    }
                }
                a[i][j] = A1 + A2;
            }
            if (a[i][j] > maxA) {
                maxA = a[i][j];
            }
        }
    }
    return maxA;
}
vector<int> getBestMachMove() {
    int maxS = evaluatePos(s, machSq);
    int maxQ = evaluatePos(q, userSq);
    int nMax = 0;

    vector<int> iMax(Size * Size, 0);
    vector<int> jMax(Size * Size, 0);

    if (maxQ >= maxS) {
        maxS = -1;
        for (int i = 0; i < Size; i++) {
            for (int j = 0; j < Size; j++) {
                if (q[i][j] == maxQ) {
                    if (s[i][j] > maxS) {
                        maxS = s[i][j];
                        nMax = 0;
                    }
                    if (s[i][j] == maxS) {
                        iMax[nMax] = i;
                        jMax[nMax] = j;
                        nMax++;
                    }
                }
            }
        }
    }
    else {
        maxQ = -1;
        for (int i = 0; i < Size; i++) {
            for (int j = 0; j < Size; j++) {
                if (s[i][j] == maxS) {
                    if (q[i][j] > maxQ) {
                        maxQ = q[i][j];
                        nMax = 0;
                    }
                    if (q[i][j] == maxQ) {
                        iMax[nMax] = i;
                        jMax[nMax] = j;
                        nMax++;
                    }
                }
            }
        }
    }
    vector<int> result;
    // Randomly select one of the moves
    srand(time(0)); // Seed for randomness
    int randomK = rand() % nMax;
    int iMach = iMax[randomK];
    int jMach = jMax[randomK];

    result.push_back(iMach);
    result.push_back(jMach);

    return result;
}
void init(vector<vector<int>>& a, vector<vector<int>>& b, vector<vector<int>>& c) {
    for (int i = 0; i < Size; i++) {
        for (int j = 0; j < Size; j++) {
            a[i][j] = 0;
            b[i][j] = 0;
            c[i][j] = 0;
        }
    }
}