#include "crow.h"
#include <iostream>

#include <climits>
#include <random>
#include <vector>

using namespace std;

constexpr int COMP_WIN = 1;
constexpr int COMP_LOSE = -1;
constexpr int DRAW = 0;

constexpr int PLAYER_COMP = 1;
constexpr int PLAYER_HUMAN = -1;

static int check_board(const vector<int>& board) {
	int size = sqrt(board.size());
	int diag_sum = 0;
	int anti_diag_sum = 0;
	for (int i = 0; i < size; i++) {
		int row_sum = 0;
		int col_sum = 0;
		for (int j = 0; j < size; j++) {
			row_sum += board[i * size + j];
			col_sum += board[j * size + i];
			if (i + j == size - 1) {
				anti_diag_sum += board[i * size + j];
			}
		}
		if (row_sum == -size) {
			return COMP_LOSE;
		}
		else if (row_sum == size) {
			return COMP_WIN;
		}
		if (col_sum == -size) {
			return COMP_LOSE;
		}
		else if (col_sum == size) {
			return COMP_WIN;
		}
		diag_sum += board[i * size + i];
	}
	if (diag_sum == -size) {
		return COMP_LOSE;
	}
	if (diag_sum == size) {
		return COMP_WIN;
	}
	if (anti_diag_sum == -size) {
		return COMP_LOSE;
	}
	if (anti_diag_sum == size) {
		return COMP_WIN;
	}
	return DRAW;
}

static bool is_full(const vector<int>& board) {
	int size = sqrt(board.size());
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (board[i * size + j] == 0) {
				return false;
			}
		}
	}
	return true;
}

static int minimax_abPruning(vector<int> board, int depth, int player, int alpha, int beta) {
	int size = sqrt(board.size());
	if (depth == 0) {
		return 0;
	}
	int result = check_board(board);
	if (result != 0) {
		return result;
	}
	if (is_full(board)) {
		return DRAW;
	}
	int best_score = player == PLAYER_COMP ? INT_MIN : INT_MAX;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (board[i * size + j] == 0) {
				board[i * size + j] = player;
				int score = minimax_abPruning(board, depth - 1, -player, alpha, beta);
				board[i * size + j] = 0;
				if (player == PLAYER_COMP) {
					best_score = max(score, best_score);
					alpha = max(alpha, best_score);
				}
				else {
					best_score = min(score, best_score);
					beta = min(beta, best_score);
				}
				if (beta <= alpha) {
					break;
				}
			}
		}
	}
	return best_score;
}

struct Move {
	int row;
	int col;
};

random_device rd;
mt19937 rng(rd());
static Move find_best_move(vector<int> board, int depth) {
	int size = sqrt(board.size());
	int best_score = INT_MIN;
	vector<Move> best_moves;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (board[i * size + j] == 0) {
				cout << "Thinking on move: " << i + 1 << " " << j + 1 << endl;
				board[i * size + j] = PLAYER_COMP;
				int score = minimax_abPruning(board, depth, PLAYER_HUMAN, INT_MIN, INT_MAX);
				board[i * size + j] = 0;
				if (score > best_score) {
					best_score = score;
					best_moves.clear();
					best_moves.push_back({ i, j });
				}
				else if (score == best_score) {
					best_moves.push_back({ i, j });
				}
			}
		}
	}
	uniform_int_distribution<int> dist(0, best_moves.size() - 1);
	return best_moves[dist(rng)];
}

static vector<int> toboard(string boardState) {
	int size = sqrt(boardState.size());
	vector<int> board;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			switch (boardState[i * size + j]) {
			case '-':
				board.push_back(PLAYER_HUMAN);
				break;
			case '+':
				board.push_back(PLAYER_COMP);
				break;
			default:
				board.push_back(0);
				break;
			}
		}
	}
	return board;
}

int main()
{
	crow::SimpleApp app;

	app.loglevel(crow::LogLevel::Info);
#ifdef NDEBUG
	crow::mustache::set_global_base("./");
#endif
	CROW_ROUTE(app, "/<int>")([](int size) {
		auto page = crow::mustache::load("tictactoe.html");
		crow::mustache::context ctx({ {"size", size} });
		return page.render(ctx);
		});

	CROW_ROUTE(app, "/findmove").methods("POST"_method)([](const crow::request& req) {
		auto board_state = crow::json::load(req.body);
		if (!board_state)
			return crow::response(400);

		string state = board_state["state"].s();
		auto depth = board_state["depth"].i();

		cout << state << endl;

		auto board = toboard(state);
		if (board.empty())
			return crow::response(400);
		if (is_full(board))
			return crow::response(204);
		auto move = find_best_move(board, depth);

		crow::json::wvalue response;
		response["row"] = move.row;
		response["col"] = move.col;

		return crow::response(response);
		});

	system("start http://localhost:8000/4");
	app.port(8000).multithreaded().run();
}