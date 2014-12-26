#include <iostream>     
#include <string>    
#include <stdio.h>
#include <stdlib.h>    
#include <time.h>
#include <ctype.h>  
#include <vector>
#include <sstream> 
#include <stack>
#include <algorithm>   
//#include "ai.cpp" 

using namespace std;

//------------------------------------------------board class -----------------------------------------

// the increment of location on board for different directions
const int DIREC[8][2] = { { 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 }, { -1, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 } };
bool on_board(int x, int y);
string convertInt(int number);

class Board {
private:
	typedef vector< vector<int> >                brd;


	stack<int> turntrack;                                             // record the previous turn 
	stack<brd> undo_list;                                             // record the previous table                
	int black_score;                                                  // the number of black disks
	int white_score;                                                  // the number of white disks
	int total_pieces;
	int undo_counter;

public:
    vector<vector<int> >  gameboard;   
       
	Board() :gameboard(8, vector<int>(8, 0)), difficulty(0), turn(-1),
		display(0), white_score(0), black_score(0), undo_counter(0)
	{
		gameboard[3][3] = gameboard[4][4] = 1;
		gameboard[3][4] = gameboard[4][3] = -1;
	}
	vector<string> possible_move;                                     // valid positions for the next move
	int turn;                                                         // -1 = black, 1 = white
	int get_bc() { return black_score; }                                // get black score
	int get_wc() { return white_score; }                                // get white score
	int difficulty;                                                   // EASY = 1, MED = 2, HARD = 3 
	void setdisplay(int a) { display = a; }
	int display;
	void set_difficulty(string s);
	int previous_turn();
	void move(string location);
	void undo();
	void display_score();
	void get_legal_moves();                                           // get all the legal moves
	void show();
	char pieceColor(int pieceNumber);
	bool isLegalMove(string location);                                // decide if the location is a legal move
	void print_possible_move();
	vector<double> best_moves();
};

void Board::print_possible_move(){
	cout << "Possible moves: ";
	for (int i = 1; i < possible_move.size() - 1; ++i){
		cout << possible_move[i] << ", ";
	}
	cout << possible_move[possible_move.size() - 1] << "\n";
}

// decide if the input is a move command
bool is_move(string token) {
	return (token[0] >= 'a' && token[0] <= 'h'
		&& token[1] >= '1' && token[1] <= '8');
}

string convertInt(int number)
{
	stringstream ss;                                                  //create a string stream
	ss << number;                                                     //add number to the stream
	return ss.str();                                                  //return a string with the contents of the stream
}

bool on_board(int x, int y){
	if (x < 0 || x > 7 || y < 0 || y > 7)
		return false;
	else
		return true;
}


bool Board::isLegalMove(string location){                             //a1; row = 0, col = 0
	int row = location[1] - '1';                                      // the coordinate 
	int col = location[0] - 'a';
	int x = 0;                                                        // the increment
	int y = 0;
	if (!on_board(col, row)){                                         //given location is not on the game board
		return false;
	}
	if (gameboard[row][col] == 0){
		int directioncounter = 1;
		bool directioncheck = false;
		for (int i = 0; i < 8; ++i){                                   // check 0-7 for 8 directions
			x = DIREC[i][0];
			y = DIREC[i][1];
			while (on_board(col + x, row + y)){
				if (gameboard[row + y][col + x] == -turn){
					x = x + DIREC[i][0];                              // check the next location on this direction
					y = y + DIREC[i][1];
					directioncheck = true;
				}
				else{
					break;
				}
			}

			if (on_board(col + x, row + y) && directioncheck == true){
				if (gameboard[row + y][col + x] == turn){
					return true;
				}
			}

			directioncheck = false;
		}
	}
	return false;
}

