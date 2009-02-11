#include "types.h"
#include "CNeuralNet.h"
#include <stdlib.h>

extern int numberOfMoves;
extern int gMaxMoves, gMaxPips;
extern struct _move movelist[4000];
extern CNeuralNet *nnbench;

int BenchActualGenerateMoves(int anBoard[2][25],int d[],int start, int cPip,int depth,int anMoves[]);
int LegalMove( int anBoard[ 2 ][ 25 ], int iSrc, int nPips );
int ApplySubMove( int anBoard[ 2 ][ 25 ],const int iSrc, const int nRoll);
void BenchSaveMoves(int cMoves, int cPip, int anMoves[],int anBoard[ 2 ][ 25 ]);
real *BenchGetScore(int anBoard[ 2 ][ 25 ]);
real *BenchConvertToInputVector(int anBoard[2][25]);

int benchmycompare(const void *left,const void *right)
{
	struct _move *l,*r;
	l=(struct _move *)left;
	r=(struct _move *)right;
	real t=l->score[0]-r->score[0];
	if (t>0) return -1;
	if (t<0) return 1;
	return 0;
}

void BenchGenerateMoves(int anBoard[2][25],int dice1,int dice2)
{
	int d[4];
	int moves[8];

	numberOfMoves=0; //reset the moves counter to zero...
	gMaxMoves=gMaxPips=0;
	d[0]=dice1;
	d[1]=dice2;
	(dice1==dice2) ? d[2]=d[3]=dice1 : d[2]=d[3]=0 ;
	BenchActualGenerateMoves(anBoard,d,23,0,0,moves);
	if (dice1!=dice2) {
		int t=d[0];
		d[0]=d[1];
		d[1]=t;
		BenchActualGenerateMoves(anBoard,d,23,0,0,moves);
	}
	qsort(&movelist[0],numberOfMoves,sizeof(struct _move),benchmycompare);
}

int BenchActualGenerateMoves(int anBoard[2][25],int d[],int start, int cPip,int depth,int anMoves[])
{
	int anBoardNew[ 2 ][ 25 ], fUsed=0;
	
    if (depth>3 || !d[depth])
		return true;
	for(int  i = start; i >= 0; --i)
	    if( anBoard[ 1 ][ i ] && LegalMove( anBoard, i, d[depth] ) ) {
			anMoves[ depth * 2 ] = i;
			anMoves[ depth * 2 + 1 ] = i - d[ depth ];
			
			//memcpy(anBoardNew,anBoard, sizeof(anBoardNew));
			for(int iCopy = 0; iCopy < 25; iCopy++ ) {
			    anBoardNew[ 0 ][ iCopy ] = anBoard[ 0 ][ iCopy ];
			    anBoardNew[ 1 ][ iCopy ] = anBoard[ 1 ][ iCopy ];
			}
	    
			ApplySubMove( anBoardNew, i, d[ depth ]);
			
			if( BenchActualGenerateMoves( anBoardNew, d,d[ 0 ] == d[ 1 ] ? i : 23,cPip + d[depth], depth + 1,anMoves) )
			    BenchSaveMoves(depth + 1, cPip + d[ depth ], anMoves, anBoardNew);
				
			fUsed = 1;
	    }
	return !fUsed;
}

void BenchSaveMoves(int cMoves, int cPip, int anMoves[],int anBoard[ 2 ][ 25 ]) 
{

	if( cMoves < gMaxMoves || cPip <gMaxPips )
	    return;

	if( cMoves > gMaxMoves || cPip > gMaxPips ) {
	    numberOfMoves = 0;
		gMaxMoves = cMoves;
		gMaxPips = cPip;
	}
	
	movelist[numberOfMoves].cMoves=cMoves;
	movelist[numberOfMoves].cPips=cPip;
	for(int i=0; i<8; ++i)
		movelist[numberOfMoves].moves[i]=anMoves[i];
	real *r = BenchGetScore(anBoard);
	for(int i=0; i<NUM_OUTPUTS; ++i)
		movelist[numberOfMoves].score[i]=r[i];
	++numberOfMoves;
	//57 move in 10000 game is wrong...
	
//    int i, j;
//    move *pm;
//    unsigned char auch[ 10 ];
//
//	/* Save only legal moves: if the current move moves plays less
//	   chequers or pips than those already found, it is illegal; if
//	   it plays more, the old moves are illegal. */
//	if( cMoves < pml->cMaxMoves || cPip < pml->cMaxPips )
//	    return;
//
//	if( cMoves > pml->cMaxMoves || cPip > pml->cMaxPips )
//	    pml->cMoves = 0;
//	
//	pml->cMaxMoves = cMoves;
//	pml->cMaxPips = cPip;
//    
//    
//    pm = pml->amMoves + pml->cMoves;
//    
//    PositionKey( anBoard, auch );
//    
//    for( i = 0; i < pml->cMoves; i++ )
//		if( EqualKeys( auch, pml->amMoves[ i ].auch ) ) {
//		    if( cMoves > pml->amMoves[ i ].cMoves || cPip > pml->amMoves[ i ].cPips ) {
//				for( j = 0; j < cMoves * 2; j++ )
//				    pml->amMoves[ i ].anMove[ j ] = anMoves[ j ] > -1 ? anMoves[ j ] : -1;
//				if( cMoves < 4 )
//				    pml->amMoves[ i ].anMove[ cMoves * 2 ] = -1;
//				pml->amMoves[ i ].cMoves = cMoves;
//				pml->amMoves[ i ].cPips = cPip;
//		    }
//		    return;
//		}
//    
//    for( i = 0; i < cMoves * 2; i++ )
//		pm->anMove[ i ] = anMoves[ i ] > -1 ? anMoves[ i ] : -1;
//    
//    if( cMoves < 4 )
//		pm->anMove[ cMoves * 2 ] = -1;
//    
//    for( i = 0; i < 10; i++ )
//		pm->auch[ i ] = auch[ i ];
//
//    pm->cMoves = cMoves;
//    pm->cPips = cPip;
//
//    for ( i = 0; i < NUM_OUTPUTS; i++ )
//    	pm->arEvalMove[ i ] = 0.0;
//    
//    pml->cMoves++;
//
//    assert( pml->cMoves < MAX_INCOMPLETE_MOVES );
}

