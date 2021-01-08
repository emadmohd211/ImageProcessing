/*
 *  BackProp.cpp
 *  ImageProcessingC
 *
 *  
 *
 */

#include "BackProp.h"


namespace KimBackProp
{
	
	CBackProp::CBackProp() : m_pabpnInputs(NULL), m_pabpnHiddens(NULL), m_pabpnHiddens2(NULL), m_pabpnOutputs(NULL),
	m_bInit(false)
	{
	
	}
	
	CBackProp::~CBackProp()
	{
		Uninit();
	}
	
	// Function name    : CBackProp::GetLastError
	// Description      : Return the last error message
	// Return type      : const _TCHAR *
	const _TCHAR * CBackProp::GetLastError()
	{
		return m_szError;
	}
	
	
	// Function name    : CBackProp::Init
	// Description      : Initialize the class members
	// Return type      : bool 
	// Argument         : INT iInputsCount
	// Argument         : INT iHiddensCount
	// Argument         : INT iHiddens2Count
	// Argument         : INT iOutputsCount
	bool CBackProp::Init(INT iInputsCount, INT iHiddensCount, INT iHiddens2Count, INT iOutputsCount, FLOAT fLearningRate)
	{
		if (m_bInit == true)
		{
			Uninit();
		}
		
		if (iInputsCount <= 0 || iOutputsCount <= 0 || iHiddensCount <= 0 || iHiddens2Count < 0)
		{
			sprintf(m_szError,"Invalid arguments");
			return false;
		}
		
		m_iInputsCount    = iInputsCount;
		m_iHiddensCount   = iHiddensCount;
		m_iHiddens2Count  = iHiddens2Count;
		m_iOutputsCount   = iOutputsCount;
		m_fLearningRate   = fLearningRate;
		
		if (InitArrays() == false)
		{
			return false;
		}
		
		// initialize the neurons
		INT i;
		for (i=0; i<m_iInputsCount; i++)
		{
			if (m_pabpnInputs[i].Init(i,ID_INPUTS_LAYER,0) == false)
			{
				sprintf(m_szError, "Error initializing inputs");
				return false;
			}
		}
		
		for (i=0; i<m_iHiddensCount; i++)
		{
			if (m_pabpnHiddens[i].Init(i, ID_HIDDENS_LAYER, m_iInputsCount) == false)
			{
				sprintf(m_szError, "Error initializing hiddens");
				return false;
			}
		}
		
		for (i=0; i<m_iHiddens2Count; i++)
		{
			if (m_pabpnHiddens2[i].Init(i, ID_HIDDENS2_LAYER, m_iHiddensCount) == false)
			{
				sprintf(m_szError,"Error initializing hiddens2");
				return false;
			}
		}
		
		INT iOutputsConnectionsCount;
		if (m_iHiddens2Count > 0)
		{
			iOutputsConnectionsCount = m_iHiddens2Count;
		}
		else
		{
			iOutputsConnectionsCount = m_iHiddensCount;
		}
		
		for (i=0; i<m_iOutputsCount; i++)
		{
			if (m_pabpnOutputs[i].Init(i, ID_OUTPUTS_LAYER, iOutputsConnectionsCount) == false)
			{
				sprintf(m_szError, "Error initializing outputs");
				return false;
			}
		}
		
		// add the connections
		if (InitConnections() == false)
		{
			return false;
		}
		
		// initialize the weights
		if (RandomizeWeights() == false)
		{
			return false;
		}
		
		m_bInit = true;
		
		return true;
	}
	
	
	// Function name    : CBackProp::Uninit
	// Description      : Uninitialize the class members
	// Return type      : bool 
	bool CBackProp::Uninit()
	{
		if (m_pabpnInputs!= NULL)
		{
			delete [] m_pabpnInputs;
			m_pabpnInputs = NULL;
		}
		
		if (m_pabpnHiddens!= NULL)
		{
			delete [] m_pabpnHiddens;
			m_pabpnHiddens = NULL;
		}
		
		if (m_pabpnHiddens2!= NULL)
		{
			delete [] m_pabpnHiddens2;
			m_pabpnHiddens2 = NULL;
		}
		
		if (m_pabpnOutputs!= NULL)
		{
			delete [] m_pabpnOutputs;
			m_pabpnOutputs = NULL;
		}
		
				
		m_bInit = false;
		
		return true;   
	}
	
