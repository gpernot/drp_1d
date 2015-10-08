#ifndef ELEMENTLIST_H
#define ELEMENTLIST_H


#include <epic/core/common/ref.h>
#include <epic/core/common/range.h>
#include <epic/redshift/common/datatypes.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include <epic/redshift/ray/catalog.h>
#include <epic/redshift/spectrum/spectrum.h>

#include <epic/redshift/operator/linemodelresult.h>
#include <epic/redshift/linemodel/element.h>
#include <epic/redshift/linemodel/singleline.h>
#include <boost/shared_ptr.hpp>

namespace NSEpic
{

class CLineModelElementList
{

public:

    CLineModelElementList( const CSpectrum& spectrum, const CRayCatalog::TRayVector& restRayList );
    ~CLineModelElementList();

    void LoadCatalog(const CRayCatalog::TRayVector& restRayList);
    void LoadCatalog_tplExtendedBlue(const CRayCatalog::TRayVector& restRayList);
    void LoadCatalogMultilineBalmer(const CRayCatalog::TRayVector& restRayList);
    void LoadCatalogSingleLines(const CRayCatalog::TRayVector& restRayList);
    void LogCatalogInfos();

    Int32 GetModelValidElementsNDdl();
    std::vector<Int32> GetModelValidElementsIndexes();
    void SetElementAmplitude(Int32 j, Float64 a);
    Float64 GetElementAmplitude(Int32 j);

    void fit(Float64 redshift, CLineModelResult::SLineModelSolution &modelSolution);
    void refreshModel();
    void addToModel();

    Float64 getLeastSquareMerit();
    Float64 getLeastSquareMeritUnderElements();
    CLineModelResult::SLineModelSolution GetModelSolution();
    const CSpectrum&                GetModelSpectrum() const;

private:

    void fitAmplitudesSimplex();
    void fitAmplitudesLinSolve();
    std::vector<Int32> getSupportIndexes();

    std::vector<Int32> findLineIdxInCatalog(const CRayCatalog::TRayVector& restRayList, std::string strTag, Int32 type);
    Void Apply2SingleLinesAmplitudeRule(Int32 linetype, std::string lineA, std::string lineB, Float64 coeff );

    void addSingleLine(const CRay &r, Int32 index, Float64 nominalWidth);
    void addDoubleLine(const CRay &r1, const CRay &r2, Int32 index1, Int32 index2, Float64 nominalWidth, Float64 a1, Float64 a2);

    void applyRules();
    Void ApplyStrongHigherWeakRule( Int32 lineType );
    Float64 FindHighestStrongLineAmp( Int32 lineType );

    Int32 FindElementIndex(Int32 LineCatalogIndex);
    Int32 FindElementIndex(std::string LineTagStr, Int32 linetype);


    Float64 m_Redshift;
    std::vector<boost::shared_ptr<CLineModelElement>  > m_Elements;

    CRef<CSpectrum>   m_SpectrumModel;
    CSpectrumFluxAxis m_SpcFluxAxis;

    CRayCatalog::TRayVector m_RestRayList;

    Float64 m_nominalWidthDefaultEmission;
    Float64 m_nominalWidthDefaultAbsorption;
};

}







#endif // ELEMENTLIST_H

