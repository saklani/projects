/*
 * File: tictactoe.cpp
 * -------------------
 * This program plays a game of tic-tac-toe with the user.  The
 * code is designed to make it easy to adapt the general structure
 * to other games.
 */


#include "Vector.h"
#include "grid.h"

/*
 * Constants: WINNING_POSITION, NEUTRAL_POSITION, LOSING_POSITION
 * --------------------------------------------------------------
 * These constants define a rating system for game positions.  A
 * rating is an integer centered at 0 as the neutral score: ratings
 * greater than 0 are good for the current player, ratings less than
 * 0 are good for the opponent.  The constants WINNING_POSITION and
 * LOSING_POSITION are opposite in value and indicate a position that
 * is a forced win or loss, respectively.  In a game in which the
 * analysis is complete, no intermediate values ever arise.  If the
 * full tree is too large to analyze, the EvaluatePosition function
 * returns integers that fall between the two extremes.
 */
const int WINNING_POSITION = 1000;
const int NEUTRAL_POSITION = 0;
const int LOSING_POSITION = -WINNING_POSITION;
/*
 * Type: playerT
 * -------------
 * This type is used to distinguish the human and computer
 * players and keep track of who has the current turn.
 */
enum playerT {
    Human, Computer
};

/*
 * Type: moveT
 * -----------
 * For any particular game, the moveT type must keep track of the
 * information necessary to describe a move.  For tic-tac-toe,
 * a moveT is simply an integer identifying the number of one of
 * the nine squares, as follows:
 */
typedef int moveT;


/*
* Type: stateT
* ------------
* For any game, the stateT structure records the current state of
* the game.  As in Chapter 4, the tic-tac-toe board is represented
* using a Grid<char>; the elements must be either 'X', 'O', or ' '.
* In addition to the board array, the code stores a playerT value
* to indicate whose turn it is.  In this game, the stateT structure
* also contains the total number of moves so that functions can
* find this value without counting the number of occupied squares.
*/
struct stateT {
    Grid<char> board;
    playerT whoseTurn;
    int turnsTaken;
};


/*
 * Constant: MAX_DEPTH
 * -------------------
 * This constant indicates the maximum depth to which the recursive
 * search for the best move is allowed to proceed.
 */
const int MAX_DEPTH = 10000;

/*
 * Constant: FIRST_PLAYER
 * ----------------------
 * This constant indicates which player goes first.
 */
const playerT FIRST_PLAYER = Computer;

/* Private function prototypes */
void GiveInstructions();

moveT FindBestMove(stateT state, int depth, int &pRating);

int EvaluatePosition(stateT state, int depth);

stateT NewGame();

void DisplayGame(stateT state);

void DisplayMove(moveT move);

char PlayerMark(playerT player);

moveT GetUserMove(stateT state);

moveT ChooseComputerMove(stateT state);

void GenerateMoveList(stateT state, Vector<moveT> &moveList);

bool MoveIsLegal(moveT move, stateT state);

void MakeMove(stateT &state, moveT move);

void RetractMove(stateT &state, moveT move);

bool GameIsOver(stateT state);

void AnnounceResult(stateT state);

playerT WhoseTurn(stateT state);

playerT Opponent(playerT player);

int EvaluateStaticPosition(stateT state);

bool CheckForWin(stateT state, playerT player);

bool CheckForWin(Grid<char> &board, char mark);

bool CheckLine(Grid<char> &board, char mark,
               int row, int col, int dRow, int dCol);


/*
 * Main program
 * ------------
 * The main program, along with the functions FindBestMove and
 * EvaluatePosition, are general in their design and can be
 * used with most two-player games.  The specific details of
 * tic-tac-toe do not appear in these functions and are instead
 * encapsulated in the stateT and moveT data structures and a
 * a variety of subsidiary functions.
 */

int main() {
    GiveInstructions();
    stateT state = NewGame();
    moveT move;
    while (!GameIsOver(state)) {
        DisplayGame(state);
        switch (WhoseTurn(state)) {
            case Human:
                move = GetUserMove(state);
                break;
            case Computer:
                move = ChooseComputerMove(state);
                DisplayMove(move);
                break;
        }
        MakeMove(state, move);
    }
    AnnounceResult(state);
    return 0;
}

 /* Function: FindBestMove
 * Usage: move = FindBestMove(state, depth, pRating);
 * --------------------------------------------------
 * This function finds the best move for the current player, given
 * the specified state of the game.  The depth parameter and the
 * constant MAX_DEPTH are used to limit the depth of the search
 * for games that are too difficult to analyze in full detail.
 * The function returns the best move by storing an integer in
 * the variable to which pRating points.
 */

moveT FindBestMove(stateT state, int depth, int &rating) {
    Vector<moveT> moveList;
    GenerateMoveList(state, moveList);
    int nMoves = moveList.size();
    if (nMoves == 0) std::cout << ("No moves available");
    moveT bestMove;
    int minRating = WINNING_POSITION + 1;
    for (int i = 0; i < nMoves && minRating != LOSING_POSITION; i++) {
        moveT move = moveList[i];
        MakeMove(state, move);
        int curRating = EvaluatePosition(state, depth + 1);
        if (curRating < minRating) {
            bestMove = move;
            minRating = curRating;
        }
        RetractMove(state, move);
    }
    rating = -minRating;
    return bestMove;
}


/* Function: EvaluatePosition
 * Usage: rating = EvaluatePosition(state, depth);
 * -----------------------------------------------
 * This function evaluates a position by finding the rating of
 * the best move in that position.  The depth parameter and the
 * constant MAX_DEPTH are used to limit the depth of the search.
*/
int EvaluatePosition(stateT state, int depth) {
    int rating;
    if (GameIsOver(state) || depth >= MAX_DEPTH) {
        return EvaluateStaticPosition(state);
    }
    FindBestMove(state, depth, rating);
    return rating;
}