#include "CNeuralNet.h"

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <limits>

#include <assert.h>

CNeuron::CNeuron() {
}

CNeuron::~CNeuron() {
	delete[] m_w;
}

void CNeuron::setNumOfWeights(int n) {
	m_numOfInputs = n;
	m_w = new real[m_numOfInputs + 1]; /// The +1 part is for the BIAS
	if (!m_w) {
		std::cerr << "Uh oh, no memory for weights..." << std::endl;
		return;
	}
	real sum = 0;
	for (int i = 0; i < m_numOfInputs + 1; ++i)
		sum += m_w[i] = myRand(-1.0, 1.0);
	//std::cout << "Average = " << sum/m_numOfInputs << std::endl;
}

real CNeuron::myRand(real from, real to) {
	real r = (((real) rand()) / RAND_MAX) * (to - from) + from;
	return r;
}

real CNeuron::sigmoid(real x) {
	return 1 / (1 + exp(-x));
}

//real sigmoid(real x)
//{
//	return 1/(1+exp(-x));
//}
real CNeuron::feed(real *inp) {
	real sum = 0;
	for (int i = 0; i < m_numOfInputs; ++i)
		sum += m_w[i] * inp[i];
	sum += m_w[m_numOfInputs] * BIAS;
	return m_activation = sigmoid(sum);
	//return m_activation=1/(1+exp(sum));	//better inline it here...
}

CNeuralNet::CNeuralNet(int numOfInputs, int numOfHiddenNeurons,
		int numOfOutputs) :
		m_numOfInputs(numOfInputs), m_numOfHiddenNeurons(numOfHiddenNeurons), m_numOfOutputs(
				numOfOutputs), m_learningRate((real) LEARNING_RATE), m_trainingCounter(
				0), m_gameCounter(0) {
	m_hiddenNeurons = new CNeuron[numOfHiddenNeurons];
	if (m_hiddenNeurons) {
		for (int i = 0; i < numOfHiddenNeurons; ++i)
			m_hiddenNeurons[i].setNumOfWeights(numOfInputs);
	} else {
		std::cout << "Uh oh, no memory for " << numOfHiddenNeurons
				<< " neurons!" << std::endl;
		return;
	}

	m_outNeurons = new CNeuron[numOfOutputs];
	if (m_outNeurons) {
		for (int i = 0; i < numOfOutputs; ++i)
			m_outNeurons[i].setNumOfWeights(numOfHiddenNeurons);
	} else {
		std::cout << "Uh oh, no memory for " << numOfOutputs << " neurons!"
				<< std::endl;
		return;
	}
	inp2 = new real[numOfHiddenNeurons];
	out = new real[numOfOutputs];
}

CNeuralNet::CNeuralNet(char *filename) {

#ifdef FSTREAM_BINARY
	std::fstream file_op(filename, std::ios::binary | std::ios::in);
#else
	std::fstream file_op(filename,std::ios::in);
#endif

	file_op.precision(std::numeric_limits<real>::digits10);
	file_op >> m_numOfInputs;
	file_op >> m_numOfHiddenNeurons;
	file_op >> m_numOfOutputs;
	file_op >> m_gameCounter;
	file_op >> m_trainingCounter;
	m_hiddenNeurons = new CNeuron[m_numOfHiddenNeurons];
	if (m_hiddenNeurons) {
		for (int i = 0; i < m_numOfHiddenNeurons; ++i)
			m_hiddenNeurons[i].setNumOfWeights(m_numOfInputs);
	} else {
		std::cout << "Uh oh, no memory for " << m_numOfHiddenNeurons
				<< " neurons!" << std::endl;
		return;
	}

	m_outNeurons = new CNeuron[m_numOfOutputs];
	if (m_outNeurons) {
		for (int i = 0; i < m_numOfOutputs; ++i)
			m_outNeurons[i].setNumOfWeights(m_numOfHiddenNeurons);
	} else {
		std::cout << "Uh oh, no memory for " << m_numOfOutputs << " neurons!"
				<< std::endl;
		return;
	}
	inp2 = new real[m_numOfHiddenNeurons];
	out = new real[m_numOfOutputs];
	file_op.close();
	loadNet(filename);
}
CNeuralNet::~CNeuralNet() {
	delete[] m_hiddenNeurons;
	delete[] m_outNeurons;
	delete[] inp2;
	delete[] out;
}

