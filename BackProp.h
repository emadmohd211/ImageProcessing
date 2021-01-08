/*
 *  BackProp.h
 *  ImageProcessingC
 *
 *
 */

#include <fstream>
#include "BackPropNeuron.h"
#include "typedefin.h"

namespace KimBackProp
{
	
	class CBackProp  
	{
	public:
		// public methods
		bool           Load(FILE * pfData);
		bool           Save(std::ofstream & ofs);
		bool		   GetInfo(FILE *);
		FLOAT          GetOutputPercentage(INT iIndex);
		FLOAT          GetInput(INT iIndex);
		float          GetError(int);
		FLOAT          GetDesiredOutput(INT iIndex);
		INT            GetInputCount();
		bool           Learn();
		bool           FeedForward();
		bool           CalculateError();
		bool           BackPropagate();
		
		FLOAT          GetOutput(INT iIndex);
		bool           SetDesiredOutput(INT iIndex, FLOAT fOutput);
		bool           SetInput(INT iIndex, FLOAT fInput);
		bool           Uninit();
		bool           Init(INT iInputsCount, INT iHiddensCount, INT iHiddens2Count, INT iOutputsCount, FLOAT fLearningRate);
		bool           InitDerWeights();
		bool           UpdateDerWeights();
		float		   Frand();
		
		const _TCHAR * GetLastError();
		
		CBackProp();
		virtual        ~CBackProp();
		
	private:
		
		// private constants
		enum
		{
			ID_INPUTS_LAYER,
			ID_HIDDENS_LAYER,
			ID_HIDDENS2_LAYER,
			ID_OUTPUTS_LAYER
		};
		
		// private methods
		inline bool SaveNeuronData(CBackPropNeuron & bpn, std::ofstream & ofs);
		inline bool LoadNeuronData(CBackPropNeuron & bpn, FILE * pfData);
		inline bool CheckInit();
		bool		GetNeuronInfo(CBackPropNeuron &, FILE *);
		bool        InitArrays();
		//bool        CalculateError();
		//bool        BackPropagate();
		bool        RandomizeWeights();
		bool        InitConnections();
		
		// private members
		FLOAT m_fLearningRate;
		bool  m_bInit;
		
		INT   m_iOutputsCount;
		INT   m_iHiddens2Count;
		INT   m_iHiddensCount;
		INT   m_iInputsCount;
		
		CBackPropNeuron * m_pabpnOutputs;
		CBackPropNeuron * m_pabpnHiddens2;
		CBackPropNeuron * m_pabpnHiddens;
		CBackPropNeuron * m_pabpnInputs;
		
		_TCHAR m_szError[256];
	};
	
};

