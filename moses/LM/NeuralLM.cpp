
#include "moses/StaticData.h"
#include "moses/FactorCollection.h"
#include "NeuralLM.h"
#include "neuralLM.h"

using namespace std;

namespace Moses
{
NeuralLM::NeuralLM()
{
  // This space intentionally left blank
}


NeuralLM::~NeuralLM()
{
  delete m_neuralLM;
}


bool NeuralLM::Load(const std::string &filePath, FactorType factorType, size_t nGramOrder) 
{

  TRACE_ERR("Loading NeuralLM " << filePath << endl);
  
  // Store parameters
  m_nGramOrder = nGramOrder;
  m_filePath = filePath;
  m_factorType = factorType;
  if (factorType == NOT_FOUND) {
    m_factorType = 0;
  }
  
  // Set parameters required by ancestor classes
  FactorCollection &factorCollection = FactorCollection::Instance();
  m_sentenceStart = factorCollection.AddFactor(Output, m_factorType, BOS_);
  m_sentenceStartArray[m_factorType] = m_sentenceStart;
  m_sentenceEnd		= factorCollection.AddFactor(Output, m_factorType, EOS_);
  m_sentenceEndArray[m_factorType] = m_sentenceEnd;  
  
  m_neuralLM = new nplm::neuralLM();
  m_neuralLM->read(m_filePath);
  m_neuralLM->set_log_base(10);
  
  return true;
  //TODO: Implement this
}


LMResult NeuralLM::GetValue(const vector<const Word*> &contextFactor, State* finalState) const
{

  unsigned int hashCode = 0;
  vector<int> words(contextFactor.size());
//  TRACE_ERR("NeuralLM words:");
  for (size_t i=0, n=contextFactor.size(); i<n; i+=1) {
    const Word* word = contextFactor[i];
    const Factor* factor = word->GetFactor(m_factorType);
    const std::string& string= factor->GetString();
    int neuralLM_wordID = m_neuralLM->lookup_word(string);
    words[i] = neuralLM_wordID;
    hashCode += neuralLM_wordID;
//    TRACE_ERR(" " << string << "(" << neuralLM_wordID << ")" );
  }

  double value = m_neuralLM->lookup_ngram(words);
//  TRACE_ERR("\t=\t" << value);
//  TRACE_ERR(endl);
  
  // Create a new struct to hold the result
  LMResult ret;
  ret.score = value;
  ret.unknown = false;
  
  
  // State* finalState is a void pointer
  //
  // Construct a hash value from the vector of words (contextFactor)
  // 
  // The hash value must be the same size as sizeof(void*)
  // 
  // TODO Set finalState to the above hash value
  
  // use last word as state info
//  const Factor *factor;
//  size_t hash_value(const Factor &f);
//  if (contextFactor.size()) {
//    factor = contextFactor.back()->GetFactor(m_factorType);
//  } else {
//    factor = NULL;
//  }
//
//  (*finalState) = (State*) factor;

  (*finalState) = (State*) hashCode;
  
  return ret;
}

}