int CNeuralNet::saveNet(char *filename) {
#ifdef FSTREAM_BINARY
	std::fstream file_op(filename, std::ios::binary | std::ios::out);
#else
	std::fstream file_op(filename,std::ios::out);
#endif
	file_op.precision(std::numeric_limits<real>::digits10);
	file_op << m_numOfInputs << std::endl;
	file_op << m_numOfHiddenNeurons << std::endl;
	file_op << m_numOfOutputs << std::endl;
	file_op << m_gameCounter << std::endl;
	file_op << m_trainingCounter << std::endl;
	for (int i = 0; i < m_numOfHiddenNeurons; ++i)
		for (int j = 0; j < m_numOfInputs + 1; ++j)
			file_op << m_hiddenNeurons[i].m_w[j] << std::endl;
	for (int i = 0; i < m_numOfOutputs; ++i)
		for (int j = 0; j < m_numOfHiddenNeurons + 1; ++j)
			file_op << m_outNeurons[i].m_w[j] << std::endl;
	file_op.close();
	return 1;
}

int CNeuralNet::loadNet(char *filename) {
	int numOfInputs, numOfHiddenNeurons, numOfOutputs, trainingCounter;

#ifdef FSTREAM_BINARY
	std::fstream file_op(filename, std::ios::binary | std::ios::in);
#else
	std::fstream file_op(filename,std::ios::in);
#endif
	file_op.precision(std::numeric_limits<real>::digits10);
	file_op >> numOfInputs;
	file_op >> numOfHiddenNeurons;
	file_op >> numOfOutputs;
	file_op >> m_gameCounter;
	file_op >> trainingCounter;
	if (numOfInputs != m_numOfInputs) {
		std::cerr << "Problem in number of inputs when loading weights"
				<< std::endl;
		return 0;
	}
	if (numOfHiddenNeurons != m_numOfHiddenNeurons) {
		std::cerr << "Problem in number of hidden neurons when loading weights"
				<< std::endl;
		return 0;
	}
	if (numOfOutputs != m_numOfOutputs) {
		std::cerr << "Problem in number of outputs when loading weights"
				<< std::endl;
		return 0;
	}
	m_trainingCounter = trainingCounter;

	for (int i = 0; i < m_numOfHiddenNeurons; ++i)
		for (int j = 0; j < m_numOfInputs + 1; ++j)
			file_op >> m_hiddenNeurons[i].m_w[j];
	for (int i = 0; i < m_numOfOutputs; ++i)
		for (int j = 0; j < m_numOfHiddenNeurons + 1; ++j)
			file_op >> m_outNeurons[i].m_w[j];
	file_op.close();
	return m_gameCounter;
}

void CNeuralNet::setGameCounter(int n) {
	m_gameCounter = n;
}

void CNeuralNet::showStats(void) {
	static int lastTrainingCounter = 0;
	static time_t previousTime;

	time_t now = time(&now);
#ifdef GNUBG_LR
	std::cout << "Trained positions: " << m_trainingCounter << " Learning Rate: "<<m_learningRate / pow(m_trainingCounter / 1000.0 + 1.0, 0.3 );
#elif defined(ALKIS_LR)
	std::cout << "Trained positions: " << m_trainingCounter
			<< " Learning Rate: "
			<< m_learningRate / pow(2, m_trainingCounter / 300000000.0);
#else
	std::cout << "Trained positions: " << m_trainingCounter << " Learning Rate: "<<m_learningRate;
#endif
	if (lastTrainingCounter != 0) {
		double elapsed = difftime(now, previousTime);
		std::cout << " - "
				<< (m_trainingCounter - lastTrainingCounter) / elapsed
				<< " tps." << std::endl;
	}
	lastTrainingCounter = m_trainingCounter;
	previousTime = now;
}
real *CNeuralNet::nnFeed(real *inp) {
	int i;

	for (i = 0; i < m_numOfHiddenNeurons; ++i)
		inp2[i] = m_hiddenNeurons[i].feed(inp);
	for (i = 0; i < m_numOfOutputs; ++i)
		out[i] = m_outNeurons[i].feed(inp2);
	return out;
}

