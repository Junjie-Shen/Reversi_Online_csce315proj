import java.util.*;
import java.io.*;
import java.lang.Math;



public class gameMech   {
	
    public static int DIREC[][] = { { 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 }, { -1, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 } };

    public static int gameboard[][] = new int[8][8];
    public static Stack<Integer> turntrack = new Stack<Integer>();
    public static Vector<int[][]> undo_list = new Vector<int[][]>();
    public static Vector possible_move = new Vector();
    public static int turn;
    public static int black_score;
    public static int white_score;
    public static int undo_counter;
    public static int difficulty;
    public static int display;

    gameMech() {
    	turn = -1;
    	black_score = 0;
    	white_score = 0;
    	undo_counter = 0;
    	difficulty = 0;
    	display = 0;
    	gameboard[3][3] = gameboard[4][4] = 1;
		gameboard[3][4] = gameboard[4][3] = -1;
    }

    public static void setdisplay(int a) {
    	display = a;
    }

    public static void print_possible_move(){
        System.out.print("Possible moves: ");
        
        for (int i = 1; i < possible_move.size(); ++i){
        	String s = possible_move.elementAt(i).toString();
		    System.out.print(s+", ");
	    }
	    System.out.println();
    }


    public static boolean is_move(String token) {
	    return (token.charAt(0) >= 'a' && token.charAt(0) <= 'h'
			 && token.charAt(1) >= '1' && token.charAt(1) <= '8');
}

    public static String convertInt (int number) {
        String s = "" + number;
        return s;
    }

    public static boolean on_board(int x, int y) {
    	if (x < 0 || x > 7 || y < 0 || y > 7)
		    return false;
	    else
		    return true;
    }

    public static boolean isLegalMove(String location, int [][] gameboard, int turn) {
        int row = location.charAt(1) - '1';                                      // the coordinate 
		int col = location.charAt(0) - 'a';
		int x = 0;                                                        // the increment
		int y = 0;
		if (!on_board(col, row)){                                         //given location is not on the game board
			return false;
		}
		if (gameboard[row][col] == 0){
			int directioncounter = 1;
			Boolean directioncheck = false;
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
					if (gameboard[row + y][col + x] == turn) {
						return true;
					}
				}
				directioncheck = false;
			}
		}
		return false;
	}

    public static void display_score() {
	    System.out.println("White: " + white_score);
	    System.out.println("Black: " + black_score);
    }

    public static void get_legal_moves() {
        white_score = 0;
        black_score = 0;
        Vector<String> legal_moves = new Vector<String>();
        legal_moves.add("no moves. :(");
        String location;
        for (int i = 0; i < 8; i++)
	        for (int j = 0; j < 8; j++) {
	        	if (gameboard[i][j] == 1)
	        		white_score++;
	        	if (gameboard[i][j] == -1)
	        		black_score++;
	            location = convert(i,j);
	            if (isLegalMove(location, gameboard, turn) )
	                legal_moves.add(location); 
	        }
        possible_move = legal_moves;
    }



    public static void set_difficulty(String s) {
	    if (s == "EASY")
		    difficulty = 3;
	    else if (s == "MEDIUM")
		    difficulty = 5;
	    else if (s == "HARD")
		    difficulty = 7;
	    else
		    System.out.println("invalid difficulty: " + s);
    }

    public static int previous_turn() {
	    if (turntrack.size() > 0) {
		    int p = turntrack.pop();
		    return p;
	    }
	    else
		    return 0;
    }

    public static void printIt (int[] [] x)  {
      int i, j;

      for (i = 0; i < x.length; i++)     {
        System.out.print ("\n");
        for (j = 0; j < x[0].length; j++)
          System.out.printf (" %7d ", x[i][j]);
      }
      System.out.println("");
    }
    
    public static void undo() {
        if (undo_counter < 10) {
		    if (undo_list.size() != 0) {      //undo_list contains some previous turns
			    for (int i = 0; i < 8; i++)
			    	for (int j = 0; j < 8; j++)
			    		gameboard[i][j] = undo_list.lastElement()[i][j];
			    undo_list.remove(undo_list.size()-1);
			    if (previous_turn() == 1) {
				    undo();
			    }
			    else {              //undo_list is empty
				    turn = -1;
				    undo_counter++;
				    show();
				    System.out.println((10 - undo_counter) + " UNDO remain");
			    }
		    }
		    else
			    System.out.println("no more undo steps");
	    }
	    else
		    System.out.println("at most undo 10 times");
    }
    
    public static void move(String location) {
        int row = location.charAt(1) - '1';                                      // the coordinate 
		int col = location.charAt(0) - 'a';
		int x = 0;                                                        // the increment
		int y = 0;
        
        int[][] temp = new int[8][8];	
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++)
                temp[i][j] = gameboard[i][j]; 

        turntrack.push(turn);
		undo_list.add(temp);
