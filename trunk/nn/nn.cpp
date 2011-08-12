#include <iostream>
#include <iomanip>
#include <memory>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <limits>

#include <string.h>

#include "CNeuralNet.h"




struct _move movelist[32000];

int numberOfMoves;
int gMaxMoves;
int gMaxPips;

CNeuralNet *nn, *nnbench;

/* Lets see */
void printMove(struct _move move)
{
	struct _move mymove=move;
	
        for(int i=0; i<mymove.partialMoves; ++i) {
		std::cout << mymove.moves[2*i]+1 << "->";
		if (mymove.moves[2*i+1]>=0)
			std::cout << mymove.moves[2*i+1]+1;
		else
			std::cout << "off";
                if (i!=(mymove.partialMoves-1)) std::cout <<", ";
	}
	std::cout.precision( std::numeric_limits<real>::digits10 );
	std::cout << "[" << mymove.score[0] << "/" << mymove.score[1] << "/" << mymove.score[2] << "]"<< std::endl;
}

void printAllMoves(int d1, int d2)
{
	for(int j=0; j<numberOfMoves; ++j) {
		std::cout <<j+1 << " (" << d1 << "," << d2 <<")   ";
		printMove(movelist[j]);
	}
}

int mycompare(const void *left,const void *right)
{
        static int cc=0;
	
	struct _move *l,*r;
	l=(struct _move *)left;
	r=(struct _move *)right;
	real t=l->score[0]-r->score[0];
	if (t>0) return -1;
	if (t<0) return 1;
	//std::cout << "No shit!"<< ++cc << l->score[0] << r->score[0]<<std::endl;
	t=l->score[1]-r->score[1];
	if (t>0) return -1;
	if (t<0) return 1;
	//std::cout << "   No shit!"<< ++cc << l->score[1] << r->score[1]<<std::endl;
	t=r->score[2]-l->score[2];
	if (t>0) return -1;
	if (t<0) return 1;
        std::cout << "       No shit! "<< ++cc << " " << l->score[2] << " " << r->score[2]<<std::endl;
	return 0;
}

