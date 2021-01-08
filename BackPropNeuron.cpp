/*
 *  BackPropNeuron.cpp
 *  ImageProcessingC
 *
 *  
 */

#include "BackPropNeuron.h"

namespace KimBackProp
{
	
	CBackPropNeuron::CBackPropNeuron()
	{
		m_pbpnaConnections = NULL;
		m_faConnectionsWeights = NULL;
		m_faConnectionsDerWeights = NULL;
	}
	
	CBackPropNeuron::~CBackPropNeuron()
	{
		UnInitArrays();
	}
	
	
	// Function name    : CBackPropNeuron::Init
	// Description      : To initialize the properties of the backpropagation neuron class
	// Return type      : bool 
	// Argument         : int iID - the ID of the neuron for a particular layer
	// Argument         : int iLayer - the Layer of the neuron
	// Argument         : int iConnections - the number of connections to other neurons
	bool CBackPropNeuron::Init(int iID, int iLayer, int iConnections)
	{
		m_bInit = false;
		
		// check for argument errors
		if (iID < 0)
			return false;
		
		if (iLayer < 0)
			return false;
		
		if (iConnections < 0)
		{
			return false;
		}
		
		// assign everything to zero
		m_fBiasWeight = 0;
		m_fDerBiasWeight = 0;
		m_fDesiredValue = 0;
		m_fError = 0;
		m_fValue = 0;
		
		// assign values to member variables
		m_iID = iID;
		m_iLayer = iLayer;
		m_iConnections = iConnections;
		
		if (m_iConnections > 0)
		{
			if (InitArrays(iConnections) == false)
				return false;
		}
		
		m_bInit = true;
		return true;
	}
	
	
	
	
	
	
	
	// Function name    : CBackPropNeuron::InitArrays
	// Description      : To initialize the arrays, m_bpnConnections & m_faConnectionsWeights
	// Return type      : bool 
	// Argument         : int iConnections
	bool CBackPropNeuron::InitArrays(int iConnections)
	{
		// check for argument errors
		if (iConnections <= 0)
			return false;
		
		UnInitArrays();
		
		m_pbpnaConnections = new CBackPropNeuron *[iConnections];
		
		// check if memory allocated
		if (m_pbpnaConnections == NULL)
			return false;
		
		m_faConnectionsWeights = new float[iConnections];
		m_faConnectionsDerWeights = new float[iConnections];
		
		if (m_faConnectionsWeights == NULL)
			return false;
		
		if (m_faConnectionsDerWeights == NULL)
			return false;
		
		memset(m_faConnectionsWeights, 0, sizeof(float) * iConnections);
		memset(m_faConnectionsDerWeights, 0, sizeof(float) * iConnections);
		
		m_iConnections = iConnections;
		
		return true;
	}

	
	
	// Function name    : CBackPropNeuron::UnInitArrays
	// Description      : To uninitialize the arrays, m_bpnConnections & m_faConnectionsWeights
	// Return type      : bool 
	bool CBackPropNeuron::UnInitArrays()
	{
		if (m_pbpnaConnections != NULL)
		{
			delete [] m_pbpnaConnections;
			m_pbpnaConnections = NULL;
		}
		
		if (m_faConnectionsWeights != NULL)
		{
			delete [] m_faConnectionsWeights;
			m_faConnectionsWeights = NULL;
		}
		
		if (m_faConnectionsDerWeights != NULL)
		{
			delete [] m_faConnectionsDerWeights;
			m_faConnectionsDerWeights = NULL;
		}
		
		
		m_iConnections = 0;
		
		return true;
	}
	
	// Function name    : CBackPropNeuron::GetValue
	// Description      : Get the value of the neuron
	// Return type      : float 
	float CBackPropNeuron::GetValue()
	{
		return m_fValue;
	}
	
	
	// Function name    : CBackPropNeuron::SetValue
	// Description      : Set the value of the neuron
	// Return type      : bool 
	// Argument         : float fValue
	bool CBackPropNeuron::SetValue(float fValue)
	{
		m_fValue = fValue;
		
		return true;
	}
	
	
	// Function name    : CBackPropNeuron::AddConnections
	// Description      : Add a new connection to the neuron
	// Return type      : bool 
	// Argument         : int iConnectionsIndex
	// Argument         : CBackPropNeuron &bpnNeuron
	bool CBackPropNeuron::AddConnections(int iConnectionsIndex, CBackPropNeuron &bpnNeuron)
	{
		if (CheckInit() == false)
		{
			return false;
		}
		
		if (m_iConnections <= 0)
		{
			return false;
		}
		
		if (iConnectionsIndex < 0 || iConnectionsIndex >= m_iConnections)
			return false;
		
		m_pbpnaConnections[iConnectionsIndex] = &bpnNeuron;
		
		return true;
	}
	
	
	// Function name    : CBackPropNeuron::FeedForward
	// Description      : Start the feed forwarding for the neuron (activate this neuron)
	// Return type      : bool 
	bool CBackPropNeuron::FeedForward()
	{
		if (CheckInit() == false)
		{
			return false;
		}
		
		if (m_iConnections <= 0)
		{
			return false;
		}
		
		m_fValue = 0.0f;
		int i;
		for (i=0; i<m_iConnections; i++)
		{
			m_fValue += m_pbpnaConnections[i]->m_fValue * m_faConnectionsWeights[i];
		}
		
		// add with bias
		m_fValue += m_fBiasWeight;
		// normalize the m_fValue
		m_fValue /= m_iConnections + 1;
		m_fValue = SigmoidActivation(m_fValue);
		
		return true;
	}
	