/*
 int CNeuralNet::train(real *inp,real *target)
 {
 //real errorSum=0;
 int i,j;

 #ifdef GNUBG_LR
 real learningRate=m_learningRate / pow(m_trainingCounter / 1000.0 + 1.0, 0.3 );
 //std::cout << "Learning rate : " << learningRate << std::endl;
 #elif defined(ALKIS_LR)
 real learningRate=m_learningRate / pow(2,m_trainingCounter / 300000000.0);
 //std::cout << "Learning rate : " << learningRate << std::endl;
 #else
 #define learningRate	m_learningRate
 #endif
 nnFeed(inp);
 //#define DEBUGTRAIN
 #ifdef DEBUGTRAIN
 real beforeError;//[NUM_OUTPUTS];
 real afterError;//[NUM_OUTPUTS];
 beforeError=0.0;
 for(i=0; i<m_numOfOutputs; ++i) 
 beforeError+=(target[i]-out[i])*(target[i]-out[i]);
 #endif
 for(i=0; i<m_numOfOutputs; ++i) {
 real err=(target[i]-out[i])*out[i] * (1 - out[i]);
 m_outNeurons[i].m_error=err;
 for (j=0;  j<m_numOfHiddenNeurons; ++j)
 m_outNeurons[i].m_w[j] += err * learningRate * m_hiddenNeurons[j].m_activation;
 m_outNeurons[i].m_w[j] += err * learningRate * BIAS;
 }

 for (i=0;  i<m_numOfHiddenNeurons; ++i) {
 real err=0.0;
 for(j=0; j<m_numOfOutputs; ++j)
 err += m_outNeurons[j].m_error * m_outNeurons[j].m_w[i];
 err *= m_hiddenNeurons[i].m_activation * (1 - m_hiddenNeurons[i].m_activation);
 if (err) {
 for(j=0; j<m_numOfInputs; ++j) {
 m_hiddenNeurons[i].m_w[j]+= err * learningRate * inp[j];
 }
 m_hiddenNeurons[i].m_w[j]+= err * learningRate * BIAS;
 }
 }
 ++m_trainingCounter;
 #ifdef DEBUGTRAIN
 nnFeed(inp);
 afterError=0.0;
 for(i=0; i<m_numOfOutputs; ++i) {
 afterError+=(target[i]-out[i])*(target[i]-out[i]);
 }
 if (afterError>beforeError)
 return 1;
 #endif
 return 0;
 }
 */

/* From original code*/
int CNeuralNet::train(real *inp, real *target) {
	//real errorSum=0;
	int i, j;

#ifdef GNUBG_LR
	real learningRate=m_learningRate / pow(m_trainingCounter / 1000.0 + 1.0, 0.3 );
	//std::cout << "Learning rate : " << learningRate << std::endl;
#elif defined(ALKIS_LR)
	real learningRate = m_learningRate
			/ pow(2, m_trainingCounter / ((real) 300000000.0));
	//std::cout << "Learning rate : " << learningRate << std::endl;
#else
#define learningRate	m_learningRate
#endif

	nnFeed(inp);
	for (i = 0; i < m_numOfOutputs; ++i) {
		real err = (target[i] - out[i]) * out[i] * (1 - out[i]);
		//errorSum += (target[i]-out[i]) * (target[i]-out[i]);
		m_outNeurons[i].m_error = err;
		//std::cout << "Out["<< i << "]=" << out[i] << " Error=" << err<< std::endl;
		for (j = 0; j < m_numOfHiddenNeurons; ++j)
			m_outNeurons[i].m_w[j] += err * learningRate
					* m_hiddenNeurons[j].m_activation;
		m_outNeurons[i].m_w[j] += err * learningRate * BIAS;
	}

	for (i = 0; i < m_numOfHiddenNeurons; ++i) {
		real err = 0;
		for (j = 0; j < m_numOfOutputs; ++j)
			err += m_outNeurons[j].m_error * m_outNeurons[j].m_w[i];
		err *= m_hiddenNeurons[i].m_activation
				* (1 - m_hiddenNeurons[i].m_activation);
		for (j = 0; j < m_numOfInputs; ++j) {
			m_hiddenNeurons[i].m_w[j] += err * learningRate * inp[j];
		}
		m_hiddenNeurons[i].m_w[j] += err * learningRate * BIAS;
	}
	++m_trainingCounter;
	return 0;
}