//		System.out.println(undo_list.size());
//		int it = undo_list.size()-1;
//		printIt(undo_list.get(it));
		gameboard[row][col] = turn;

		int directioncounter = 1;
	    boolean directioncheck = false;
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

				    for (int j = 1; j < Math.max(Math.abs(x), Math.abs(y)); ++j){
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

    public static char pieceColor(int pieceNumber) {
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
   
    public static void show(){
    	if (display == 1) { 
	        System.out.println("  _ _ _ _ _ _ _ _");
	        for (int i = 0; i < 8; ++i){
		        System.out.println(i + 1 + "|" + pieceColor(gameboard[i][0]) + "|" + pieceColor(gameboard[i][1])
		   	     + "|" + pieceColor(gameboard[i][2]) + "|" + pieceColor(gameboard[i][3])
			     + "|" + pieceColor(gameboard[i][4]) + "|" + pieceColor(gameboard[i][5]) + "|"
			     + pieceColor(gameboard[i][6]) + "|" + pieceColor(gameboard[i][7]) + "|");
	        }
	        System.out.println("  a b c d e f g h");
	        display_score();
	        System.out.print("It is your turn: ");
	        if (turn == 1){
		        System.out.println("White.");
	        }
	        else if (turn == -1){
		        System.out.println("Black.");
	        }
        }
    }

    public static void win() {
        if ((black_score + white_score == 64) || (black_score == 0) || (white_score == 0)) {
		    System.out.println("Game Over!");
		    System.out.println("BLACK VS WHITE: " + black_score + " : " + white_score);

		    if (black_score == 0){
			    System.out.println("White Wins!");
		    }
		    if (white_score == 0){
			    System.out.println("Black Wins!");
		    }
		    if ((black_score + white_score == 64) && black_score > white_score)
		    {
			    System.out.println("Black Wins!");
		    }
		    if ((black_score + white_score == 64) && white_score > black_score)
		    {
			    System.out.println("White Wins!");
		    }
		    if ((black_score + white_score == 64) && white_score == black_score){
			    System.out.println("DRAW!");
		    }
		    System.exit(0);
	     }   
    }

    public static void play(String location) {
        move(location);
        win();
        if (possible_move.size() == 1){
		    System.out.println("PASS!");
		    turn = -turn;
		    System.out.print("It is now your turn: ");
		    if (turn == 1){
			    System.out.println("White.");
			    get_legal_moves();
			    print_possible_move();
		    }
		    else if (turn == -1){
			    System.out.println("Black.");
			    get_legal_moves();
			    print_possible_move();
		    }
	    }
	    else {print_possible_move(); }
    }

    public static void expr(String token) {
        if (token.equals("UNDO")) {
		    undo();
	    }
	    else if (token.equals("DISPLAY ON")) {
		    setdisplay(1);
		    show();
		    print_possible_move();
	    }
	    else if (token.equals("DISPLAY OFF")) {
            setdisplay(0); 
        }
	    else if (is_move(token) && isLegalMove(token, gameboard, turn))  {
		    play(token);
		    while (turn == 1) {
			    int displayflag = 0;
			    if(display == 1){ displayflag = 1; }
			    setdisplay(0);
			
			    String location = ai_function();

			    System.out.println("move:" + location);
			    if(displayflag == 1){
				    setdisplay(1);
			    }
			    play(location);
		    }
	    }
	    else{
		    System.out.println("Illegal!");
	    }
    }

/* ***************************** A.I. *************************************** */
    
    public static int node_table[][] = { { 100, -5,  10, 5, 5, 10, -5, 100 },
						 {  -5, -45, 1,  1, 1, 1, -45, -5 },
						 {  10,  1,  3,  2, 2, 3,  1,   10 },
						 {   5,  1,  2,  1, 1, 2,  1,   5 },
						 {   5,  1,  2,  1, 1, 2,  1,   5 },
					     {  10,  1,  3,  2, 2, 3,  1,   10 },
						 {  -5,  -45, 1, 1, 1, 1,  -45, -5 },
						 { 100,  -5, 10, 5, 5, 10, -5,   100 } };

     static class node {
	    int board[][] = new int[8][8];
	    int my_turn;
	    int turn;
	    String location;
	    node() {}
	};
	
	//Not sure if this should be public either.
	static class result {
	    String location;
	    int value;  
	    result()  { location = "";value = 0;}
	    result(int val) { location = "";value = val;} 
	}; 


    public static String convert(int row, int col) {
        char ch1 = (char)('a'+col);
        char ch2 = (char)('1'+row);
        String str = "" + ch1+ch2;      
        return str;   
    }

   

  

    public static Vector<String> getLegalMove(int[][] board, int turn) {
	    String location;
	   	Vector<String> legal_moves = new Vector<String>();
	    for (int i = 0; i < 8; i++)
	        for (int j = 0; j < 8; j++) {
	            location = convert(i,j);
	            if (isLegalMove(location, board, turn) )
	                legal_moves.add(location); 
	        }
	    return legal_moves;                         
	}

    public static int get_node_value(String location) {
	    int row = location.charAt(1) - '1';
		int col = location.charAt(0) - 'a';
	    return node_table[row][col];  //Unsure of what to change to make it mesh with the node_table in AI
	}
   
    
    public static int[][] nodeMove(int[][] gameboard, String location, int turn) {
	    int row = location.charAt(1) - '1';
		int col = location.charAt(0) - 'a';
		int x = 0;
		int y = 0;
		turn = gameboard[row][col];

		int directioncounter = 1;
		Boolean directioncheck = false;
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

					for (int j = 1; j < Math.max(Math.abs(x), Math.abs(y)); ++j){
						turn = gameboard[row + (j*p)][col + (j*k)];
					}
				}
			}
			directioncheck = false;
		} 
	    return gameboard;                
	}
	
	public static Vector<node> getChildNode(Vector<String> possibleMove, node n) {
	    node tmp = new node();
	    Vector<node> child = new Vector<node>();
	    for (int i = 0; i < possibleMove.size(); i++) {
	        tmp.board = nodeMove(n.board, possibleMove.get(i), n.turn);
	        tmp.my_turn = n.my_turn;
	        tmp.turn = -n.turn;
	        tmp.location = possibleMove.get(i);
	        child.add(tmp);
	    } 
	    return child;          
	}
	
	public static result maximum(result a, result b){
		if(a.value > b.value){
			return a;
		}
		else 
			return b;
	}
	
	public static result minimum(result a, result b){
		if(a.value < b.value){
			return a;
		}
		else 
			return b;
	}
	
	public static int evaluate(int[][] b, int size){
		int boardstate = 0;
		for (int i = 0; i < 8; ++i){
			for (int j = 0; j < 8; ++j){
				boardstate = boardstate + (b[i][j] * node_table[i][j]);
			}
		}
		boardstate = boardstate + size;
		return boardstate;
	}
	
	public static result minmax(node node1, result alpha, result beta, int height) {
		Vector<String> possibleMove = getLegalMove(node1.board, node1.turn);
		result r = new result();
		if (height == 0) {		
			r.value = evaluate(node1.board, possibleMove.size());   // + possibleMove.size()
			r.location = node1.location;
			return r;
		}
		else {
			Vector<node> possible = getChildNode(possibleMove, node1);
			if (node1.turn == node1.my_turn) {
				//result a = alpha;       //-9999
				for (int i = 0; i < possible.size(); i++){
					alpha = maximum(alpha, minmax(possible.get(i), alpha, beta,  height - 1));
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
					beta = minimum(beta, minmax(possible.get(i), alpha, beta, height - 1));
					if (beta.value <= alpha.value)
						break;
				}
				if (node1.location != "")
					beta.location = node1.location;
				return beta;
			}			
			else {
				System.out.println("turn = " + node1.turn);
				System.exit(-1);
			}		
		}
		return r;
	}
	

    public static String ai_function(){

        node n = new node();
	    n.board = gameboard;
	    n.my_turn = turn;
	    n.turn = turn;
	    n.location = "";
	    result i = minmax(n, new result(-9999), new result(9999), difficulty);
	    String location = i.location;
	    return location;
    }

  /* ***************************** A.I. *************************************** */

    public static void main(String[] args) {
        gameMech game = new gameMech();

        Scanner keyIn = new Scanner (System.in);
        String input;
  //      str = keyIn.nextLine();
        System.out.println("WELCOM");
        System.out.println("LOCAL + (EASY, MEDIUM, HARD)");
        System.out.println("DISPLAY ON");
        System.out.println("UNDO");
        
        boolean flag= false;
        do {
            input = keyIn.nextLine();
            input.trim();
            if (input.substring(0,5).equals("LOCAL")) {
             if (input.substring(5,10).equals(" EASY")) {
                 game.set_difficulty("EASY");
                 System.out.println("local game - easy: DISPLAY ON/move/UNDO");
             }
             else if (input.substring(5,12).equals(" MEDIUM")) {
                 game.set_difficulty("MEDIUM");
                 System.out.println("local game - medium: DISPLAY ON/move/UNDO");
             }
             else if (input.substring(6,10).equals(" HARD")) {
                 game.set_difficulty("HARD");
                 System.out.println("local game - hard: DISPLAY ON/move/UNDO");
             }
             else {
                 System.out.println("invalid difficulty: " + input +" enter again");
                 flag = true; 
                 continue;
             } 
             game.get_legal_moves();
	         while (true){
	         	 input = keyIn.nextLine();
		         game.expr(input);
	         }               
        }
        else if (input.substring(0,6).equals("REMOTE")) {
//           remote();             
        }
        else {
            System.out.println("Please choose LOCAL or REMOTE first.");
            flag = true; 
            continue;    
        }
    } while (flag);

         /*
        String location = "d3";
        boolean bool = game.isLegalMove(location, gameboard, -1);
        System.out.println(bool);
        game.get_legal_moves();
        game.setdisplay(1);
        game.show();
        game.print_possible_move();
        */


    }

}