int main(int argc, char **argv)
{
	char buff[80];
	int anBoardTrain[ 2 ][ 25 ],anBoardOld[ 2 ][ 25 ];
	real out[NUM_OUTPUTS];
	int n=0;
	int gameCounter, startGameCounter;
	int upto;
	int benchmode=0;
	
	time_t start,now;

//	std::cout << sizeof(struct _move) << std::endl;
//	std::cout << sizeof(struct __move) << std::endl;
//	std::cin >> n;
//	return 0;

	startGameCounter=gameCounter=0;upto=300000;
	start=time(&start);
	srand((unsigned int) start);
	CNeuralNet ann(NUM_INPUTS,NUM_NEURONS,NUM_OUTPUTS);
	nn=&ann;
	if (argc>=2) {
                if ((startGameCounter=gameCounter=ann.loadNet(argv[1]))) {
			std::cout << "Neural net loaded ok. Resuming from "<< gameCounter << std::endl;
		} else {
			std::cerr << "Could not load \""<<argv[1]<<"\" for weights."<<std::endl;
		}
		if (argc>=3) {
			upto=atol(argv[2]);
		}
		
		if (argc>=4 && !strcmp(argv[3],"-bench"))
			benchmode=1;
		if (argc==4 && !strcmp(argv[3],"-human")) {
			playAgainstHuman();
		}
		//if (!benchmode)
		//	if (upto<300000)
		//		upto=300000;
	}
	
	//real testInput[2]={1,1};
	//real testTarget[2]={.19,.71};
	
	//ann.train(&testInput[0],&testTarget[0]);
	//for(int i=0; i<39*2; ++i)
	//	std::cout << RollDice() << ((i & 1) ? "  " : " ");
	//std::cout << std::endl << "Sizeof(_move) = " << sizeof(_move) << std::endl;
	int oldn=0;
	int done;
	
	if (benchmode) {
		int go;
		int current=0;
		int bench=0;
		CNeuralNet benchann(argv[4]);
		nnbench=&benchann;
		//if (!benchann.loadNet("benchmark-weights")) {
		//	std::cout << "Problem loading benchmark net..."<<std::endl;
		//	return 0;
		//}
		for(int i=0; i<upto/2; ++i) {
			InitBoard(anBoardTrain);
			done=0;
			do {
				int d1=RollDice();
				int d2=RollDice();
				
				GenerateMoves(anBoardTrain,d1,d2);
				
//				if (!(gameCounter % 1000)) {
//					int anBoardTemp[ 2 ][ 25 ];
//					memcpy(anBoardTemp, anBoardTrain, sizeof(anBoardTrain));
//					//printAllMoves(d1,d2);
//					if (n % 2)
//						std::cout <<(n+1)-oldn << " X to play roll ";
//					else
//						std::cout <<(n+1)-oldn << " O to play roll ";
//					std::cout <<d1<<","<<d2<<std::endl;
//					if (!numberOfMoves)
//						std::cout << "Can't move!" << std::endl;
//					else
//						if (!(gameCounter %10000))
//							printAllMoves(d1,d2);
//						else
//							printMove(movelist[0]);
//					printBoard(anBoardTemp,n);
//				}
				
				//Let's play the best move now
				if (numberOfMoves) {
                                        for(int i=0; i<movelist[0].partialMoves; ++i)
						ApplySubMove(anBoardTrain,movelist[0].moves[2*i],movelist[0].moves[2*i]-movelist[0].moves[2*i+1]);
				}
				SwapSides(anBoardTrain);
                                if ((go=GameOver(anBoardTrain,out))) {
					done=1;
                                        if (go!=3) current+=go;
                                        break;
				}
                                d1=RollDice();
                                d2=RollDice();

                                BenchGenerateMoves(anBoardTrain,d1,d2);
                                if (numberOfMoves) {
                                        for(int i=0; i<movelist[0].partialMoves; ++i)
                                                ApplySubMove(anBoardTrain,movelist[0].moves[2*i],movelist[0].moves[2*i]-movelist[0].moves[2*i+1]);
                                }
                                SwapSides(anBoardTrain);
                                if ((go=GameOver(anBoardTrain,out))) {
                                        done=1;
                                        if (go!=3) bench+=go;
                                        break;
                                }

				++n;
			} while (!done);//!EvalOver());
			
			InitBoard(anBoardTrain);
			done=0;
			
			do {
				int d1=RollDice();
				int d2=RollDice();
				
				BenchGenerateMoves(anBoardTrain,d1,d2);
				//Let's play the best move now
				if (numberOfMoves) {
                                        for(int i=0; i<movelist[0].partialMoves; ++i)
						ApplySubMove(anBoardTrain,movelist[0].moves[2*i],movelist[0].moves[2*i]-movelist[0].moves[2*i+1]);
				}
				SwapSides(anBoardTrain);
                                if ((go=GameOver(anBoardTrain,out))) {
					done=1;
                                        bench+=go;
                                        break;
				}

                                d1=RollDice();
                                d2=RollDice();

                                GenerateMoves(anBoardTrain,d1,d2);
                                if (numberOfMoves) {
                                        for(int i=0; i<movelist[0].partialMoves; ++i)
                                                ApplySubMove(anBoardTrain,movelist[0].moves[2*i],movelist[0].moves[2*i]-movelist[0].moves[2*i+1]);
                                }
                                SwapSides(anBoardTrain);
                                if ((go=GameOver(anBoardTrain,out))) {
                                        done=1;
                                        current+=go;
                                        break;
                                }
				++n;
			} while (!done);//!EvalOver());
			std::cout << i <<" Current net vs bench-net: "<< current << "/" << bench << " (ppg "<<((float)current-bench)/((i+1)*2)<<")"<< std::endl;
		}
		return 0;
	}
	while (gameCounter<=upto) {
		InitBoard(anBoardTrain);
		//out[0]=0.5;out[1]=out[2]=0;
		//ann.train(ConvertToInputVector(anBoardTrain),&out[0]);
		done=0;
		do {
			int d1=RollDice();
			int d2=RollDice();
			memcpy(anBoardOld, anBoardTrain, sizeof(anBoardOld));
			GenerateMoves(anBoardTrain,d1,d2);

#define DOPRINT
#ifdef DOPRINT
			if (!(gameCounter % 1000)) {
				int anBoardTemp[ 2 ][ 25 ];
				memcpy(anBoardTemp, anBoardTrain, sizeof(anBoardTrain));
				//printAllMoves(d1,d2);
				if (n % 2)
					std::cout <<(n+1)-oldn << " X to play roll ";
				else
					std::cout <<(n+1)-oldn << " O to play roll ";
				std::cout <<d1<<","<<d2<<std::endl;
				if (!numberOfMoves)
					std::cout << "Can't move!" << std::endl;
				else
					if (!(gameCounter %10000))
						printAllMoves(d1,d2);
				else
					printMove(movelist[0]);
				printBoard(anBoardTemp,n);

			}
#endif
			//Let's play the best move now
			if (numberOfMoves) {
                                for(int i=0; i<movelist[0].partialMoves; ++i)
					ApplySubMove(anBoardTrain,movelist[0].moves[2*i],movelist[0].moves[2*i]-movelist[0].moves[2*i+1]);
			}
			SwapSides(anBoardTrain);
			
			/*if (GameOver(anBoardTrain,out)) {
				done=1;
			} else {
				real *r=GetScore(anBoardTrain);
				for(int i=0; i<NUM_OUTPUTS; ++i)
					out[i]=r[i];
			}*/
			
			real *r=GetScore(anBoardTrain);
			for(int i=0; i<NUM_OUTPUTS; ++i)
				out[i]=r[i];
			if (GameOver(anBoardTrain,out))
				done=1;
			
			
			InvertEvaluation(out);
#ifdef DOPRINT
			if (!(gameCounter % 1000)) {
				std::cout << "Reverse [" << out[0] << "/" << out[1] << "/" << out[2] << "]"<< std::endl;
			}
#endif
			ann.train(ConvertToInputVector(anBoardOld),&out[0]);
			++n;
//			if (n && 1023) {
//				std::cout << n << "\r";
//				std::flush(std::cout);
//			}
		} while (!done);//!EvalOver());
                if (!(gameCounter % 1000)) ann.showStats();
		now=time(&now);
		double elapsed=difftime(now,start);
                int ng = gameCounter-startGameCounter;
                double eta = (upto-gameCounter)*elapsed/ng;
                int etaHours = (int) (eta/3600);
                int etaMinutes = (int) ((eta-etaHours*3600)/60);
                std::cout << "Game #" << gameCounter << " took " << (n-oldn+1) << " moves... ("<<(n/elapsed)<<" moves/sec ETA: "<<etaHours << ":"<< etaMinutes <<")\r";std::flush(std::cout);
                oldn=n;
		if ((!(gameCounter %100000) && gameCounter!=startGameCounter) || !gameCounter) {
			ann.setGameCounter(gameCounter);
#ifdef GNUBG_LR
			char *lr="GNUBG_LR";
#elif defined(ALKIS_LR)
			char *lr="ALKIS_LR";
#else
			char *lr="CONST_LR";
#endif
#ifdef FEVGA
			sprintf(buff,"weights-%d",gameCounter);
#endif
#ifdef MANAWINS
			char *mana="-manaWins-";
#else
			char *mana="";
#endif
#ifdef PLAKOTO
			sprintf(buff,"plakoto-%s-%s%d-weights-%d",lr,mana,NUM_NEURONS,gameCounter);
#endif
			ann.saveNet(buff);
			std::cout << std::endl <<std::endl<<"Saved \""<<buff<<"\"" <<std::endl;

		}
		++gameCounter;
	}
	
//	int d1=RollDice();
//	int d2=RollDice();
//	std::cout << "Going to find moves for (" << d1 << "," << d2 <<")" << std::endl;
//	GenerateMoves(theBoard,d1,d2);
//	std::cout << "1\r";std::flush(std::cout);
//	//sleep(2);
	std::cout << "Ok\n";
	return 0;
}


