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
	
        movelist[numberOfMoves].partialMoves=cMoves;
	movelist[numberOfMoves].cPips=cPip;
	for(int i=0; i<8; ++i)
            movelist[numberOfMoves].moves[i]=anMoves[i];
	real *r = BenchGetScore(anBoard);
	for(int i=0; i<NUM_OUTPUTS; ++i)
            movelist[numberOfMoves].score[i]=r[i];
	++numberOfMoves;
}

real *BenchGetScore(int anBoard[ 2 ][ 25 ])
{
	// 1 is our, 0 is opponent
        real *r=nnbench->nnFeed(ConvertToInputVector(anBoard));
	return r;
}