	// Function name    : CBackProp::InitConnections
	// Description      : Initialize the connections between the layers of neurons
	// Return type      : bool 
	bool CBackProp::InitConnections()
	{
		INT i;
		INT j;
		INT iConnectionsCount;
		
		for (i=0; i<m_iHiddensCount; i++)
		{
			iConnectionsCount = m_pabpnHiddens[i].GetConnectionsCount();
			
			for (j=0; j<iConnectionsCount; j++)
			{
				if (m_pabpnHiddens[i].AddConnections(j, m_pabpnInputs[j]) == false)
				{
					sprintf(m_szError, "Error adding hiddens' connections");
					return false;
				}
			}
		}
		
		for (i=0; i<m_iHiddens2Count; i++)
		{
			iConnectionsCount = m_pabpnHiddens2[i].GetConnectionsCount();
			
			for (j=0; j<iConnectionsCount; j++)
			{
				if (m_pabpnHiddens2[i].AddConnections(j, m_pabpnHiddens[j]) == false)
				{
					sprintf(m_szError, "Error adding hiddens2' connections");
					return false;
				}
			}
		}
		
		CBackPropNeuron * pabpnOutputsConnections;
		if (m_iHiddens2Count > 0)
		{
			pabpnOutputsConnections = m_pabpnHiddens2;
		}
		else
		{
			pabpnOutputsConnections = m_pabpnHiddens;
		}
		
		for (i=0; i<m_iOutputsCount; i++)
		{
			iConnectionsCount = m_pabpnOutputs[i].GetConnectionsCount();
			
			for (j=0; j<iConnectionsCount; j++)
			{
				if (m_pabpnOutputs[i].AddConnections(j, pabpnOutputsConnections[j]) == false)
				{
					sprintf(m_szError, "Error adding outputs' connections");
					return false;
				}
			}
		}
		
		return true;
	}
	
	
	bool CBackProp::InitDerWeights()
	{
		INT i;
		INT j;
		INT iConnectionsCount;
		
		for (i=0; i<m_iHiddensCount; i++)
		{
			iConnectionsCount = m_pabpnHiddens[i].GetConnectionsCount();
			
			for (j=0; j<iConnectionsCount; j++)
			{
				if (m_pabpnHiddens[i].SetDerWeight(j, 0.0f) == false)
				{
					sprintf(m_szError, "Error setting hiddens' weights");
					return false;
				}
				
			}
			
			if (m_pabpnHiddens[i].SetDerBiasWeight(0.0f) == false)
			{
				sprintf(m_szError, "Error setting hiddens' bias weights");
				return false;
			}
		}
		
		for (i=0; i<m_iHiddens2Count; i++)
		{
			iConnectionsCount = m_pabpnHiddens2[i].GetConnectionsCount();
			
			for (j=0; j<iConnectionsCount; j++)
			{
				if (m_pabpnHiddens2[i].SetDerWeight(j, 0.0f) == false)
				{
					sprintf(m_szError, "Error setting hiddens2 weights");
					return false;
				}
			}
			if (m_pabpnHiddens2[i].SetDerBiasWeight(0.0f) == false)
			{
				sprintf(m_szError, "Error setting hiddens2 bias weights");
				return false;
			}
		}
		
		for (i=0; i<m_iOutputsCount; i++)
		{
			iConnectionsCount = m_pabpnOutputs[i].GetConnectionsCount();
			
			for (j=0; j<iConnectionsCount; j++)
			{
				if (m_pabpnOutputs[i].SetDerWeight(j, 0.0f) == false)
				{
					sprintf(m_szError, "Error setting outputs' weights");
					return false;
				}
			}
			if (m_pabpnOutputs[i].SetDerBiasWeight(0.0f) == false)
			{
				sprintf(m_szError, "Error setting outputs' bias weights");
				return false;
			}
		}
		
		return true;
	}
	
	
	// Function name    : CMathUtility::frand
	// Description      : Random number generator generating a number between 0 and 1
	// Return type      : float 
	float CBackProp::Frand()
	{
		int num = rand();
		num--;
		
		if (num < 0)
			num = 0;
		
		return static_cast<float>(num) / RAND_MAX;
	}
	