void playAgainstHuman(void)
{
	int n=0;
	int anBoardTrain[ 2 ][ 25 ];
	int anBoardTemp[ 2 ][ 25 ];
	real out[3];
	int themove;
	int done=0;
	
	InitBoard(anBoardTrain);
	
	do {
		int d1=RollDice();
		int d2=RollDice();
		GenerateMoves(anBoardTrain,d1,d2);
		
		
		std::cout <<d1<<","<<d2<<std::endl;
		if (!numberOfMoves)
			std::cout << "I can't move!" << std::endl;
		else
			printAllMoves(d1,d2);
		
		//Let's play the best move now
		if (numberOfMoves) {
                        for(int i=0; i<movelist[0].partialMoves; ++i)
				ApplySubMove(anBoardTrain,movelist[0].moves[2*i],movelist[0].moves[2*i]-movelist[0].moves[2*i+1]);
		}
		memcpy(anBoardTemp, anBoardTrain, sizeof(anBoardTrain));
		printBoard(anBoardTemp,n);
		++n;
		if (GameOver(anBoardTrain,out)) {
			std::cout << "I won!!!" << std::endl;
			return;
		}
		
		SwapSides(anBoardTrain);
		d1=RollDice();
		d2=RollDice();
		std::cout << "You rolled " << d1 << ", " <<d2 << std::endl;
		GenerateMoves(anBoardTrain,d1,d2);
		if (!numberOfMoves)
			std::cout << "You can't move!" << std::endl;
		else {
			printAllMoves(d1,d2);
			themove=getMove();
                        for(int i=0; i<movelist[themove].partialMoves; ++i)
				ApplySubMove(anBoardTrain,movelist[themove].moves[2*i],movelist[themove].moves[2*i]-movelist[themove].moves[2*i+1]);
		}
		memcpy(anBoardTemp, anBoardTrain, sizeof(anBoardTrain));
		printBoard(anBoardTemp,n);
		if (GameOver(anBoardTrain,out)) {
			std::cout << "You won!!!" << std::endl;
			return;
		}
		SwapSides(anBoardTrain);
		++n;
	} while (!done);//!EvalOver());
}

