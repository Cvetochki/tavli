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
#define NUM_NEURONS 40

#define OUT_WIN			0
#define OUT_WINDOUBLE	1
#define OUT_LOSEDOUBLE	2

#define PLAKOTO
#define MANAWINS
//#define GNUBG_LR
#define ALKIS_LR

#define FSTREAM_BINARY

struct _move {
	int moves[8];
	int cMoves;
	int cPips;
	real score[NUM_OUTPUTS];
	int Board[2][25];
};
#endif /*TYPES_H_*/