/**/
int CNeuralNet::newtrain(real *inp, real *target) {
	static real delta_hidden[NUM_NEURONS + 1];
	static real delta_outputs[NUM_OUTPUTS];

#ifdef GNUBG_LR
	real learningRate=m_learningRate / pow(m_trainingCounter / 1000.0 + 1.0, 0.3 );
	//std::cout << "Learning rate : " << learningRate << std::endl;
#elif defined(ALKIS_LR)
	real learningRate = m_learningRate
			/ pow(2, m_trainingCounter / ((real) 300000000.0));
	//std::cout << "Learning rate : " << learningRate << std::endl;
#else
#define learningRate	m_learningRate
#endif

	nnFeed(inp);

//#define DEBUGTRAIN
#ifdef DEBUGTRAIN
	real beforeError; //[NUM_OUTPUTS];
	real afterError;//[NUM_OUTPUTS];
	beforeError=0.0;
	for(int i=0; i<m_numOfOutputs; ++i)
	beforeError+=(target[i]-out[i])*(target[i]-out[i]);
#endif

	// Get the delta value for the output layer
	for (int i = 0; i < NUM_OUTPUTS; ++i)
		delta_outputs[i] = out[i] * (((real) 1.0) - out[i])
				* (target[i] - out[i]);

	// Get the delta value for the hidden layer
	for (int i = 0; i < NUM_NEURONS + 1; ++i) {
		real error = 0.0;
		for (int j = 0; j < NUM_OUTPUTS; ++j) {
			error += m_outNeurons[j].m_w[i] * delta_outputs[j];
		}
		delta_hidden[i] = m_hiddenNeurons[i].m_activation
				* (((real) 1.0) - m_hiddenNeurons[i].m_activation) * error;
	}

	// Now update the weights between hidden & output layer
	for (int i = 0; i < NUM_OUTPUTS; ++i) {
		int j;
		for (j = 0; j < NUM_NEURONS; ++j) {
			//use momentum (delta values from last pass),
			//to ensure moved in correct direction
			m_outNeurons[i].m_w[j] += learningRate * delta_outputs[i]
					* m_hiddenNeurons[j].m_activation;
		}
		m_outNeurons[i].m_w[j] += learningRate * delta_outputs[i] * BIAS;
	}
	// Now update the weights between input & hidden layer
	for (int i = 0; i < NUM_NEURONS; i++) {
		int j;
		for (j = 0; j < NUM_INPUTS; j++) {
			//use momentum (delta values from last pass),
			//to ensure moved in correct direction
			m_hiddenNeurons[i].m_w[j] += learningRate * delta_hidden[i]
					* inp[j];
		}
		m_hiddenNeurons[i].m_w[j] += learningRate * delta_hidden[i] * BIAS;
	}

	++m_trainingCounter;
#ifdef DEBUGTRAIN
	nnFeed(inp);
	afterError=0.0;
	for(int i=0; i<m_numOfOutputs; ++i) {
		afterError+=(target[i]-out[i])*(target[i]-out[i]);
	}
	if (afterError>beforeError)
	return 1;
#endif
	return 0;
}

/* The betas are from gnubg */
#define BETA_HIDDEN 0.1f
#define BETA_OUTPUT 1.0f

