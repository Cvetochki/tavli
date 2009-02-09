#ifndef _CNEURALNET_H_
#define _CNEURALNET_H_

#include "types.h"

class CNeuron
{
	int m_numOfInputs;

public:
	real *m_w;
	real m_activation;
	real m_error;
	
	CNeuron();
	~CNeuron();
	void setNumOfWeights(int n);
	real myRand(real from, real to);
	real feed(real *inp);
	real sigmoid(real x);
};

class CNeuralNet
{
	int	m_numOfInputs;
	int m_numOfHiddenNeurons;
	int m_numOfOutputs;
	CNeuron *m_hiddenNeurons;
	CNeuron *m_outNeurons;
	real *inp2;
	real *out;
	real m_learningRate;
	int m_trainingCounter;
	int m_gameCounter;
public:
	CNeuralNet(int numOfInputs, int numOfHiddenNeurons, int numOfOutputs);
	CNeuralNet(char *filename);
	~CNeuralNet();
	real *nnFeed(real *inp);
	int train(real *inp,real *target);
	int newtrain(real *inp,real *target);
	int newtrain2(real *inp,real *target);
	
	int saveNet(char *filename);
	int loadNet(char *filename);
	void setGameCounter(int n);
	void showStats(void);
};


#endif //_CNEURALNET_H_
