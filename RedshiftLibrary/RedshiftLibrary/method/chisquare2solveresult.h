#ifndef _REDSHIFT_OPERATOR_CHISQUARE2SOLVERESULT_
#define _REDSHIFT_OPERATOR_CHISQUARE2SOLVERESULT_

#include <RedshiftLibrary/processflow/result.h>
#include <RedshiftLibrary/common/datatypes.h>
#include <RedshiftLibrary/ray/catalog.h>

#include <vector>

namespace NSEpic
{

class CProcessFlowContext;

/**
 * \ingroup Redshift
 */
class CChisquare2SolveResult : public COperatorResult
{

public:

    enum EType
    {
             nType_raw = 1,
             nType_continuumOnly = 2,
             nType_noContinuum = 3,
             nType_all = 4,
    };

    CChisquare2SolveResult();
    virtual ~CChisquare2SolveResult();

    Void Save( const CDataStore& store, std::ostream& stream ) const;
    Void SaveLine( const CDataStore& store, std::ostream& stream ) const;
    Bool GetBestRedshift(const CDataStore& store, Float64& redshift, Float64& merit, std::string& tplName , Float64 &amplitude, Float64 &dustCoeff, Int32 &meiksinIdx) const;
    Bool GetBestRedshiftPerTemplateString( const CDataStore& store, std::string& output ) const;
    Bool GetBestRedshiftFromPdf(const CDataStore& store, Float64& redshift, Float64& merit) const;

    Int32 m_type;

    Int32 m_bestRedshiftMethod = 2; //best chi2, best proba

};


}

#endif


