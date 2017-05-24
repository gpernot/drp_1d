#ifndef _REDSHIFT_OPERATOR_CHISQUARELOGLAMBDA_
#define _REDSHIFT_OPERATOR_CHISQUARELOGLAMBDA_

#include <RedshiftLibrary/common/datatypes.h>
#include <RedshiftLibrary/common/range.h>
#include <RedshiftLibrary/operator/operator.h>
#include <RedshiftLibrary/operator/chisquareresult.h>
#include <RedshiftLibrary/common/mask.h>

#include <RedshiftLibrary/spectrum/spectrum.h>
#include <RedshiftLibrary/spectrum/template/template.h>
#include <RedshiftLibrary/spectrum/fluxcorrectionmeiksin.h>

namespace NSEpic
{

class COperatorChiSquareLogLambda : public COperator
{

public:

    COperatorChiSquareLogLambda( std::string calibrationPath );
    ~COperatorChiSquareLogLambda();

     std::shared_ptr<COperatorResult> Compute(const CSpectrum& spectrum, const CTemplate& tpl,
                                    const TFloat64Range& lambdaRange, const TFloat64List& redshifts,
                                    Float64 overlapThreshold, std::vector<CMask> additional_spcMasks, std::string opt_interp, Int32 opt_extinction=0, Int32 opt_dustFitting=0);

    const Float64*  getDustCoeff(Float64 dustCoeff, Float64 maxLambda);
    const Float64*  getMeiksinCoeff(Int32 meiksinIdx, Float64 redshift, Float64 maxLambda);


    Int32 prepareRebinedSpectrum(const CSpectrum& spectrum, const TFloat64List& redshift_step);

private:

    Void BasicFit(const CSpectrum& spectrum, const CTemplate& tpl, Float64 *pfgTplBuffer,
                   const TFloat64Range& lambdaRange, Float64 redshift, Float64 overlapThreshold,
                   Float64& overlapRate, Float64& chiSquare,
                  Float64 &fittingAmplitude,
                  Float64& fittingDtM,
                  Float64& fittingMtM,
                  Float64 &fittingDustCoeff,
                  Float64 &fittingMeiksinIdx,
                  EStatus& status, std::string opt_interp, Float64 forcedAmplitude=-1, Int32 opt_extinction=0, Int32 opt_dustFitting=0, CMask spcMaskAdditional=CMask() );

    // buffers for the precomputed log grid template
    CTemplate       m_templateRebined_bf; //buffer
    CMask           m_mskRebined_bf; //buffer
    CSpectrumSpectralAxis m_shiftedTplSpectralAxis_bf; //buffer

    // ISM Calzetti
    Float64 *m_dataCalzetti;
    Float64 m_NdataCalzetti;
    Float64* m_YtplRawBuffer;
    Int32 m_YtplRawBufferMaxBufferSize;
    Int32 m_nDustCoeff;
    Float64 m_dustCoeffStep;
    Float64 m_dustCoeffStart;
    Float64* m_dataDustCoeff;
    bool calzettiInitFailed;
    
    //IGM meiksin
    CSpectrumFluxCorrectionMeiksin* m_igmCorrectionMeiksin;
};


}

#endif