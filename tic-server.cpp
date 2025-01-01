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
constexpr int EMPTY = 0;

constexpr int WIN_LENGTH = 5;

struct Move {
	int row;
	int col;
};

// todo:
// - now calculate consecutive for all directions not just downward
// - add open 4 and 2 threes check
static int evaluate_large_board(const vector<int>& board, int gridSize, int player) {
	// Lambda function to check if 5 consecutive cells sum to target.
	auto checkConsecutive = [&](int row, int col, int dRow, int dCol) -> int {
		int sum = 0;
		for (int i = 0; i < WIN_LENGTH; ++i) {
			int r = row + i * dRow;
			int c = col + i * dCol;
			if (r < 0 || r >= gridSize || c < 0 || c >= gridSize) return 0;

			sum += board[r * gridSize + c];
		}
		return sum;
		};

	int score = 0;
	// Iterate through each cell to check all possible directions.
	for (int i = 0; i < gridSize; ++i) {
		for (int j = 0; j < gridSize; ++j) {
			if (board[i * gridSize + j] == EMPTY) continue;

			int horizontal = checkConsecutive(i, j, 0, 1);
			int vertical = checkConsecutive(i, j, 1, 0);
			int diagonal = checkConsecutive(i, j, 1, 1);
			int antiDiagonal = checkConsecutive(i, j, 1, -1);

			if (horizontal == WIN_LENGTH * player ||
				vertical == WIN_LENGTH * player ||
				diagonal == WIN_LENGTH * player ||
				antiDiagonal == WIN_LENGTH * player)
				score += 100000;

			if (horizontal == -WIN_LENGTH * player ||
				vertical == -WIN_LENGTH * player ||
				diagonal == -WIN_LENGTH * player ||
				antiDiagonal == -WIN_LENGTH * player)
				score -= 100000;

			// open 4
			if (horizontal == WIN_LENGTH - 1 ||
				vertical == WIN_LENGTH - 1 ||
				diagonal == WIN_LENGTH - 1 ||
				antiDiagonal == WIN_LENGTH - 1)
				score += 50000;

			if (horizontal == -WIN_LENGTH + 1 ||
				vertical == -WIN_LENGTH + 1 ||
				diagonal == -WIN_LENGTH + 1 ||
				antiDiagonal == -WIN_LENGTH + 1)
				score -= 50000;
			// end open 4

			// 2 threes check
			array<int, 4> allConsecutives = { horizontal,vertical,diagonal,antiDiagonal };

			int consecutiveCount = 0;
			int enemyConsecutiveCount = 0;

			for (auto conse : allConsecutives) {
				if (conse == WIN_LENGTH - 2) consecutiveCount++;
				else if (conse == -WIN_LENGTH + 2) enemyConsecutiveCount++;
			}

			if (consecutiveCount >= 2) score += 40000;
			if (enemyConsecutiveCount >= 2) score -= 40000;
			// end 2 threes check

			if (abs(horizontal) == 1 &&
				abs(vertical) == 1 &&
				abs(diagonal) == 1 &&
				abs(antiDiagonal) == 1)
				continue;

			score += (horizontal + vertical + diagonal + antiDiagonal) * 5;
		}
	}
	return score * player;
}

static int check_board(const vector<int>& board, int gridSize) {
	int diag_sum = 0;
	int anti_diag_sum = 0;
	for (int i = 0; i < gridSize; i++) {
		int row_sum = 0;
		int col_sum = 0;
		for (int j = 0; j < gridSize; j++) {
			row_sum += board[i * gridSize + j];
			col_sum += board[j * gridSize + i];
			if (i + j == gridSize - 1) {
				anti_diag_sum += board[i * gridSize + j];
			}
		}
		if (row_sum == -gridSize) {
			return COMP_LOSE;
		}
		else if (row_sum == gridSize) {
			return COMP_WIN;
		}
		if (col_sum == -gridSize) {
			return COMP_LOSE;
		}
		else if (col_sum == gridSize) {
			return COMP_WIN;
		}
		diag_sum += board[i * gridSize + i];
	}
	if (diag_sum == -gridSize) {
		return COMP_LOSE;
	}
	if (diag_sum == gridSize) {
		return COMP_WIN;
	}
	if (anti_diag_sum == -gridSize) {
		return COMP_LOSE;
	}
	if (anti_diag_sum == gridSize) {
		return COMP_WIN;
	}
	return DRAW;
}

