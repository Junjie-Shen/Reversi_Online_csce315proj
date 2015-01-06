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
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h> 
#include <netdb.h>
#include <sys/stat.h>   
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <sys/select.h> 
#include <string.h>

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
	int passflag;
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
	if (display == 1) {
		cout << "Possible moves: ";
		for (int i = 1; i < possible_move.size() - 1; ++i){
			cout << possible_move[i] << ", ";
		}
		cout << possible_move[possible_move.size() - 1] << "\n";
	}
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
		difficulty = 3;
	else if (s == "MEDIUM")
		difficulty = 5;
	else if (s == "HARD")
		difficulty = 7;
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

int node_table[8][8] = { { 100, -5, 10, 5, 5, 10, -5, 100 },
{ -5, -45, 1, 1, 1, 1, -45, -5 },
{ 10, 1, 3, 2, 2, 3, 1, 10 },
{ 5, 1, 2, 1, 1, 2, 1, 5 },
{ 5, 1, 2, 1, 1, 2, 1, 5 },
{ 10, 1, 3, 2, 2, 3, 1, 10 },
{ -5, -45, 1, 1, 1, 1, -45, -5 },
{ 100, -5, 10, 5, 5, 10, -5, 100 } };

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
	result() : location(""), value(0) {}
	result(int val) : value(val) {}
};

string convert(int row, int col) {
	char ch1 = 'a' + col;
	char ch2 = '1' + row;
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
		location = convert(i, j);
		if (isLegal(location, board, turn))
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
	if (a.value > b.value){
		return a;
	}
	else
		return b;
}

result minimum(result a, result b){
	if (a.value < b.value){
		return a;
	}
	else
		return b;
}

int evaluate(vector<vector<int>> b, int t, int size){
	int boardstate = 0;
	int my_tiles = 0;
	int opp_tiles = 0;
	if (b[0][0] == t) my_tiles++;
	else if (b[0][0] == -t) opp_tiles++;
	if (b[0][7] == t) my_tiles++;
	else if (b[0][7] == -t) opp_tiles++;
	if (b[7][0] == t) my_tiles++;
	else if (b[7][0] == -t) opp_tiles++;
	if (b[7][7] == t) my_tiles++;
	else if (b[7][7] == -t) opp_tiles++;
	int c = 25 * (my_tiles - opp_tiles);
	for (int i = 0; i < 8; ++i){
		for (int j = 0; j < 8; ++j){
			boardstate = boardstate + (b[i][j] * node_table[i][j]);
		}
	}
	boardstate = boardstate + size + c;
	return boardstate;
}

