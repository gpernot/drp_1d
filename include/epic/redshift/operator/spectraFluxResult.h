#ifndef _REDSHIFT_OPERATOR_SPECTRARESULT_
#define _REDSHIFT_OPERATOR_SPECTRARESULT_

#include <epic/redshift/processflow/result.h>
#include <epic/core/common/datatypes.h>
#include <epic/redshift/operator/operator.h>


using namespace std;
namespace NSEpic
{

class CSpectraFluxResult : public COperatorResult
{

public:

    CSpectraFluxResult();
    virtual ~CSpectraFluxResult();

    CSpectraFluxResult ( UInt32 optio );

    Void Save( const CDataStore& store, std::ostream& stream ) const;
    Void SaveLine( const CDataStore& store, std::ostream& stream ) const;

    TFloat64List   fluxes;
    TFloat64List   wavel;

    UInt32	         m_optio;

};


}

#endif