static bool is_full(const vector<int>& board) {
	for (int i = 0; i < board.size(); i++) {
		if (board[i] == EMPTY) {
			return false;
		}
	}
	return true;
}

static bool are_neighbors_not_empty(const vector<int>& board, int gridSize, int row, int col) {
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (i == 0 && j == 0) continue;

			int row_offset = row + i;
			int col_offset = col + j;


			if (row_offset < 0 || row_offset >= gridSize || col_offset < 0 || col_offset >= gridSize) continue;

			if (board[row_offset * gridSize + col_offset] != EMPTY)
				return true;
		}
	}
	return false;
}

static int minimax_abPruning(vector<int>& board, int gridSize, int depth, int player, int alpha, int beta) {
	if (depth == 0) {
		if (gridSize <= 5) return check_board(board, gridSize);
		return evaluate_large_board(board, gridSize, player);
	}
	if (is_full(board)) {
		if (gridSize <= 5) return check_board(board, gridSize);
		else return DRAW; // optimize for large board, but may not be accurate
	}
	int best_score = player == PLAYER_COMP ? INT32_MIN : INT32_MAX;
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			size_t index = i * gridSize + j;
			if (board[index] == EMPTY && are_neighbors_not_empty(board, gridSize, i, j)) {
				board[index] = player;
				int score = minimax_abPruning(board, gridSize, depth - 1, -player, alpha, beta);
				board[index] = EMPTY;

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
	return best_score * depth;
}

// !: improvement is very minimal while cpu usage is significantly higher
static int minimax_abPruning_parallel(vector<int>& board, int gridSize, int depth, int player, int alpha, int beta) {
	if (depth == 0) {
		if (gridSize <= 5) return check_board(board, gridSize);
		return evaluate_large_board(board, gridSize, player);
	}
	if (is_full(board)) {
		if (gridSize <= 5) return check_board(board, gridSize);
		else return DRAW;
	}

	int best_score = player == PLAYER_COMP ? INT32_MIN : INT32_MAX;

	// Vector to store futures for parallel computation
	vector<future<int>> futures;

	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			size_t index = i * gridSize + j;
			if (board[index] == EMPTY && are_neighbors_not_empty(board, gridSize, i, j)) {
				board[index] = player;

				// Use std::async to launch the recursive call
				futures.push_back(async(launch::async, [=]() {
					vector<int> new_board = board; // Pass a copy of the board for thread safety
					return minimax_abPruning(new_board, gridSize, depth - 1, -player, alpha, beta);
					}));

				board[index] = EMPTY;
			}
		}
	}

	// Wait for all futures and update best_score
	for (auto& future : futures) {
		int score = future.get(); // Get the result of the async task
		if (player == PLAYER_COMP) {
			best_score = max(score, best_score);
			alpha = max(alpha, best_score);
		}
		else {
			best_score = min(score, best_score);
			beta = min(beta, best_score);
		}
		if (beta <= alpha) {
			break; // Pruning
		}
	}

	return best_score * depth;
}