int getMove(void)
{
	int mov;
	
	do {
		std::cin >>mov;
		--mov;
		//std::cout <<mov<<"/"<<numberOfMoves<<std::endl;
	} while(mov>=numberOfMoves || mov<0);
	return mov;
}
void InitBoard( int anBoard[ 2 ][ 25 ])
{
	int i;
	
	for( i = 0; i < 25; i++ )
		anBoard[ 0 ][ i ] = anBoard[ 1 ][ i ] = 0;
	
//    anBoard[ 0 ][ 5 ] = anBoard[ 1 ][ 5 ] =
//	anBoard[ 0 ][ 12 ] = anBoard[ 1 ][ 12 ] = 5;
//    anBoard[ 0 ][ 7 ] = anBoard[ 1 ][ 7 ] = 3;
//    anBoard[ 0 ][ 23 ] = anBoard[ 1 ][ 23 ] = 2;
	anBoard[ 0 ][ 23 ] = anBoard[ 1 ][ 23 ] = 15;
	//anBoard[1][6]=1;
}

void GenerateMoves(int anBoard[2][25],int dice1,int dice2)
{
	int d[4];
	int moves[8];
	
	numberOfMoves=0; //reset the moves counter
	gMaxMoves=gMaxPips=0;
	d[0]=dice1;
	d[1]=dice2;
	(dice1==dice2) ? d[2]=d[3]=dice1 : d[2]=d[3]=0 ;
	ActualGenerateMoves(anBoard,d,23,0,0,moves);
	if (dice1!=dice2) {
		int t=d[0];
		d[0]=d[1];
		d[1]=t;
		ActualGenerateMoves(anBoard,d,23,0,0,moves);
	}
	qsort(&movelist[0],numberOfMoves,sizeof(struct _move),mycompare);
}

#ifdef FEVGA
int ApplySubMove( int anBoard[ 2 ][ 25 ],const int iSrc, const int nRoll)
{
	int iDest = iSrc - nRoll;
	
	anBoard[ 1 ][ iSrc ]--;
	if( iDest < 0 ) {
		++anBoard[1][24];
		return 0;
	}
	anBoard[ 1 ][ iDest ]++;
	return 0;
}
#endif

#ifdef PLAKOTO
int ApplySubMove( int anBoard[ 2 ][ 25 ],const int iSrc, const int nRoll)
{
	int iDest = iSrc - nRoll;
	
	int t= --anBoard[ 1 ][ iSrc ];
	if (t==64)
		anBoard[ 1 ][ iSrc ]=0;
	if( iDest < 0 ) {
		++anBoard[1][24];
		return 0;
	}
	t= ++anBoard[ 1 ][ iDest ];
	if (t<64 && anBoard[0][23-iDest]==1)
		anBoard[1][iDest]+=64;
	return 0;
}
#endif

#ifdef FEVGA
int LegalMove( int anBoard[ 2 ][ 25 ], int iSrc, int nPips )
{
	int i, nBack = 0, iDest = iSrc - nPips,nForth=0, offs;
	
	//printBoard(anBoard,0);
	if( iDest >= 0 ) { /* Normal Play (not bearing-off */
		for (i=0; i<24; ++i)
			if( anBoard[ 1 ][ i ] > 0 ) {
				nForth = i;
				break;
			}
			//std::cout << nForth <<std::endl;
		if (iDest>11)
			offs=iDest-12;
		else
			offs=12+iDest;
		if (nForth<12)
			return ( !anBoard[ 0 ][ offs ] );
		else
			return iSrc==nForth && ( !anBoard[ 0 ][ offs ] );
	}
	/* otherwise, attempting to bear off */
	for( i = 0; i < 24; i++ )
		if( anBoard[ 1 ][ i ] > 0 )
			nBack = i;
	return ( nBack <= 5 && ( iSrc == nBack || iDest == -1 ) );
}
#endif
#ifdef PLAKOTO
int LegalMove( int anBoard[ 2 ][ 25 ], int iSrc, int nPips )
{
	int i, nBack = 0, iDest = iSrc - nPips, offs;
	
	if (anBoard[0][23-iSrc]>64) //are we captured?
		return 0;				//yes, we can't move...
	if( iDest >= 0 ) { /* Normal Play (not bearing-off */
		offs=23-iDest;
		return ( anBoard[ 0 ][ offs ]<2 );
	}
	/* otherwise, attempting to bear off */
	for( i = 0; i < 24; i++ )
		if( anBoard[ 1 ][ i ] > 0 )
			nBack = i;
	return ( nBack <= 5 && ( iSrc == nBack || iDest == -1 ));
}
#endif