	// Function name    : CBackProp::RandomizeWeights
	// Description      : Randomize the weights of the of the connections between the neurons
	// Return type      : bool 
	bool CBackProp::RandomizeWeights()
	{
		INT i;
		INT j;
		INT iConnectionsCount;
		
		for (i=0; i<m_iHiddensCount; i++)
		{
			iConnectionsCount = m_pabpnHiddens[i].GetConnectionsCount();
			
			for (j=0; j<iConnectionsCount; j++)
			{
				if (m_pabpnHiddens[i].SetWeight(j, (Frand()*0.1)-0.05) == false)
				{
					
					sprintf(m_szError, "Error setting hiddens' weights");
					return false;
				}
			}
			
			if (m_pabpnHiddens[i].SetBiasWeight((Frand()*0.1)-0.05) == false)
			{
				sprintf(m_szError, "Error setting hiddens' bias weights");
				return false;
			}
		}
		
		
		for (i=0; i<m_iHiddens2Count; i++)
		{
			iConnectionsCount = m_pabpnHiddens2[i].GetConnectionsCount();
			
			for (j=0; j<iConnectionsCount; j++)
			{
				if (m_pabpnHiddens2[i].SetWeight(j, (Frand()*0.1)-0.05) == false)
				{
					sprintf(m_szError, "Error setting hiddens2 weights");
					return false;
				}
			}
			
			if (m_pabpnHiddens2[i].SetBiasWeight((Frand()*0.1)-0.05) == false)
			{
				sprintf(m_szError, "Error setting hiddens2 bias weights");
				return false;
			}
		}
		
		for (i=0; i<m_iOutputsCount; i++)
		{
			iConnectionsCount = m_pabpnOutputs[i].GetConnectionsCount();
			
			for (j=0; j<iConnectionsCount; j++)
			{
				if (m_pabpnOutputs[i].SetWeight(j, (Frand()*0.1)-0.05) == false)
				{
					sprintf(m_szError, "Error setting outputs' weights");
					return false;
				}
			}
			
			if (m_pabpnOutputs[i].SetBiasWeight((Frand()*0.1)-0.05) == false)
			{
				sprintf(m_szError, "Error setting outputs' bias weights");
				return false;
			}
		}
		
		return true;
	}
	
	// Function name    : CBackProp::SetInput
	// Description      : Set the input of the particular input neurons
	// Return type      : bool 
	// Argument         : INT iIndex - Zero-based index to the arrays of input neurons
	// Argument         : FLOAT fInput
	bool CBackProp::SetInput(INT iIndex, FLOAT fInput)
	{
		if (iIndex < 0 || iIndex >= m_iInputsCount)
		{
			sprintf(m_szError, "Index out of bounds");
			return false;
		}
		
		if (m_pabpnInputs[iIndex].SetValue(fInput) == false)
		{
			sprintf(m_szError, "Error setting the input values");
			return false;
		}
		
		return true;
	}
	
	
	// Function name    : CBackProp::SetDesiredOutput
	// Description      : Set the desired output value
	// Return type      : bool 
	// Argument         : INT iIndex - Zero-based index to the output neurons
	// Argument         : FLOAT fOutput - The desired output of the neuron
	bool CBackProp::SetDesiredOutput(INT iIndex, FLOAT fOutput)
	{
		if (iIndex < 0 || iIndex >= m_iOutputsCount)
		{
			sprintf(m_szError, "Index out of bounds");
			return false;
		}
		
		if (m_pabpnOutputs[iIndex].SetDesiredValue(fOutput) == false)
		{
			sprintf(m_szError, "Error setting the desired output values");
			return false;
		}
		
		return true;
	}
	
	// Function name    : CBackProp::FeedForward
	// Description      : Feed forward the network
	// Return type      : bool 
	bool CBackProp::FeedForward()
	{
		INT i;
		for (i=0; i<m_iHiddensCount; i++)
		{
			if (m_pabpnHiddens[i].FeedForward() == false)
			{
				sprintf(m_szError, "Error feed forwarding hiddens");
				return false;
			}
		}
		
		for (i=0; i<m_iHiddens2Count; i++)
		{
			if (m_pabpnHiddens2[i].FeedForward() == false)
			{
				sprintf(m_szError, "Error feed forwarding hiddens2");
				return false;
			}
		}
		
		for (i=0; i<m_iOutputsCount; i++)
		{
			if (m_pabpnOutputs[i].FeedForward() == false)
			{
				sprintf(m_szError, "Error feed forwarding outputs");
				return false;
			}
		}
		
		return true;
	}
	