//Display the latest score
void Board::display_score(){
	int bs = 0;
	int ws = 0;
	vector<string> legal_moves;
	legal_moves.push_back("no moves. :(");
	for (char k = 'a'; k < 'i'; k++)
	{
		for (int j = 1; j < 9; j++)
		{
			if (gameboard[k - 'a'][j - 1] == 1)
			{
				ws++;                                                    // counter the number of white pieces
			}
			if (gameboard[k - 'a'][j - 1] == -1)
			{
				bs++;                                                    // counter the number of black pieces
			}
			if (isLegalMove(k + convertInt(j))){
				legal_moves.push_back(k + convertInt(j));                // push back all the legal moves
			}
		}
	}
	white_score = ws;
	black_score = bs;
	cout << "White: " << ws << endl;
	cout << "Black: " << bs << endl;
}

//This function gets all the legal moves available
void Board::get_legal_moves(){
	int bs = 0;
	int ws = 0;
	vector<string> legal_moves;
	legal_moves.push_back("no moves. :(");
	for (char k = 'a'; k < 'i'; k++)
	{
		for (int j = 1; j < 9; j++)
		{
			if (gameboard[k - 'a'][j - 1] == 1)
			{
				ws++;
			}
			if (gameboard[k - 'a'][j - 1] == -1)
			{
				bs++;
			}
			if (isLegalMove(k + convertInt(j))){
				legal_moves.push_back(k + convertInt(j));
			}
		}
	}
	possible_move = legal_moves;
	white_score = ws;
	black_score = bs;
}

//This function sets the difficulty level based on the given command
//There are three levels for the difficulty: "EASY", "MEDIUM" and "HARD".
//These three levels are representd with 1, 2 and 3.
void Board::set_difficulty(string s) {
	if (s == "EASY")
		difficulty = 1;
	else if (s == "MEDIUM")
		difficulty = 3;
	else if (s == "HARD")
		difficulty = 5;
	else
		cout << "invalid difficulty" << s << endl;
}

//this function works for the UNDO command, return a previous turn(black or white)
int Board::previous_turn() {
	if (turntrack.size() > 0) {
		int p = turntrack.top();
		turntrack.pop();
		return p;
	}
	else
		return 0;
}

//This is also a helper function for UNDO command, return the previous turn result
void Board::undo() {
	if (undo_counter < 10) {
		if (undo_list.size() != 0) {      //undo_list contains some previous turns
			gameboard = undo_list.top();
			undo_list.pop();
			if (previous_turn() == 1) {
				undo();
			}
			else {              //undo_list is empty
				turn = -1;
				undo_counter++;
				show();
				cout << 10 - undo_counter << " UNDO remain\n";
			}
		}
		else
			cout << "no more undo steps" << endl;
	}
	else
		cout << "at most undo 10 times\n";
}

//This function place a move on the board according to the move string recieved
void Board::move(string location) {
	int row = location[1] - '1';
	int col = location[0] - 'a';
	int x = 0;
	int y = 0;

	turntrack.push(turn);
	undo_list.push(gameboard);
	gameboard[row][col] = turn;

	int directioncounter = 1;
	bool directioncheck = false;
	for (int i = 0; i < 8; ++i){       //conversion of pieces begin here
		x = DIREC[i][0];
		y = DIREC[i][1];
		while (on_board(col + x, row + y)){
			if (gameboard[row + y][col + x] == -turn){
				x = x + DIREC[i][0];
				y = y + DIREC[i][1];
				directioncheck = true;
			}
			else{
				break;
			}
		}

		if (on_board(col + x, row + y) && directioncheck == true){
			if (gameboard[row + y][col + x] == turn){

				int k = 0;
				int p = 0;

				if (y > 0){ p = 1; }
				else if (y < 0){ p = -1; }
				else { p = 0; }
				if (x > 0){ k = 1; }
				else if (x < 0){ k = -1; }
				else { k = 0; }

				for (int j = 1; j < max(abs(x), abs(y)); ++j){
					gameboard[row + (j*p)][col + (j*k)] = turn;
				}
			}
		}

		directioncheck = false;
	}  //conversion of pieces ends here

	turn = turn * (-1);
	show();
	get_legal_moves();
}