	// Function name    : CBackPropNeuron::CalculateOutputError
	// Description      : Calculate the Error for the value (m_fValue) based on the desired value (m_fDesiredValue)
	// Return type      : bool 
	bool CBackPropNeuron::CalculateOutputError()
	{
		if (CheckInit() == false)
		{
			return false;
		}
		
		m_fError = (m_fDesiredValue - m_fValue)*(1-m_fValue)*m_fValue;
		
		return true;
	}
	
	
	// Function name    : CBackPropNeuron::SetWeight
	// Description      : Set the weight for a particular connection
	// Return type      : bool 
	// Argument         : int iConnectionsIndex - Index of connection neuron
	// Argument         : float fWeight - weight to be assigned
	bool CBackPropNeuron::SetWeight(int iConnectionsIndex, float fWeight)
	{
		if (CheckInit() == false)
		{
			return false;
		}
		
		if (m_iConnections <= 0)
		{
			return false;
		}
		
		if (iConnectionsIndex < 0 || iConnectionsIndex >= m_iConnections)
			return false;
		
		m_faConnectionsWeights[iConnectionsIndex] = fWeight;
		
		return true;
	}
	
	bool CBackPropNeuron::SetDerWeight(int iConnectionsIndex, float fWeight)
	{
		if (CheckInit() ==false)
		{
			return false;
		}
		
		if (m_iConnections <= 0)
		{
			return false;
		}
		
		if (iConnectionsIndex < 0 || iConnectionsIndex >= m_iConnections)
			return false;
		
		m_faConnectionsDerWeights[iConnectionsIndex] = fWeight;
		
		return true;
	}
	
	// Function name    : CBackPropNeuron::GetWeight
	// Description      : Get the weight for a particular connection
	// Return type      : float 
	// Argument         : int iConnectionsIndex - Index of connection neuron
	float CBackPropNeuron::GetWeight(int iConnectionsIndex)
	{
		if (CheckInit() == false)
		{
			return false;
		}
		
		if (m_iConnections <= 0)
		{
			return false;
		}
		
		if (iConnectionsIndex < 0 || iConnectionsIndex >= m_iConnections)
			return false;
		
		return m_faConnectionsWeights[iConnectionsIndex];
	}
	
	// Function name    : CBackPropNeuron::SetDesiredValue
	// Description      : Set the desired (target) value of the neuron, usually used for Output neurons
	// Return type      : bool 
	bool CBackPropNeuron::SetDesiredValue(float fDesiredValue)
	{
		m_fDesiredValue = fDesiredValue;
		
		return true;
	}
	
	
	
	// Function name    : CBackPropNeuron::GetDesiredValue
	// Description      : Get the desired (target) value of the neuron, usually used for Output neurons
	// Return type      : float 
	float CBackPropNeuron::GetDesiredValue()
	{
		return m_fDesiredValue;
	}
	
	
	// Function name    : CBackPropNeuron::GetErrorTerm
	// Description      : Get the calculated error term
	// Return type      : float 
	float CBackPropNeuron::GetErrorTerm()
	{
		return m_fError;
	}
	
	
	// Function name    : CBackPropNeuron::GetError
	// Description      : Get the calculated error of the (output node)
	// Return type      : float 
	float CBackPropNeuron::GetError()
	{
		return m_fDesiredValue - m_fValue;
	}
	
	
	
	// Function name    : CBackPropNeuron::SetError
	// Description      : Set the calculated error
	// Return type      : bool 
	// Argument         : float fError
	bool CBackPropNeuron::SetError(float fError)
	{
		m_fError = fError*(1-m_fValue)*m_fValue;
		return true;
	}
	
	
	// Function name    : CBackPropNeuron::BackPropagate
	// Description      : Do a back propagation, to change the weights
	// Return type      : bool 
	bool CBackPropNeuron::BackPropagate(float fLearningRate)
	{
		if (CheckInit() == false)
		{
			return false;
		}
		
		if (m_iConnections <= 0)
		{
			return false;
		}
		
		if (fLearningRate <= 0.0f)
			return false;
		
		// loop through all the connections and change the weights accordingly
		int i;
		for (i=0; i<m_iConnections; i++)
		{
			m_faConnectionsWeights[i] += m_faConnectionsDerWeights[i];
		}
		
		// change the bias weight
		m_fBiasWeight += m_fDerBiasWeight;
		
		return true;
	}
	