result minmax(node node1, result alpha, result beta, int height) {
	vector<string> possibleMove = getLegalMove(node1.board, node1.turn);
	if (height == 0) {
		result r;
		r.value = evaluate(node1.board, node1.my_turn, possibleMove.size());   // + possibleMove.size()
		r.location = node1.location;
		return r;
	}
	else {
		vector<node> possible = getChildNode(possibleMove, node1);
		if (node1.turn == node1.my_turn) {
			//result a = alpha;       //-9999
			for (int i = 0; i < possible.size(); i++){
				alpha = maximum(alpha, minmax(possible[i], alpha, beta, height - 1));
				if (beta.value <= alpha.value)
					break;
			}
			if (node1.location != "")
				alpha.location = node1.location;
			return alpha;
		}
		else if (node1.turn == -node1.my_turn) {
			//result b = beta;      //9999
			for (int i = 0; i < possible.size(); i++){
				beta = minimum(beta, minmax(possible[i], alpha, beta, height - 1));
				if (beta.value <= alpha.value)
					break;
			}
			if (node1.location != "")
				beta.location = node1.location;
			return beta;
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
	result i = minmax(n, result(-9999), result(9999), game.difficulty);
	string location = i.location;
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
	game.passflag = 0;
	game.move(token);
	win(game);
	if (game.possible_move.size() == 1){
		game.passflag = 1;
		game.turn = -game.turn;
		if (game.display == 1) {
			cout << "PASS!\n";
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
	}
	else { game.print_possible_move(); }
}

void expr(string token, Board& game) {
	if (token.substr(0,4) == "UNDO") {
		game.undo();
	}
	else if (token.substr(0,10) == "DISPLAY ON") {
		game.setdisplay(1);
		game.show();
		game.print_possible_move();
	}
	else if (token.substr(0,11) == "DISPLAY OFF") {
		game.setdisplay(0);
	}
	else if (is_move(token) && game.isLegalMove(token))  {
		play(token, game);
		while (game.turn == 1) {
			int displayflag = 0;
			if (game.display == 1){ displayflag = 1; }
			game.setdisplay(0);
			string location = ai_function(game);
			cout << location << endl;
			if (displayflag == 1){
				game.setdisplay(1);
			}
			play(location, game);
		}
	}
	else{
		cout <<"Illegal: " << token << token.length() << endl;
	}
}

string strconvert(string input) {
	string tmp;
	string tmp2 = "";
	stringstream ss;
	ss << input;

	ss >> tmp;
	tmp2 = tmp2 + tmp;
	while (ss >> tmp) {
		tmp2 = tmp2 + " ";
		tmp2 = tmp2 + tmp;
	}
	return tmp2;
}

bool is_int(string s) {	
	for (int i = 1; i < s.length(); i++)
	    if (!isdigit(s[i]))
		    return false;
	return true;
}

string get_location(int othersock) {
	char buffer[255] = { 0 };
	string tmp = "";
	while (read(othersock, buffer, 255)) {
		tmp = buffer;
		if (is_move(tmp.substr(0, 2)))
			return tmp.substr(0, 2);
		if (tmp.substr(0, 4) == "PASS")
			return tmp.substr(0, 4);
		tmp = "";
		bzero(buffer, 255);
	}
}

void remote(string input, Board& game) {
//	game.setdisplay(1);
	string tmp;
	string hostname;
	string oppodifficulty;
	int portnumber;
	stringstream ss;
	ss << input;
	ss >> tmp;
	if (tmp != "REMOTE")
		cout << "REMOTE: " << tmp << endl;

	ss >> hostname;

	ss >> tmp;
	if (is_int(tmp))
		portnumber = atoi(tmp.c_str());
	else
		cout << "portnumber is not a int: " << tmp << endl;

	ss >> tmp;
	if (tmp == "EASY" || tmp == "MEDIUM" || tmp == "HARD")
		game.set_difficulty(tmp);
	else
		cout << "invalid client difficulty: " << tmp << endl;

	ss >> tmp;
	if (tmp == "EASY" || tmp == "MEDIUM" || tmp == "HARD")
		oppodifficulty = "LOCAL " + tmp;
	else
		cout << "invalid server difficulty: " << tmp << endl;


	int othersock;
	struct sockaddr_in serveraddr;
	struct hostent *hostp;
	char buffer[255] = { 0 };

	if ((othersock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		perror("Failed to create othersocket");

	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons((short)portnumber);
	serveraddr.sin_addr.s_addr = inet_addr(hostname.c_str());

	if (serveraddr.sin_addr.s_addr == (unsigned long)INADDR_NONE) {
		hostp = gethostbyname(hostname.c_str());
		if (hostp == (struct hostent *)NULL) {
			perror("Host not found --> ");
		}
		memcpy(&serveraddr.sin_addr,
			hostp->h_addr,
			sizeof(serveraddr.sin_addr));
	}

	int error = connect(othersock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (error == -1)
		perror("Failed to connect");

	cout << "connected to other server\n";

	error = write(othersock, oppodifficulty.c_str(), oppodifficulty.length());
	if (error == -1)
		perror("Failed to write to other server");

	cout << "write difficuty: " << oppodifficulty << endl;

	string location;
	int passflag = 0;

	while (true) {
		if (game.turn == -1) {
			location = ai_function(game);
			write(othersock, location.c_str(), location.length());
			cout << "black: " << location << endl;
			play(location, game);
			if (game.passflag == 1) 
				cout << "white: PASS\n";
		}

		if (game.turn == 1) {
			location = get_location(othersock);			
			if (is_move(location))
				cout << "white: " << location << endl;
			    play(location, game);
				if (game.passflag == 1)
					cout << "black: PASS\n";
		}
	}
	
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s portnumber \n", argv[0]);
		return 1;
	}

    int sock;
    int portnumber = atoi(argv[1]);
    int backlog = 20;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        perror("Failed to create socket");

    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons((short)portnumber);
    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) == -1)
        perror("Failed to bind the socket to port");

    if (listen(sock, backlog) == -1)
        perror("Failed to listen");
    
    cout << "waiting for connection\n";
     
    int acceptsock;
    struct sockaddr_in client;
    
	socklen_t client_len = sizeof(client);
    acceptsock = accept(sock, (struct sockaddr *)&client, &client_len);
    if (acceptsock == -1)
       perror("Failed to accept");

    cout << "connecting with player\n";

	dup2(acceptsock, STDOUT_FILENO);
	
	Board game;
	int flag = 0;
	int error;
	char buffer[255] = { 0 };
	string input;

	write(acceptsock, "WELCOME\n", 8);
	write(acceptsock, "Here are the commands for the Reversi game:\n", 44);
	write(acceptsock, "LOCAL + (EASY, MEDIUM, HARD)\n", 29);
	write(acceptsock, "DISPLAY ON: toggles board display\n", 34);
	write(acceptsock, "UNDO: undo AI's move and your last move. Up to 10 times\n", 56);


	do {
		if ((error = read(acceptsock, buffer, 255)) == -1)
			perror("Failed to read socket");
		input = buffer;
		if (input.substr(0, 5) == "LOCAL") {
			if (input.substr(5, 5) == " EASY") {
				game.set_difficulty("EASY");
				cout << "local game - easy: DISPLAY ON/move/UNDO\n";
			}
			else if (input.substr(5, 7) == " MEDIUM") {
				game.set_difficulty("MEDIUM");
				cout << "local game - medium: DISPLAY ON/move/UNDO\n";
	
			}
			else if (input.substr(5, 5) == " HARD") {
				game.set_difficulty("HARD");
				cout << "local game - hard: DISPLAY ON/move/UNDO\n";
			}
			else {
				cout << "invalid difficulty: " << input << " enter again\n";
				flag = 1;
				continue;
			}
			game.get_legal_moves();
			bzero(buffer, 255);
			while (read(acceptsock, buffer, 255)){
				string tmp = buffer;
				input = strconvert(tmp);
				expr(input, game);
			}
		}
		else if (input.substr(0, 6) == "REMOTE") {
			remote(input, game);
			
		}
		else {
			cout << "Please choose LOCAL or REMOTE first.\n";
			flag = 1;
			continue;
		}

	} while (flag);

	close(acceptsock);
    return 0;
}