//prints out the visual representation of squares that are black, white, or empty
char Board::pieceColor(int pieceNumber){
	if (pieceNumber == 1){
		return 'O';
	}
	else if (pieceNumber == -1){
		return 'X';
	}
	else {
		return '_';
	}
}

//shows the game's configuration on the turn: board, pieces, score, and the color's turn.
void Board::show(){
    if (display == 1) { 
	    cout << "  _ _ _ _ _ _ _ _\n";
	    for (int i = 0; i < 8; ++i){
		    cout << i + 1 << "|" << pieceColor(gameboard[i][0]) << "|" << pieceColor(gameboard[i][1])
		   	     << "|" << pieceColor(gameboard[i][2]) << "|" << pieceColor(gameboard[i][3])
			     << "|" << pieceColor(gameboard[i][4]) << "|" << pieceColor(gameboard[i][5]) << "|"
			     << pieceColor(gameboard[i][6]) << "|" << pieceColor(gameboard[i][7]) << "|\n";
	    }
	    cout << "  a b c d e f g h\n";
	    display_score();
	    cout << "It is your turn: ";
	    if (turn == 1){
		    cout << "White.\n";
	    }
	    else if (turn == -1){
		    cout << "Black.\n";
	    }
    }
}

/* ***************************** A.I. *************************************** */

int node_table[8][8] = {{ 30, -25, 10, 5, 5, 10, -25,  30},
                        {-25, -25,  1, 1, 1,  1, -25, -25}, 
                        { 10,   1,  5, 2, 2,  5,   1,  10}, 
                        {  5,   1,  2, 1, 1,  2,   1,   5}, 
                        {  5,   1,  2, 1, 1,  2,   1,   5}, 
                        { 10,   1,  5, 2, 2,  5,   1,  10}, 
                        {-25, -25,  1, 1, 1,  1, -25, -25}, 
                        { 30, -25, 10, 5, 5, 10, -25,  30}};

struct node {
    vector<vector<int> > board;
    int my_turn;
    int turn;
    string location;
//    int value;
};

struct result {
    string location;
    int value;  
    result(): location(""), value(0) {}
    result(int val): value(val) {} 
};

string convert(int row, int col) {
    char ch1 = 'a'+col;
    char ch2 = '1'+row;
    string str = "xx";
    str[0] = ch1;
    str[1] = ch2;       
    return str;   
}

bool isLegal(string location, vector<vector<int> > gameboard, int turn) {
    int row = location[1] - '1';                                      // the coordinate 
	int col = location[0] - 'a';
	int x = 0;                                                        // the increment
	int y = 0;
	if (!on_board(col, row)){                                         //given location is not on the game board
		return false;
	}
	if (gameboard[row][col] == 0){
		int directioncounter = 1;
		bool directioncheck = false;
		for (int i = 0; i < 8; ++i){                                   // check 0-7 for 8 directions
			x = DIREC[i][0];
			y = DIREC[i][1];
			while (on_board(col + x, row + y)){
				if (gameboard[row + y][col + x] == -turn){
					x = x + DIREC[i][0];                              // check the next location on this direction
					y = y + DIREC[i][1];
					directioncheck = true;
				}
				else
					break;
			}
			if (on_board(col + x, row + y) && directioncheck == true){
				if (gameboard[row + y][col + x] == turn)
					return true;
			}
			directioncheck = false;
		}
	}
	return false;
} 


vector<string> getLegalMove(vector<vector<int> > board, int turn) {
    string location;
   	vector<string> legal_moves;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            location = convert(i,j);
            if (isLegal(location, board, turn) )
                legal_moves.push_back(location); 
        }
    return legal_moves;                         
}