	// Function name    : CBackProp::BackPropagate
	// Description      : Back propagate the error and make the network learn
	// Return type      : bool 
	bool CBackProp::BackPropagate()
	{
		INT i;
		
		for (i=0; i<m_iOutputsCount; i++)
		{
			if (m_pabpnOutputs[i].BackPropagate(m_fLearningRate) == false)
			{
				sprintf(m_szError, "Error backpropagating outputs");
				return false;
			}
		}
		
		for (i=0; i<m_iHiddens2Count; i++)
		{
			if (m_pabpnHiddens2[i].BackPropagate(m_fLearningRate) == false)
			{
				sprintf(m_szError, "Error backpropagating hiddens2");
				return false;
			}
		}
		
		for (i=0; i<m_iHiddensCount; i++)
		{
			if (m_pabpnHiddens[i].BackPropagate(m_fLearningRate) == false)
			{
				sprintf(m_szError, "Error backpropagating hiddens");
				return false;
			}
		}
		
		return true;
	}
	
	bool CBackProp::UpdateDerWeights()
	{
		INT i;
		
		for (i=0; i<m_iOutputsCount; i++)
		{
			if (m_pabpnOutputs[i].UpdateDerWeights(m_fLearningRate) == false)
			{
				sprintf(m_szError, "Error updating derWeight for output");
				return false;
			}
		}
		
		for (i=0; i<m_iHiddens2Count; i++)
		{
			if (m_pabpnHiddens2[i].UpdateDerWeights(m_fLearningRate) == false)
			{
				sprintf(m_szError, "Error updating derWeight for hiddens2");
				return false;
			}
		}
		
		for (i=0; i<m_iHiddensCount; i++)
		{
			if (m_pabpnHiddens[i].UpdateDerWeights(m_fLearningRate) == false)
			{
				sprintf(m_szError, "Error updating derWeight for hiddens");
				return false;
			}
		}
		
		return true;
	}
	
	
	// Function name    : CBackProp::CalculateError
	// Description      : Calculate the error in the feed forwarding
	// Return type      : bool 
	bool CBackProp::CalculateError()
	{
		// get the error value for back propagation
		INT i;
		FLOAT fError = 0.0f;
		for (i=0; i<m_iOutputsCount; i++)
		{
			if (m_pabpnOutputs[i].CalculateOutputError() == false)
			{
				sprintf(m_szError, "Error calculating back propagation error");
				return false;
			}
			fError += m_pabpnOutputs[i].GetErrorTerm();
		}
		//fError /= m_iOutputsCount;
		
		// set error for all the neurons
		for (i=0; i<m_iHiddens2Count; i++)
		{
			float fHError = 0.0f;
			for(int j=0;j<m_iOutputsCount;j++)
				fHError += m_pabpnOutputs[j].GetErrorTerm() * m_pabpnOutputs[j].GetWeight(i);
			if (m_pabpnHiddens2[i].SetError(fHError) == false)
			{
				sprintf(m_szError, "Error setting error for hiddens2");
				return false;
			}
		}
		
		CBackPropNeuron * pabpnOutputsConnections;
		int iConnect;
		if (m_iHiddens2Count > 0)
		{
			pabpnOutputsConnections = m_pabpnHiddens2;
			iConnect=m_iHiddens2Count;
		}
		else
		{
			pabpnOutputsConnections = m_pabpnOutputs;
			iConnect=m_iOutputsCount;
		}
		
		for (i=0; i<m_iHiddensCount; i++)
		{
			float fHError = 0.0f;
			for(int j=0;j<iConnect;j++)
			{
				fHError += pabpnOutputsConnections[j].GetErrorTerm() * pabpnOutputsConnections[j].GetWeight(i);
			}
			if (m_pabpnHiddens[i].SetError(fHError) == false)
			{
				sprintf(m_szError, "Error setting error for hiddens");
				return false;
			}
		}
		
		return true;
	}
	
	
	// Function name    : CBackProp::Learn
	// Description      : To make the network learn
	// Return type      : bool 
	bool CBackProp::Learn()
	{
		if (FeedForward() == false)
		{
			return false;
		}
		
		if (CalculateError() == false)
		{
			return false;
		}
		
		if (BackPropagate() == false)
		{
			return false;
		}
		
		return true;
	}
	
	
	// Function name    : CBackProp::GetInputCount
	// Description      : Return the number of Input neurons
	// Return type      : INT 
	INT CBackProp::GetInputCount()
	{
		return m_iInputsCount;
	}
	