int ActualGenerateMoves(int anBoard[2][25],int d[],int start, int cPip,int depth,int anMoves[])
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
			
			if( ActualGenerateMoves( anBoardNew, d,d[ 0 ] == d[ 1 ] ? i : 23,cPip + d[depth], depth + 1,anMoves) )
				SaveMoves(depth + 1, cPip + d[ depth ], anMoves, anBoardNew);
			
			fUsed = 1;
		}
	return !fUsed;
}

int RollDice(void)
{
	return ((int)(((1.0*rand())/RAND_MAX)*6)+1);
}

//#define SHOWMAXMOVES
#ifdef SHOWMAXMOVES
int g_maxmoves=0;
#endif
int Exists(int anBoard[2][25])
{
	for(int b=0; b<numberOfMoves; ++b)
		if (!memcmp(&anBoard[0][0],&movelist[b].Board[0][0],2*25*sizeof(int))) {
			//for(int i=0; i<25; ++i)
			//	std::cout << anBoard[1][i];
				//std::cout << " - " << 2*25*sizeof(int) << std::endl;
			return 1;
		}
	return 0;
}
void SaveMoves(int cMoves, int cPip, int anMoves[],int anBoard[ 2 ][ 25 ])
{
	real go[NUM_OUTPUTS];
	
	if( cMoves < gMaxMoves || cPip <gMaxPips )
		return;
	
	if( cMoves > gMaxMoves || cPip > gMaxPips ) {
		numberOfMoves = 0;
		gMaxMoves = cMoves;
		gMaxPips = cPip;
	}
	if (Exists(anBoard))
		return;
	memcpy(movelist[numberOfMoves].Board,anBoard,2*25*sizeof(int));
        movelist[numberOfMoves].partialMoves=cMoves;
	movelist[numberOfMoves].cPips=cPip;
	for(int i=0; i<8; ++i)
		movelist[numberOfMoves].moves[i]=anMoves[i];
	real *r = GetScore(anBoard);
	if (GameOver(anBoard,go)) {
		nn->train(ConvertToInputVector(anBoard),&go[0]);
		//r=&go[0];
		//std::cout << "Lets see" << std::endl;
	}
	for(int i=0; i<NUM_OUTPUTS; ++i)
		movelist[numberOfMoves].score[i]=r[i];
	++numberOfMoves;
#ifdef SHOWMAXMOVES
	if (numberOfMoves>g_maxmoves) {
		g_maxmoves=numberOfMoves;
		std::cout << g_maxmoves << std::endl;
	}
#endif
	
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

real *GetScore(int anBoard[ 2 ][ 25 ])
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
	
	real *r=nn->nnFeed(ConvertToInputVector(anBoard));
	return r;
}

real *ConvertToInputVector(int anBoard[2][25])
{
	int i,t;
	
	static real vec[NUM_INPUTS];
	
	
	for(i=0; i<NUM_INPUTS; ++i)
		vec[i]=0.0;
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
                if ((t=anBoard[1][i])) {
			if (t>64) {
				t-=64;
				vec[10*i+4]=1;
			}
			if (t==1) vec[10*i+0]=1;
			if (t==2) vec[10*i+1]=1;
			if (t==3) vec[10*i+2]=1;
                        if (t>3)  vec[10*i+3]=(real)(t-3)/((real)12.0);
		}
                if ((t=anBoard[0][i])) {
			if (t>64) {
				t-=64;
                                vec[10*i+9]=-1;
			}
                        if (t==1) vec[10*i+5]=-1;
                        if (t==2) vec[10*i+6]=-1;
                        if (t==3) vec[10*i+7]=-1;
                        if (t>3)  vec[10*i+8]=-(real)(t-3)/((real)12.0);
		}
	}
	
	t=anBoard[1][23];
	//assert(anBoard[0][0]<64);
	if (t==1 && anBoard[0][0]<64) {
		int cap=0;
		for(int d1=1; d1<7; ++d1)
			for(int d2=1; d2<7; ++d2) {
				if (anBoard[1][23-d1]>1 && anBoard[1][23-d2]>1) continue;
				if (anBoard[0][d1]>0 || anBoard[0][d2]>0) {
					++cap;
					continue;
				}
				if (anBoard[1][23-d1-d2]<=1 && anBoard[0][d1+d2]>0) {
					++cap;
					continue;
				}
				if (d1==d2) {
					if (anBoard[1][23-d1-d1-d1]<=1 && anBoard[0][d1+d1+d1]>0) {
						++cap;
						continue;
					}
                                        if (d1!=6 && anBoard[1][23-d1-d1-d1-d1]<=1 && anBoard[0][d1+d1+d1+d1]>0) {
						++cap;
						continue;
					}
				}
			}
		vec[NUM_INPUTS-4]=cap/((real)36.0);
		
//#define DEBUGMANAPROPABILITY
#ifdef DEBUGMANAPROPABILITY
		if (vec[NUM_INPUTS-4]==1.00) {
			int inp;
			printBoard(anBoard,1);
			std::cout << "MANA PROB: " << vec[NUM_INPUTS-4] << "[" << cap << "]" << std::endl;
			std::cin >> inp;
		}
		assert(vec[NUM_INPUTS-4]<=1.0);
		
#endif
	}
	
	
	t=anBoard[0][23];
	//assert(anBoard[1][0]<64);
	if (t==1 && anBoard[1][0]<64) {
		int cap=0;
		for(int d1=1; d1<7; ++d1)
			for(int d2=1; d2<7; ++d2) {
				if (anBoard[0][23-d1]>1 && anBoard[0][23-d2]>1) continue;
				if (anBoard[1][d1]>0 || anBoard[1][d2]>0) {
					++cap;
					continue;
				}
				if (anBoard[0][23-d1-d2]<=1 && anBoard[1][d1+d2]>0) {
					++cap;
					continue;
				}
				if (d1==d2) {
					if (anBoard[0][23-d1-d1-d1]<=1 && anBoard[1][d1+d1+d1]>0) {
						++cap;
						continue;
					}
                                        if (d1!=6 && anBoard[0][23-d1-d1-d1-d1]<=1 && anBoard[1][d1+d1+d1+d1]>0) {
						++cap;
						continue;
					}
				}
			}
                vec[NUM_INPUTS-3]=-cap/((real)36.0);
#ifdef DEBUGMANAPROPABILITY 
		if (vec[NUM_INPUTS-3]==1.00) {
			int inp;
			printBoard(anBoard,2);
			std::cout << "MANA PROB: " << vec[NUM_INPUTS-3] << "[" << cap << "]" << std::endl;
			std::cin >> inp;
		}
		assert(vec[NUM_INPUTS-3]<=1.0);
#endif
	}
	