real *BenchGetScore(int anBoard[ 2 ][ 25 ])
{
	// 1 is our, 0 is opponent
	
//	int i,t;
//	real vec[98];
//	
//	for(i=0; i<98; ++i)
//		vec[i]=0;
//	
//	for(i=0; i<24; ++i) {
//		if (t=anBoard[1][i]) {
//		   	vec[4*i+0]=1;
//			if (t>2) 
//				vec[4*i+1]=(real)(t-1)/2.0;
//		}
//		if (t=anBoard[0][i]) {
//			vec[4*i+2]=-1;
//			if (t>2)
//				vec[4*i+3]=-(real)(t-1)/2.0;
//		}
//	}
//	
//    vec[96] =  (real) (anBoard[1][24])/15.0;
//    
//    vec[97] = -(real) (anBoard[0][24])/15.0;
	
	real *r=nnbench->nnFeed(BenchConvertToInputVector(anBoard));
	return r;
}

#undef SIMPLE_INPUT_ENCODING
#undef INTERMEDIATE_INPUT_ENCODING
//#define TESAURO_INPUT_ENCODING
real *BenchConvertToInputVector(int anBoard[2][25])
{
	int i,t;

	static real vec[NUM_INPUTS];

	
	for(i=0; i<NUM_INPUTS; ++i)
		vec[i]=0;
#ifdef SIMPLE_INPUT_ENCODING	
	for(i=0; i<24; ++i) {
		if (t=anBoard[1][i]) {
		   	vec[4*i+0]=1;
			if (t>2) 
				vec[4*i+1]=(real)(t-1)/2.0;
		}
		if (t=anBoard[0][i]) {
			vec[4*i+2]=-1;
			if (t>2)
				vec[4*i+3]=-(real)(t-1)/2.0;
		}
	}
	
#endif
#ifdef INTERMEDIATE_INPUT_ENCODING
	for(i=0; i<24; ++i) {
		if (t=anBoard[1][i]) {
		   	vec[6*i+0]=1;
		   	if (t>1) vec[6*i+1]=1;
			if (t>2) vec[6*i+2]=(real)(t-2)/3.0;
		}
		if (t=anBoard[0][i]) {
			vec[6*i+3]=-1;
			if (t>1) vec[6*i+4]=-1;
			if (t>2) vec[6*i+5]=-(real)(t-2)/3.0;
		}
	}
#endif
#ifdef TESAURO_INPUT_ENCODING
	for(i=0; i<24; ++i) {
		if (t=anBoard[1][i]) {
		   	vec[8*i+0]=1;
		   	if (t>1) vec[8*i+1]=1;
		   	if (t>2) vec[8*i+2]=1;
			if (t>3) vec[8*i+3]=(real)(t-3)/2.0;
		}
		if (t=anBoard[0][i]) {
			vec[8*i+4]=-1;
			if (t>1) vec[8*i+5]=-1;
			if (t>2) vec[8*i+6]=-1;
			if (t>3) vec[8*i+7]=-(real)(t-3)/2.0;
		}
	}
#endif
#ifdef PLAKOTO1_INPUT_ENCODING
	for(i=0; i<24; ++i) {
		if (t=anBoard[1][i]) {
			if (t>64) {
				t-=64;
				vec[10*i+4]=1;
			}
		   	vec[10*i+0]=1;
		   	if (t>1) vec[10*i+1]=1;
		   	if (t>2) vec[10*i+2]=1;
			if (t>3) vec[10*i+3]=(real)(t-3)/((real)2.0);
		}
		if (t=anBoard[0][i]) {
			if (t>64) {
				t-=64;
				vec[10*i+9]=-1;
			}
			vec[10*i+5]=-1;
			if (t>1) vec[10*i+6]=-1;
			if (t>2) vec[10*i+7]=-1;
			if (t>3) vec[10*i+8]=-(real)(t-3)/((real)2.0);
		}
	}
#endif
	vec[NUM_INPUTS-2] =  (real) (anBoard[1][24])/((real)15.0);
    vec[NUM_INPUTS-1] = -(real) (anBoard[0][24])/((real)15.0);
	return &vec[0];
}