	// Function name    : CBackProp::GetOutput
	// Description      : Get the value of the Output neurons
	// Return type      : FLOAT 
	// Argument         : INT iIndex
	FLOAT CBackProp::GetOutput(INT iIndex)
	{
		if (iIndex < 0 || iIndex >= m_iOutputsCount)
		{
			sprintf(m_szError, "Index out of bounds");
			return 0.0f;
		}
		
		return m_pabpnOutputs[iIndex].GetValue();
	}
	
	
	float CBackProp::GetError(int iIndex)
	{
		if (iIndex < 0 || iIndex >= m_iOutputsCount)
		{
			sprintf(m_szError, "Index out of bounds");
			return 0.0f;
		}
		
		return m_pabpnOutputs[iIndex].GetError();
		
	}
	
	
	// Function name    : CBackProp::GetDesiredOutput
	// Description      : Return the desired value for the Output neurons
	// Return type      : FLOAT 
	// Argument         : INT iIndex
	FLOAT CBackProp::GetDesiredOutput(INT iIndex)
	{
		if (iIndex < 0 || iIndex >= m_iOutputsCount)
		{
			sprintf(m_szError, "Index out of bounds");
			return 0.0f;
		}
		
		return m_pabpnOutputs[iIndex].GetDesiredValue();
	}
	
	FLOAT CBackProp::GetInput(INT iIndex)
	{
		return m_pabpnInputs[iIndex].GetValue();
	}
	
	FLOAT CBackProp::GetOutputPercentage(INT iIndex)
	{
		if (iIndex < 0 || iIndex >= m_iOutputsCount)
		{
			sprintf(m_szError, "Index out of bounds");
			return 0.0f;
		}
		
		FLOAT fPercentage = 1 - m_pabpnOutputs[iIndex].GetDesiredValue() - m_pabpnOutputs[iIndex].GetValue();
		if (fPercentage < 0)
		{
			fPercentage = -fPercentage;
		}
		
		return fPercentage;
	}
	
	
	bool CBackProp::Save(std::ofstream &ofs)
	{
		if (CheckInit() == false)
		{
			return false;
		}
		
		ofs.write(reinterpret_cast<char *>(&m_iInputsCount), sizeof(m_iInputsCount));
		ofs.write(reinterpret_cast<char *>(&m_iHiddensCount), sizeof(m_iHiddensCount));
		ofs.write(reinterpret_cast<char *>(&m_iHiddens2Count), sizeof(m_iHiddens2Count));
		ofs.write(reinterpret_cast<char *>(&m_iOutputsCount), sizeof(m_iOutputsCount));
		ofs.write(reinterpret_cast<char *>(&m_fLearningRate), sizeof(m_fLearningRate));
		
		INT i;
		for (i=0; i<m_iInputsCount; i++)
		{
			if (SaveNeuronData(m_pabpnInputs[i], ofs) == false)
			{
				return false;
			}
		}
		
		for (i=0; i<m_iHiddensCount; i++)
		{
			if (SaveNeuronData(m_pabpnHiddens[i], ofs) == false)
			{
				return false;
			}
		}
		
		for (i=0; i<m_iHiddens2Count; i++)
		{
			if (SaveNeuronData(m_pabpnHiddens2[i], ofs) == false)
			{
				return false;
			}
		}
		
		for (i=0; i<m_iOutputsCount; i++)
		{
			if (SaveNeuronData(m_pabpnOutputs[i], ofs) == false)
			{
				return false;
			}
		}
		
		return true;
	}
	