	bool CBackPropNeuron::UpdateDerWeights(float fLearningRate)
	{
		if (CheckInit() == false)
		{
			return false;
		}
		
		if (m_iConnections <= 0)
		{
			return false;
		}
		
		if (fLearningRate <= 0.0f)
			return false;
		
		// loop through all the connections and change the weights accordingly
		int i;
		for (i=0; i<m_iConnections; i++)
		{
			m_faConnectionsDerWeights[i] += fLearningRate * m_fError * m_pbpnaConnections[i]->m_fValue;
		}
		
		// change the bias weight
		m_fDerBiasWeight += fLearningRate * m_fError;
		
		return true;
	}
	
	
	// Function name    : CBackPropNeuron::SetBiasWeight
	// Description      : To set the bias weight (this bias is just a safety measure against all inputs of zero (0) value)
	// Return type      : bool 
	bool CBackPropNeuron::SetBiasWeight(float fBiasWeight)
	{
		m_fBiasWeight = fBiasWeight;
		
		return true;
	}
	
	// Function name    : CBackPropNeuron::SetDerBiasWeight
	// Description      : To set the bias weight (this bias is just a safety measure against all inputs of zero (0) value)
	// Return type      : bool 
	bool CBackPropNeuron::SetDerBiasWeight(float fBiasWeight)
	{
		m_fDerBiasWeight = fBiasWeight;
		
		return true;
	}
	
	
	// Function name    : CBackPropNeuron::GetBiasWeight
	// Description      : To get the bias weight (this bias is just a safety measure against all inputs of zero (0) value)
	// Return type      : float 
	float CBackPropNeuron::GetBiasWeight()
	{
		return m_fBiasWeight;
	}
	
	int CBackPropNeuron::GetConnectionsCount()
	{
		return m_iConnections;
	}
	
	bool CBackPropNeuron::Load(FILE * pfData)
	{
		m_bInit = false;
		
		fread(&m_iID, sizeof(m_iID), 1, pfData);
		fread(&m_iLayer, sizeof(m_iLayer), 1, pfData);
		fread(&m_iConnections, sizeof(m_iConnections), 1, pfData);
		
		fread(&m_fBiasWeight, sizeof(m_fBiasWeight), 1, pfData);
		fread(&m_fDesiredValue, sizeof(m_fDesiredValue), 1, pfData);
		fread(&m_fError, sizeof(m_fError), 1, pfData);
		fread(&m_fValue, sizeof(m_fValue), 1, pfData);
		
		// initialize the weight arrays and connections arrays
		if (m_iConnections > 0)
		{
			if (InitArrays(m_iConnections) == false)
			{
				return false;
			}
		}
		
		INT i;
		for (i=0; i<m_iConnections; i++)
		{
			fread(&m_faConnectionsWeights[i], sizeof(m_faConnectionsWeights[i]), 1, pfData);
		}
		
		m_bInit = true;
		return true;
	}
	
	
	bool CBackPropNeuron::Save(std::ofstream & ofs)
	{
		if (CheckInit() == false)
		{
			return false;
		}
		
		ofs.write(reinterpret_cast<char *>(&m_iID), sizeof(m_iID));
		ofs.write(reinterpret_cast<char *>(&m_iLayer), sizeof(m_iLayer));
		ofs.write(reinterpret_cast<char *>(&m_iConnections), sizeof(m_iConnections));
		
		ofs.write(reinterpret_cast<char *>(&m_fBiasWeight), sizeof(m_fBiasWeight));
		ofs.write(reinterpret_cast<char *>(&m_fDesiredValue), sizeof(m_fDesiredValue));
		ofs.write(reinterpret_cast<char *>(&m_fError), sizeof(m_fError));
		ofs.write(reinterpret_cast<char *>(&m_fValue), sizeof(m_fValue));
		
		INT i;
		for (i=0; i<m_iConnections; i++)
		{
			ofs.write(reinterpret_cast<char *>(&m_faConnectionsWeights[i]), sizeof(m_faConnectionsWeights[i]));
		}
		
		return true;
	}
	
	bool CBackPropNeuron::GetInfo(FILE * infostream)
	{
		fprintf(infostream, "\n%d %d %d %f %f %f %f",
				m_iID, m_iLayer, m_iConnections,
				m_fBiasWeight, m_fDesiredValue, m_fError, m_fValue);
		fprintf(infostream, "\n%s\n", "Connection Weight");
		for (int i=0; i<m_iConnections; i++)
		{
			if (i==m_iConnections-1)
				fprintf(infostream, "%f\n\n", m_faConnectionsWeights[i]);
			else
				fprintf(infostream, "%f ", m_faConnectionsWeights[i]);
		}
		return true;
	}
	
	
	
	bool CBackPropNeuron::CheckInit()
	{
		if (m_bInit == false)
		{
			return false;
		}
		
		return true;
	}
	
	float CBackPropNeuron::SigmoidActivation(float fNumber)
	{
		return static_cast<float> (1.0 / (1.0 + exp(-fNumber)));
	}
	
	
}