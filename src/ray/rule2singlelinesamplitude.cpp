#include <cstdarg>
#include <iostream>

#include <epic/redshift/ray/rule2singlelinesamplitude.h>

using namespace NSEpic;
using namespace std;

void CRule2SingleLinesAmplitude::SetUp( Bool EnabledArgument, ... )
{
  Name = "balmer";
  Enabled = EnabledArgument;
  va_list Arguments;
  va_start ( Arguments, EnabledArgument );
  m_LineType = va_arg( Arguments, Int32 );
  m_LineA = std::string ( va_arg( Arguments, const char* ) );
  m_LineB = std::string ( va_arg( Arguments, const char* ) );
  m_Coefficient = va_arg( Arguments, Float64 );
}

/**
 * \brief Correct both lines depending on their sigmas.
 **/
void CRule2SingleLinesAmplitude::Correct( CLineModelElementList& LineModelElementList )
{
  Int32 iA = LineModelElementList.FindElementIndex( m_LineA, m_LineType );
  if( iA==-1 )
    {
      return;
    }
  if( LineModelElementList.m_Elements[iA]->GetSize()>1 )
    {
      iA=-1;
    }
  Int32 iB = LineModelElementList.FindElementIndex( m_LineB, m_LineType );
  if( iB==-1 )
    {
      return;
    }
  if( LineModelElementList.m_Elements[iB]->GetSize()>1 )
    {
      iB=-1;
    }
  if( iA==-1 || iB==-1 || iA==iB )
    {
      return;
    }
  if( LineModelElementList.m_Elements[iA]->IsOutsideLambdaRange() == false )
    {
      Float64 nSigma = 1.0;
      Float64 ampA = LineModelElementList.m_Elements[iA]->GetFittedAmplitude( 0 );
      Float64 erA = LineModelElementList.m_Elements[iA]->GetFittedAmplitudeErrorSigma( 0 );
      Float64 ampB = LineModelElementList.m_Elements[iB]->GetFittedAmplitude( 0 );
      Float64 erB = LineModelElementList.m_Elements[iB]->GetFittedAmplitudeErrorSigma( 0 );
      if( ampB!=0.0 && (erA!=0 && erB!=0) && std::abs( ampB )>std::abs( ampA*m_Coefficient ) )
	{
	  Float64 R = 1.0/m_Coefficient;
	  Float64 wA = 0.0;
	  if( erA!=0.0 )
	    {
	      wA = 1.0/(erA*erA);
	    }
	  Float64 wB = 0.0;
	  if( erB!=0.0 )
	    {
	      wB = 1.0/(erB*erB*R*R);
	    }
	  Float64 correctedA = (ampA*wA + ampB*wB*R)/(wA+wB);
	  Float64 correctedB = correctedA/R;
	  LineModelElementList.m_Elements[iA]->SetFittedAmplitude( correctedA, erA ); //check: keep the original error sigma ?
	  LineModelElementList.m_Elements[iB]->SetFittedAmplitude( correctedB, erB ); //check: keep the original error sigma ?
	}
      else
	{
	  if( ampB!=0.0 && ampA==0.0 )
	    {
	      Float64 maxB = erA;
	      LineModelElementList.m_Elements[iB]->LimitFittedAmplitude( 0, maxB );
	    }
	}
    }
}

Bool CRule2SingleLinesAmplitude::Check( CLineModelElementList& LineModelElementList )
{
  return false;
}