int get_node_value(string location) {
    int row = location[1] - '1';
	int col = location[0] - 'a';
    return node_table[row][col];  
}

vector<vector<int> > nodeMove(vector<vector<int> > gameboard, string location, int turn) {
    int row = location[1] - '1';
	int col = location[0] - 'a';
	int x = 0;
	int y = 0;
	gameboard[row][col] = turn;

	int directioncounter = 1;
	bool directioncheck = false;
	for (int i = 0; i < 8; ++i){       //conversion of pieces begin here
		x = DIREC[i][0];
		y = DIREC[i][1];
		while (on_board(col + x, row + y)){
			if (gameboard[row + y][col + x] == -turn){
				x = x + DIREC[i][0];
				y = y + DIREC[i][1];
				directioncheck = true;
			}
			else{
				break;
			}
		}
		if (on_board(col + x, row + y) && directioncheck == true){
			if (gameboard[row + y][col + x] == turn){

				int k = 0;
				int p = 0;

				if (y > 0){ p = 1; }
				else if (y < 0){ p = -1; }
				else { p = 0; }
				if (x > 0){ k = 1; }
				else if (x < 0){ k = -1; }
				else { k = 0; }

				for (int j = 1; j < max(abs(x), abs(y)); ++j){
					gameboard[row + (j*p)][col + (j*k)] = turn;
				}
			}
		}
		directioncheck = false;
	} 
    return gameboard;                
}

vector<node> getChildNode(vector<string> possibleMove, node n) {
    node tmp;
    vector<node> child;
    for (int i = 0; i < possibleMove.size(); i++) {
        tmp.board = nodeMove(n.board, possibleMove[i], n.turn);
        tmp.my_turn = n.my_turn;
        tmp.turn = -n.turn;
        tmp.location = possibleMove[i];
        child.push_back(tmp);
    } 
    return child;          
}

result maximum(result a, result b){
	if(a.value > b.value){
		return a;
	}
	else 
		return b;
}

result minimum(result a, result b){
	if(a.value < b.value){
		return a;
	}
	else 
		return b;
}
// d3-c5, c6-c7, f6-f5, f4-g7
result minmax(node node1, int height) {
	vector<string> possibleMove = getLegalMove(node1.board, node1.turn);
	if (height == 0) {
		result r;
		r.value = get_node_value(node1.location);   // + possibleMove.size()
		r.location = node1.location;
		cout << r.value << " " << r.location <<", ";                     //------------------------------
		return r;
	}
	else {
		vector<node> possible = getChildNode(possibleMove, node1);
		if (node1.turn == node1.my_turn) {
			result a(-9999);
			for (int i = 0; i < possible.size(); i++)
				a = maximum(a, minmax(possible[i], height - 1));
			if (node1.location != "")
				a.location = node1.location;
			cout << "a: " << a.value << a.location << endl;               //--------------------------
			return a;
		}
		else if (node1.turn == -node1.my_turn) {
			result b(9999);
			for (int i = 0; i < possible.size(); i++)
				b = minimum(b, minmax(possible[i], height - 1));
			if (node1.location != "")
				b.location = node1.location;
			cout << "b: " << b.value << b.location << endl;                 //------------------------
			return b;
		}
		else {
			cout << "turn = " << node1.turn << endl;
			exit(1);
		}
	}
}

string ai_function(Board game) {
    node n;
    n.board = game.gameboard;
    n.my_turn = game.turn;
    n.turn = game.turn;
    n.location = "";
    result i = minmax(n, game.difficulty);
    string location = i.location;
    cout << "f: " << i.value <<endl;                          //--------------------------------
    return location;
}

/* ***************************** A.I. *************************************** */