#endif
        vec[NUM_INPUTS-2] =  (real) (anBoard[1][24])/((real)15.0);
        vec[NUM_INPUTS-1] =  -(real) (anBoard[0][24])/((real)15.0);
	return &vec[0];
}

void InvertEvaluation(real ar[NUM_OUTPUTS])
{
	real r;
	
	ar[ OUT_WIN ] = ((real)1.0) - ar[ OUT_WIN ];
	
	r = ar[ OUT_WINDOUBLE ];
	ar[ OUT_WINDOUBLE ] = ar[ OUT_LOSEDOUBLE ];
	ar[ OUT_LOSEDOUBLE ] = r;
}

void SwapSides( int anBoard[ 2 ][ 25 ] )
{
	int i, n;
	
	for( i = 0; i < 25; i++ ) {
		n = anBoard[ 0 ][ i ];
		anBoard[ 0 ][ i ] = anBoard[ 1 ][ i ];
		anBoard[ 1 ][ i ] = n;
	}
}

int oldGameOver(int anBoard[ 2 ][ 25 ], real arOutput[NUM_OUTPUTS])
{
	int i, c;
	
	
	for( i = 0; i < 25; i++ )
		if( anBoard[ 0 ][ i ] )
			break;
	
	if( i == 24 ) {
		/* opponent has no pieces on board; player has lost */
		arOutput[ OUT_WIN ] = arOutput[ OUT_WINDOUBLE ] = 0.0;
		
		for( i = 0, c = 0; i < 25; i++ )
			c += anBoard[ 1 ][ i ];
		
		if( c == 15 ) {
			/* player still has all pieces on board; loses gammon */
			arOutput[ OUT_LOSEDOUBLE ] = 1.0;
			return 1;
		}
		arOutput[ OUT_LOSEDOUBLE ] = 0.0;
		return 1;
	}
	
	for( i = 0; i < 25; i++ )
		if( anBoard[ 1 ][ i ] )
			break;
	
	if( i == 24 ) {
		/* player has no pieces on board; wins */
		arOutput[ OUT_WIN ] = 1.0;
		arOutput[ OUT_LOSEDOUBLE ] = 0.0;
		
		for( i = 0, c = 0; i < 25; i++ )
			c += anBoard[ 0 ][ i ];
		
		if( c == 15 ) {
			/* opponent still has all pieces on board; win gammon */
			arOutput[ OUT_WINDOUBLE ] = 1.0;
			return 1;
		}		
		arOutput[ OUT_WINDOUBLE ] = 0.0;
		return 1;
	}
	return 0;
}