random_device rd;
mt19937 rng(rd());
static Move find_best_move(vector<int>& board, int depth, int player = PLAYER_COMP) {
	int size = sqrt(board.size());
	int best_score = player == PLAYER_COMP ? INT32_MIN : INT32_MAX;
	vector<Move> best_moves;
	cout << "Board size: " << size << "\n";
	cout << "Thinking depth: " << depth << "\n";
	chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (board[i * size + j] == EMPTY && are_neighbors_not_empty(board, size, i, j)) {
				cout << "Thinking move: " << i + 1 << " " << j + 1 << "\n";
				board[i * size + j] = player;
				int score = minimax_abPruning_parallel(board, size, depth, -player, INT32_MIN, INT32_MAX);
				board[i * size + j] = EMPTY;

				if (player == PLAYER_COMP && score > best_score) {
					best_score = score;
					best_moves.clear();
					best_moves.push_back({ i, j });
				}
				else if (player == PLAYER_HUMAN && score < best_score) {
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
	if (best_moves.empty()) {
		cerr << "Error: No best move found." << endl;
		return { -1, -1 };
	}
	uniform_int_distribution<int> dist(0, best_moves.size() - 1);
	chrono::high_resolution_clock::time_point stop = chrono::high_resolution_clock::now();
	cout << "Thinking time taken: " << chrono::duration_cast<chrono::milliseconds>(stop - start).count() << "ms" << endl;
	Move move_chosen = best_moves[dist(rng)];
	cout << "Chosen move: " << move_chosen.row << " " << move_chosen.col << endl;
	return move_chosen;
}

static vector<int> toboard(string boardState) {
	vector<int> board;
	board.reserve(boardState.size());
	for (int i = 0; i < boardState.size(); i++) {
		switch (boardState[i]) {
		case '-':
			board.push_back(PLAYER_HUMAN);
			break;
		case '+':
			board.push_back(PLAYER_COMP);
			break;
		default:
			board.push_back(EMPTY);
			break;
		}
	}

	return board;
}

#ifdef _DEBUG
void print_board(const vector<int>& board) {
	int size = sqrt(board.size());
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			switch (board[i * size + j]) {
			case PLAYER_HUMAN:
				cout << "|X";
				break;
			case PLAYER_COMP:
				cout << "|O";
				break;
			default:
				cout << "| ";
				break;
			}
		}
		cout << "|\n";
	}
	cout << endl;
}
#endif // _DEBUG


int main()
{
	crow::SimpleApp app;

	app.loglevel(crow::LogLevel::Info);
	//#ifdef NDEBUG
		//crow::mustache::set_global_base("./");
	//#endif

	CROW_ROUTE(app, "/")([]() {
		auto page = crow::mustache::load_text("menu.html");
		return page;
		});

	CROW_ROUTE(app, "/<int>/<int>")([&](int difficulty, int size) {
		auto page = crow::mustache::load("tictactoe.html");
		crow::mustache::context ctx({ {"size", size} });

		string difficultyStr;
		float difficultyFloat = clamp(difficulty / 2.0f, 0.0f, 1.0f);
		switch (difficulty) {
		case 0:
			difficultyStr = "Easy";
			break;
		case 1:
			difficultyStr = "Medium";
			break;
		case 2:
			difficultyStr = "Hard";
			break;
		default:
			difficultyStr = "Undefined";
			break;
		}

		ctx["difficulty"] = difficultyStr;
		ctx["float_difficulty"] = difficultyFloat;

		return page.render(ctx);
		});

	CROW_ROUTE(app, "/findmove").methods("POST"_method)([&](const crow::request& req) {
		auto board_state = crow::json::load(req.body);
		if (!board_state)
			return crow::response(400);

		string state = board_state["state"].s();
		int depth = board_state["depth"].i();

		vector<int> board = toboard(state);

		if (board.empty())
			return crow::response(400);
		int size = sqrt(board.size());
		if (size * size != board.size())
			return crow::response(400);
		if (is_full(board))
			return crow::response(204);
		auto move = find_best_move(board, depth);

		crow::json::wvalue response;
		response["row"] = move.row;
		response["col"] = move.col;

		return crow::response(response);
		});

	CROW_ROUTE(app, "/suggestMove").methods("POST"_method)([&](const crow::request& req) {
		auto board_state = crow::json::load(req.body);
		if (!board_state)
			return crow::response(400);

		string state = board_state["state"].s();
		int depth = board_state["depth"].i();

		vector<int> board = toboard(state);

		if (board.empty())
			return crow::response(400);
		int size = sqrt(board.size());
		if (size * size != board.size())
			return crow::response(400);
		if (is_full(board))
			return crow::response(204);
		auto move = find_best_move(board, depth, PLAYER_HUMAN);

		crow::json::wvalue response;
		response["row"] = move.row;
		response["col"] = move.col;

		return crow::response(response);
		});

	system("start http://localhost:8000/");
	app.port(8000).multithreaded().run();
}