#ifndef TYPES_H_
#define TYPES_H_

typedef float real;

#define LEARNING_RATE	0.1
//#define DECREASE_LEARNING_RATE 

//#define TESAURO_INPUT_ENCODING
//499 Current net vs bench-net: 551/586( moves 45616 )
//499 Current net vs bench-net: 581/566 (ppg 0.581)

//#define SIMPLE_INPUT_ENCODING
//499 Current net vs bench-net: 653/472( moves 45310 )

//#define INTERMEDIATE_INPUT_ENCODING
//499 Current net vs bench-net: 885/357( moves 45432 )
//499 Current net vs bench-net: 882/375 (ppg 0.882)
//320000 499 Current net vs bench-net: 1001/301 (ppg 1.001)
//330000 499 Current net vs bench-net: 957/337 (ppg 0.957)
//340000 499 Current net vs bench-net: 952/342 (ppg 0.952)
//400000 499 Current net vs bench-net: 987/310 (ppg 0.987)
//500000 499 Current net vs bench-net: 980/313 (ppg 0.98)
//600000 499 Current net vs bench-net: 967/317 (ppg 0.967)
//700000 499 Current net vs bench-net: 1016/304 (ppg 1.016)
//800000 499 Current net vs bench-net: 964/318 (ppg 0.964)
//2020000 499 Current net vs bench-net: 1178/212 (ppg 1.178)

#define PLAKOTO1_INPUT_ENCODING

#define BIAS	(-1)

#ifdef SIMPLE_INPUT_ENCODING
#define NUM_INPUTS	98
#endif

#ifdef INTERMEDIATE_INPUT_ENCODING
#define NUM_INPUTS	146
#endif

#ifdef TESAURO_INPUT_ENCODING
#define NUM_INPUTS 194
#endif

#ifdef PLAKOTO1_INPUT_ENCODING
#define NUM_INPUTS	244
//it was 242 but I added the two mana inputs ....
#endif

#define NUM_OUTPUTS 3
#define NUM_NEURONS 80

#define OUT_WIN			0
#define OUT_WINDOUBLE	1
#define OUT_LOSEDOUBLE	2

#define PLAKOTO
#define MANAWINS
//#define GNUBG_LR
#define ALKIS_LR

#define FSTREAM_BINARY

/*
 ** Original sizeof(struct _move)  = 252 bytes
 ** Reducing the int moves[8] thingie... **
 **
 ** We have 24 positions, say 25 with bar. We need 5 bits. (covers up to 32)
 ** We need a 'from' and a 'to' for the move, so 10 bits per move.
 ** Add one bit for a capture or hit move. Ok, 11 bits per move.
 ** Max we'll have 4 moves, so 4x11 = 44 bits for all the moves
 ** 
 ** Let's put the 'partialmoves' which holds the number of the moves in there...
 ** We can have 0-4 partialmoves so...3 bits will do
 ** 44 + 3 = 47 bits    (Original size: 9x32 = 288 bits)
 ** 17 bits spare (till 64) more than enough to cover cPips...but I am not convinced we should save that.
 ** 
 ** The int Board[2][25] can turn to char Board[2][25] -> Save 50x4-50x1 = 150 bytes
 ** 
 ** Current sizeof(struct __move) = 72 bytes
 **
 ** TO THINK
 ** (Like todo)
 ** 
 ** char has 8 bits which can hold 15 (4 bits) + 15 (4 bits)...
 ** We have no space for Capture. (plakoto)
 ** We could go with
 ** char Board[25] + 6 bytes for 24 bit Captures for one side and 24 bit Captures for the other side
 ** char Board[25+6]
 ** brings size to 56 bytes (and not 72-19=53 as expected, probably alignment issues)
 ** 
 */

typedef int move_int;

struct _move {
	move_int moves[8];				//(from,to) pairs of moves
	move_int partialMoves;			//number of moves
	move_int cPips;					//sum of all the moves
	real score[NUM_OUTPUTS]; 		//score from our neural network
	move_int Board[2][25];			//board
};

struct __move {
	long long moves;
	real score[NUM_OUTPUTS];
	char Board[25 + 6];
};

int RollDice(void);
void InitBoard(int anBoard[2][25]);
void GenerateMoves(int anBoard[2][25], int dice1, int dice2);
void BenchGenerateMoves(int anBoard[2][25], int dice1, int dice2);
int ActualGenerateMoves(int anBoard[2][25], int d[], int start, int cPip,
		int depth, int anMoves[]);
int ApplySubMove(int anBoard[2][25], const int iSrc, const int nRoll);
int LegalMove(int anBoard[2][25], int iSrc, int nPips);
void SaveMoves(int cMoves, int cPip, int anMoves[], int anBoard[2][25]);
real *GetScore(int anBoard[2][25]);
real *ConvertToInputVector(int anBoard[2][25]);
void InvertEvaluation(real ar[ NUM_OUTPUTS]);
void SwapSides(int anBoard[2][25]);
void printMove(struct _move move);
void printAllMoves(int d1, int d2);
int GameOver(int anBoard[2][25], real ar[NUM_OUTPUTS]);
void printBoard(int anBoard[2][25], int step);
void printFevga(int anBoard[2][25], int step);
void printPlakoto(int anBoard[2][25], int step);
void playAgainstHuman(void);
int getMove(void);

#endif /*TYPES_H_*/