	bool CBackProp:: GetInfo(FILE * infostream)
	{
		fprintf(infostream,"%s\n","Input Neuron Info");
		for (int i=0; i<m_iInputsCount; i++)
		{
			if (GetNeuronInfo(m_pabpnInputs[i], infostream) == false)
			{
				return false;
			}
		}
		
		fprintf(infostream,"\n\n%s\n","Hidden 1 Neuron Info");
		for (int i=0; i<m_iHiddensCount; i++)
		{
			if (GetNeuronInfo(m_pabpnHiddens[i], infostream) == false)
			{
				return false;
			}
		}
		
		fprintf(infostream,"\n\n%s\n","Hidden 2 Neuron Info");
		for ( int i=0; i<m_iHiddens2Count; i++)
		{
			if (GetNeuronInfo(m_pabpnHiddens2[i], infostream) == false)
			{
				return false;
			}
		}
		
		fprintf(infostream,"\n\n%s\n","Output Neuron Info");
		for (int i=0; i<m_iOutputsCount; i++)
		{
			if (GetNeuronInfo(m_pabpnOutputs[i], infostream) == false)
			{
				return false;
			}
		}
		
		return true;
	}
	
	
	bool CBackProp::Load(FILE * pfData)
	{
		if (m_bInit == true)
		{
			Uninit();
		}
		
		fread(&m_iInputsCount, sizeof(m_iInputsCount), 1, pfData);
		fread(&m_iHiddensCount, sizeof(m_iHiddensCount), 1, pfData);
		fread(&m_iHiddens2Count, sizeof(m_iHiddens2Count), 1, pfData);
		fread(&m_iOutputsCount, sizeof(m_iOutputsCount), 1, pfData);
		fread(&m_fLearningRate, sizeof(m_fLearningRate), 1, pfData);
		
		if (m_iInputsCount <= 0 || m_iOutputsCount <= 0 || m_iHiddensCount <= 0 || m_iHiddens2Count < 0)
		{
			sprintf(m_szError, "Invalid data loaded");
			return false;
		}
		
		if (InitArrays() == false)
		{
			return false;
		}
		
		INT i;
		for (i=0; i<m_iInputsCount; i++)
		{
			if (LoadNeuronData(m_pabpnInputs[i], pfData) == false)
			{
				return false;
			}
		}
		
		for (i=0; i<m_iHiddensCount; i++)
		{
			if (LoadNeuronData(m_pabpnHiddens[i], pfData) == false)
			{
				return false;
			}
		}
		
		for (i=0; i<m_iHiddens2Count; i++)
		{
			if (LoadNeuronData(m_pabpnHiddens2[i], pfData) == false)
			{
				return false;
			}
		}
		
		for (i=0; i<m_iOutputsCount; i++)
		{
			if (LoadNeuronData(m_pabpnOutputs[i], pfData) == false)
			{
				return false;
			}
		}
		
		if (InitConnections() == false)
		{
			return false;
		}
		
		m_bInit = true;
		return true;
	}
	
	
	bool CBackProp::InitArrays()
	{
		// allocate memory for the arrays
		
		m_pabpnInputs = new ( CBackPropNeuron[m_iInputsCount]);
		if (m_pabpnInputs == NULL)
		{
			sprintf(m_szError, "Error Allocating Memory");
			return false;
		}
		
		m_pabpnHiddens = new ( CBackPropNeuron[m_iHiddensCount]);
		if (m_pabpnHiddens == NULL)
		{
			sprintf(m_szError, "Error Allocating Memory");
			return false;
		}
		
		
		
		if (m_iHiddens2Count > 0)
		{
			
			m_pabpnHiddens2 = new (CBackPropNeuron[m_iHiddens2Count]);
			if (m_pabpnHiddens2 == NULL)
			{
				sprintf(m_szError, "Error Allocating Memory");
				return false;
			}
		
		}
		
		m_pabpnOutputs = new (CBackPropNeuron[m_iOutputsCount]);
		if (m_pabpnOutputs == NULL)
		{
			sprintf(m_szError, "Error Allocating Memory");
			return false;
		}
		
	
		return true;
	}
	
	
	bool CBackProp::CheckInit()
	{
		if (m_bInit == false)
		{
			sprintf(m_szError, "The object is not initialized");
			return false;
		}
		
		return true;
	}
	
	
	bool CBackProp::LoadNeuronData(CBackPropNeuron &bpn, FILE * pfData)
	{
		if (bpn.Load(pfData) == false)
		{
			sprintf(m_szError, "Error loading neuron data from file");
			return false;
		}
		
		return true;
	}
	
	bool CBackProp::SaveNeuronData(CBackPropNeuron &bpn, std::ofstream &ofs)
	{
		if (bpn.Save(ofs) == false)
		{
			sprintf(m_szError, "Error saving neuron data to file");
			return false;
		}
		
		return true;
	}
	
	bool CBackProp::GetNeuronInfo(CBackPropNeuron &bpn, FILE * infostream)
	{
		if (bpn.GetInfo(infostream) == false)
		{
			//		::MessageBox(NULL,_TEXT("Error get neuron info to file"), _TEXT("Neuron Info"),0);
			return false;
		}
		return true;
	}
	
	
	
	
	
	
}