//This function gets the conditions to end the game
void win(Board& game) {
	if ((game.get_bc() + game.get_wc() == 64) || (game.get_bc() == 0) || (game.get_wc() == 0))
	{
		cout << "Game Over!" << endl;
		cout << "BLACK VS WHITE: \n";
		cout << game.get_bc() << " : " << game.get_wc() << endl;

		if (game.get_bc() == 0){
			cout << "White Wins!" << endl;
		}
		if (game.get_wc() == 0){
			cout << "Black Wins!" << endl;
		}
		if ((game.get_bc() + game.get_wc() == 64) && game.get_bc() > game.get_wc())
		{
			cout << "Black Wins!" << endl;
		}
		if ((game.get_bc() + game.get_wc() == 64) && game.get_wc() > game.get_bc())
		{
			cout << "White Wins!" << endl;
		}
		if ((game.get_bc() + game.get_wc() == 64) && game.get_wc() == game.get_bc()){
			cout << "DRAW!" << endl;
		}
		exit(0);
	}
}

//once a move has been declared, this will handle placing the pieces, getting new moves, check if there's a pass, and checking if the game is over.
void play(string token, Board& game) {
	game.move(token);
	win(game);
	if (game.possible_move.size() == 1){
		cout << "PASS!\n";
		game.turn = -game.turn;
		cout << "It is now your turn: ";
		if (game.turn == 1){
			cout << "White.\n";
			game.get_legal_moves();
			game.print_possible_move();
		}
		else if (game.turn == -1){
			cout << "Black.\n";
			game.get_legal_moves();
			game.print_possible_move();
		}
	}
	else { game.print_possible_move(); }
}

//parsing command line for expressions
void expr(string token, Board& game) {

	if (token == "UNDO") {
		game.undo();
	}
	else if (token == "DISPLAY ON") {
		game.setdisplay(1);
		game.show();
		game.print_possible_move();
	}
	else if (token == "DISPLAY OFF") {
        game.setdisplay(0); 
    }
	else if (is_move(token) && game.isLegalMove(token))  {
		play(token, game);
		while (game.turn == 1) {
			int displayflag = 0;
			if(game.display == 1){ displayflag = 1; }
			game.setdisplay(0);
			//cout << "starting ai_function!\n";
			string location = ai_function(game);
			//cout << "received a move!\n";
			cout << location << endl;
			//cout << "^printing location here^!\n";
			if(displayflag == 1){
				game.setdisplay(1);
			}
			play(location, game);
			//cout << "made my move!\n";
		}
	}
	else{
		cout << "Illegal!" << endl;
	}
}

int main(){	
	cout << "WELCOME\n";
	cout << "Here are the commands for the Reversi game: " << endl;
	cout << "LOCAL + (RANDOM, EASY, MEDIUM, HARD)\n";
	cout << "DISPLAY ON: toggles board display" << endl;
	cout << "UNDO: undo AI's move and your last move. Up to 10 times." << endl;

    Board game;
	string input;
	int flag = 0;
    do {
        getline(cin, input);
        if (input.substr(0,5) == "LOCAL" ) {
             if (input.substr(5,5) ==  " EASY") {
                 game.set_difficulty("EASY");
                 cout << "local game - easy: DISPLAY ON/move/UNDO\n";
             }
             else if (input.substr(5,7) == " MEDIUM") {
                 game.set_difficulty("MEDIUM");
                 cout << "local game - medium: DISPLAY ON/move/UNDO\n";
             }
             else if (input.substr(5,5) == " HARD") {
                 game.set_difficulty("HARD");
                 cout << "local game - hard: DISPLAY ON/move/UNDO\n";
             }
             else {
                 cout << "invalid difficulty: " << input <<" enter again\n";
                 flag = 1; 
                 continue;
             } 
             game.get_legal_moves();
	         while (getline(cin, input)){
		         expr(input, game);
	         }               
        }
        else if (input.substr(0,6) == "REMOTE") {
//           remote();             
        }
        else {
            cout << "Please choose LOCAL or REMOTE first.\n";
            flag = 1; 
            continue;    
        }
    } while (flag);

    
	return 0;
}

	
