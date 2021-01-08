/*
 *  BackPropNeuron.h
 *  ImageProcessingC
 *
 *
 */

#include "typedefin.h"
#include <fstream>
#include "math.h"

namespace KimBackProp
{
	
	class CBackPropNeuron  
	{
	public:
		bool     Load(FILE * pfData);
		bool     Save(std::ofstream & ofs);
		bool	 GetInfo(FILE *);
		
		int      GetConnectionsCount();
		float    GetBiasWeight();
		bool     SetBiasWeight(float fBiasWeight);
		bool     BackPropagate(float fLearningRate);
		bool     UpdateDerWeights(float fLearningRate);
		bool     SetError(float fError);
		float    GetErrorTerm();
		float    GetError();
		float    GetDesiredValue();
		bool     SetDesiredValue(float fDesiredValue);
		float    GetWeight(int iConnectionsIndex);
		bool     SetWeight(int iConnectionsIndex, float fWeight);
		bool     SetDerWeight(int iConnectionsIndex, float fWeight);
		bool     SetDerBiasWeight(float fBiasWeight);
		bool     CalculateOutputError();
		bool     FeedForward();
		bool     AddConnections(int iConnectionsIndex, CBackPropNeuron &bpnNeuron);
		bool     SetValue(float fValue);
		float    GetValue();
		bool     Init(int iID, int iLayer, int iConnections);
		float	 SigmoidActivation(float fNumber);
		CBackPropNeuron();
		virtual  ~CBackPropNeuron();
		
	private:
		
		inline bool CheckInit();
		
		bool        UnInitArrays();
		bool        InitArrays(int iConnections);
		
		CBackPropNeuron ** m_pbpnaConnections;
		
		float m_fBiasWeight;
		float m_fDerBiasWeight;
		float m_fDesiredValue;
		float m_fError;
		float m_fValue;
		float * m_faConnectionsWeights;
		float * m_faConnectionsDerWeights;
		
		int   m_iConnections;
		int   m_iLayer;
		int   m_iID;
		
		bool  m_bInit;
	};
	
};