int GameOver(int anBoard[ 2 ][ 25 ], real arOutput[NUM_OUTPUTS])
{
#ifdef PLAKOTO
#ifdef MANAWINS
	if( anBoard[0][0]>64 ) {
		arOutput[ OUT_WIN ] = arOutput[ OUT_WINDOUBLE ] = 0.0;
		arOutput[OUT_LOSEDOUBLE]=1.0;
		return 2;
	}
	if ( anBoard[1][0]>64 ) {
		arOutput[ OUT_WIN ] = arOutput[ OUT_WINDOUBLE ] = 1.0;
		arOutput[OUT_LOSEDOUBLE]=0.0;
		return 2;
	}
#else
	if( anBoard[0][0]== 78 && anBoard[1][0]==78 ) {
		arOutput[ OUT_WIN ] = 0.5; not really??
			arOutput[ OUT_WINDOUBLE ] = arOutput[ OUT_LOSEDOUBLE ] = 0.0;
		return 3;
	}
#endif
#endif
	if( anBoard[0][24]== 15 ) {
	/* opponent has no pieces on board; player has lost */
		arOutput[ OUT_WIN ] = arOutput[ OUT_WINDOUBLE ] = 0.0;
		if( !anBoard[1][24]) {
	/* player still has all pieces on board; loses gammon */
			arOutput[ OUT_LOSEDOUBLE ] = 1.0;
			return 2;
		}
		arOutput[ OUT_LOSEDOUBLE ] = 0.0;
		return 1;
	}
	
	if( anBoard[1][24] == 15 ) {
/* player has no pieces on board; wins */
		arOutput[ OUT_WIN ] = 1.0;
		arOutput[ OUT_LOSEDOUBLE ] = 0.0;
		if( !anBoard[0][24]) {
/* opponent still has all pieces on board; win gammon */
			arOutput[ OUT_WINDOUBLE ] = 1.0;
			return 2;
		}
		arOutput[ OUT_WINDOUBLE ] = 0.0;
		return 1;
	}
	if (anBoard[0][24])
		arOutput[ OUT_WINDOUBLE ] = 0.0;
	if (anBoard[1][24])
		arOutput[ OUT_LOSEDOUBLE ] = 0.0;
	
	
	/* gammons must be less than wins */
	if( arOutput[ OUT_WINDOUBLE ] > arOutput[ OUT_WIN ] ) {
		arOutput[ OUT_WINDOUBLE ] = arOutput[ OUT_WIN ];
	}
	
	
	real lose = ((real)1.0) - arOutput[ OUT_WIN ];
	if( arOutput[ OUT_LOSEDOUBLE ] > lose ) {
		arOutput[ OUT_LOSEDOUBLE ] = lose;
	}
	

	static real noise = 1/10000.0f;
	
	for(int i = OUT_WIN; i <= OUT_LOSEDOUBLE; ++i) {
		if( arOutput[i] < noise ) 
			arOutput[i] = 0.0;
		else if ((1-arOutput[i])<noise)
			arOutput[i] = 1.0;
	}

	return 0;
}


void printBoard(int anBoard[2][25],int step)
{
#ifdef PLAKOTO
	printPlakoto(anBoard,step);
#endif
	
#ifdef FEVGA
	printFevga(anBoard,step);
#endif
}

void printFevga(int anBoard[2][25], int step)
{
	char a,b;
	int t;
	
	if (step % 2) {
		a='X';
		b='O';
	} else {
		a='X';
		b='O';
		SwapSides(anBoard);
	}
	
	for(int i=0; i<6; ++i) {
		std::cout << " --";
	}
	std::cout << "| |";
	for(int i=0; i<6; ++i) {
		std::cout << " --";
	}
	std::cout << std::endl;
	
	for(int j=0; j<6; ++j) {
		for(int i=0; i<12; ++i) {
                        if ((t=anBoard[1][12+i])) {
				if (j==0)
					if (t>5)
						std::cout <<std::setw(2)<<t<< " ";
				else
					std::cout << "   ";
				else
					if (t>=j)
						std::cout << " " << a << " ";
				else
					std::cout << "   ";
                        } else if ((t=anBoard[0][i])) {
				if (j==0)
					if (t>5)
						std::cout <<std::setw(2)<<t<< " ";
				else
					std::cout << "   ";
				else
					if (t>=j)
						std::cout << " " << b << " ";
				else
					std::cout << "   ";
			} else
				std::cout << "   ";
			if (i==5) std::cout << "| |";
		}
		std::cout << std::endl;
	}
	for(int i=0; i<6; ++i) {
		std::cout << "   ";
	}
	std::cout << "| |";
	for(int i=0; i<6; ++i) {
		std::cout << "   ";
	}
	std::cout << std::endl;
//	for(int i=0; i<6; ++i) {
//		std::cout << "   ";
//	}
//	std::cout << "| |";
//	for(int i=0; i<6; ++i) {
//		std::cout << "   ";
//	}
//	std::cout << std::endl;
	for(int j=5; j>=0; --j) {
		for(int i=0; i<12; ++i) {
                        if ((t=anBoard[1][11-i])) {
				if (j==0)
					if (t>5)
						std::cout <<std::setw(2)<<t<< " ";
				else
					std::cout << "   ";
				else
					if (t>=j)
						std::cout << " " << a << " ";
				else
					std::cout << "   ";
                        } else if ((t=anBoard[0][23-i])) {
				if (j==0)
					if (t>5)
						std::cout <<std::setw(2)<<t<< " ";
				else
					std::cout << "   ";
				else
					if (t>=j)
						std::cout << " " << b << " ";
				else
					std::cout << "   ";
			} else
				std::cout << "   ";
			if (i==5) std::cout << "| |";
		}
		std::cout << std::endl;
	}
	for(int i=0; i<6; ++i) {
		std::cout << " --";
	}
	std::cout << "| |";
	for(int i=0; i<6; ++i) {
		std::cout << " --";
	}
	
	std::cout << " Ooff: " <<std::setw(2)<< anBoard[0][24];
	std::cout << " Xoff: " <<std::setw(2)<< anBoard[1][24];
	std::cout << std::endl;
}