int CNeuralNet::newtrain2(real *inp, real *target) {
	/* This one is adapted from GNUBG	*/
	int i, j;

#ifdef GNUBG_LR
	real learningRate=m_learningRate / pow(m_trainingCounter / 1000.0 + 1.0, 0.3 );
	//std::cout << "Learning rate : " << learningRate << std::endl;
#elif defined(ALKIS_LR)
	real learningRate = m_learningRate
			/ pow(2, m_trainingCounter / ((real) 300000000.0));
	//std::cout << "Learning rate : " << learningRate << std::endl;
#else
#define learningRate	m_learningRate
#endif
	//real ar[NUM_NEURONS];

	nnFeed(inp);
	//for(i=0; i<NUM_NEURONS; ++i)
	//	ar[i]=sigmoid(-m_hiddenNeurons[i].m_w[NUM_INPUTS]);
//define DEBUGTRAIN
#ifdef DEBUGTRAIN
	real beforeError;	//[NUM_OUTPUTS];
	real afterError;//[NUM_OUTPUTS];
	beforeError=0.0;
	for(int i=0; i<m_numOfOutputs; ++i)
	beforeError+=(target[i]-out[i])*(target[i]-out[i]);
	std::cout << "[" << out[0] << "/" << out[1] << "/" << out[2] << "] -> [" << target[0] << "/" << target[1] << "/" << target[2] << "]" << std::endl;
#endif
	/* Calculate error at output nodes */
	for (i = 0; i < NUM_OUTPUTS; ++i)
		m_outNeurons[i].m_error = (target[i] - out[i]) * out[i]
				* (1 - out[i]) /* * BETA_OUTPUT */;

	/* Calculate error at hidden nodes */
	for (i = 0; i < NUM_NEURONS; ++i)
		m_hiddenNeurons[i].m_error = 0.0;

	for (i = 0; i < NUM_OUTPUTS; ++i)
		for (j = 0; j < NUM_NEURONS; ++j)
			m_hiddenNeurons[j].m_error += m_outNeurons[i].m_error
					* m_outNeurons[i].m_w[j];
	for (i = 0; i < NUM_NEURONS; ++i)
		m_hiddenNeurons[i].m_error *=
				/*BETA_HIDDEN **/m_hiddenNeurons[i].m_activation
						* (1 - m_hiddenNeurons[i].m_activation);
	//m_hiddenNeurons[i].m_error *= BETA_HIDDEN * ar[i] * (1 - ar[i]);

	/* Adjust weights at output nodes */
	for (i = 0; i < NUM_OUTPUTS; ++i) {
		for (j = 0; j < NUM_NEURONS; ++j)
			m_outNeurons[i].m_w[j] += learningRate * m_outNeurons[i].m_error
					* m_hiddenNeurons[j].m_activation;
		m_outNeurons[i].m_w[j] += learningRate * m_outNeurons[i].m_error * BIAS;
	}

	/*Adjust weights at hidden nodes */
	for (i = 0; i < NUM_INPUTS; ++i)
		if (inp[i] == 1.0)
			for (j = 0; j < NUM_NEURONS; ++j)
				m_hiddenNeurons[j].m_w[i] += learningRate
						* m_hiddenNeurons[j].m_error;
		else if (inp[i])
			for (j = 0; j < NUM_NEURONS; ++j)
				m_hiddenNeurons[j].m_w[i] += learningRate
						* m_hiddenNeurons[j].m_error * inp[i];
	for (j = 0; j < NUM_NEURONS; ++j)
		m_hiddenNeurons[j].m_w[i] += learningRate * m_hiddenNeurons[j].m_error
				* BIAS;
	++m_trainingCounter;
#ifdef DEBUGTRAIN
	nnFeed(inp);
	std::cout << " After [" << out[0] << "/" << out[1] << "/" << out[2] << "] -> [" << target[0] << "/" << target[1] << "/" << target[2] << "]" << std::endl;
	afterError=0.0;
	for(int i=0; i<m_numOfOutputs; ++i) {
		afterError+=(target[i]-out[i])*(target[i]-out[i]);
	}
	if (afterError>beforeError)
	return 1;
#endif
	return 0;
}
