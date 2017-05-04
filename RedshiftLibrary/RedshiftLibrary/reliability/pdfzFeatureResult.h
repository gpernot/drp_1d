#ifndef _REDSHIFT_OPERATOR_PDFZFEATURERESULT_
#define _REDSHIFT_OPERATOR_PDFZFEATURERESULT_

#include <RedshiftLibrary/processflow/result.h>
#include <RedshiftLibrary/operator/operator.h>
#include <RedshiftLibrary/common/datatypes.h>

#include <boost/unordered_map.hpp>

using namespace std;
namespace NSEpic
{

class CPdfzFeatureResult : public COperatorResult
{

public:

	CPdfzFeatureResult();
    virtual ~CPdfzFeatureResult();

    Void Save( const CDataStore& store, std::ostream& stream ) const;
    Void SaveLine( const CDataStore& store, std::ostream& stream ) const;

	typedef boost::unordered_map<const std::string, Float64> Mapz;
	Mapz mapzfeatures;

};


}

#endif