void printPlakoto(int anBoard[2][25], int step)
{
	char a,b;
	int t;
	int c;
	
	if (step % 2) {
		a='X';
		b='O';
	} else {
		a='X';
		b='O';
		SwapSides(anBoard);
	}
	
	for(int i=0; i<6; ++i) {
		std::cout << " --";
	}
	std::cout << "| |";
	for(int i=0; i<6; ++i) {
		std::cout << " --";
	}
	std::cout << std::endl;
	
	for(int j=0; j<6; ++j) {
		for(int i=0; i<12; ++i) {
			if ((t=anBoard[1][23-i]) && (anBoard[0][i]<64)) {
				if (t>64) {
					t-=64;
					c=1;
				} else
					c=0;
				if (j==0)
					if (t>4)
						std::cout <<std::setw(2)<<t<< " ";
				else
					std::cout << "   ";
				else if (j==1)
					if (c)
						std::cout << " " << b << " ";
				else
					std::cout << " " << a << " ";
				else
					if (t>=(j-c))
						std::cout << " " << a << " ";
				else
					std::cout << "   ";
			}
			if ((t=anBoard[0][i]) && (anBoard[1][23-i]<64)) {
				if (t>64) {
					t-=64;
					c=1;
				} else
					c=0;
				if (j==0)
					if (t>4)
						std::cout <<std::setw(2)<<t<< " ";
				else
					std::cout << "   ";
				else if (j==1)
					if (c)
						std::cout << " " << a << " ";
				else
					std::cout << " " << b << " ";
				else
					if (t>=(j-c))
						std::cout << " " << b << " ";
				else
					std::cout << "   ";
			}
			if ((!anBoard[0][i]) && (!anBoard[1][23-i]))
				std::cout << "   ";
			if (i==5) std::cout << "| |";
		}
		std::cout << std::endl;
	}
	for(int i=0; i<6; ++i) {
		std::cout << "   ";
	}
	std::cout << "| |";
	for(int i=0; i<6; ++i) {
		std::cout << "   ";
	}
	
	std::cout << std::endl;
	
	for(int j=5; j>=0; --j) {
		for(int i=0; i<12; ++i) {
			if ((t=anBoard[1][i]) && (anBoard[0][23-i]<64)) {
				if (t>64) {
					t-=64;
					c=1;
				} else
					c=0;
				if (j==0)
					if (t>4)
						std::cout <<std::setw(2)<<t<< " ";
				else
					std::cout << "   ";
				else if (j==1)
					if (c)
						std::cout << " " << b << " ";
				else
					std::cout << " " << a << " ";
				else
					if (t>=(j-c))
						std::cout << " " << a << " ";
				else
					std::cout << "   ";
			}
			if ((t=anBoard[0][23-i]) && (anBoard[1][i]<64)) {
				if (t>64) {
					t-=64;
					c=1;
				} else
					c=0;
				if (j==0)
					if (t>4)
						std::cout <<std::setw(2)<<t<< " ";
				else
					std::cout << "   ";
				else if (j==1)
					if (c)
						std::cout << " " << a << " ";
				else
					std::cout << " " << b << " ";
				else
					if (t>=(j-c))
						std::cout << " " << b << " ";
				else
					std::cout << "   ";
			}
			if ((!anBoard[0][23-i]) && (!anBoard[1][i]))
				std::cout << "   ";
			if (i==5) std::cout << "| |";

		}
		std::cout << std::endl;
	}
	for(int i=0; i<6; ++i) {
		std::cout << " --";
	}
	std::cout << "| |";
	for(int i=0; i<6; ++i) {
		std::cout << " --";
	}
	
	std::cout << " Off-Board   O:" <<anBoard[0][24];
	std::cout << " X:" << anBoard[1][24];
	std::cout << std::endl;
}
