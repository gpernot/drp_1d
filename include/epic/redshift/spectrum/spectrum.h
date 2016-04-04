#ifndef _REDSHIFT_SPECTRUM_SPECTRUM_
#define _REDSHIFT_SPECTRUM_SPECTRUM_

#include <epic/core/common/range.h>
#include <epic/redshift/spectrum/fluxaxis.h>
#include <epic/redshift/spectrum/spectralaxis.h>
#include <epic/redshift/continuum/continuum.h>

#include <string>

namespace NSEpic
{

/**
 * \ingroup Redshift
 */
class CSpectrum
{

public:

    enum EFLags
    {

    };

    CSpectrum();
    CSpectrum(const CSpectrum& other, TFloat64List mask);
    ~CSpectrum();

    CSpectrum& operator=(const CSpectrum& other);

    Void  SetName( const char* name );

    Bool InvertFlux();

    const CSpectrumSpectralAxis&    GetSpectralAxis() const;
    const CSpectrumFluxAxis&        GetFluxAxis() const;

    const std::string               GetName() const;

    CSpectrumFluxAxis&              GetFluxAxis();
    CSpectrumSpectralAxis&          GetSpectralAxis();

    UInt32                          GetSampleCount() const;
    Float64                         GetResolution() const;
    Float64                         GetMeanResolution() const;
    TLambdaRange                    GetLambdaRange() const;

    bool                         GetMeanFluxInRange( TFloat64Range wlRange, Float64& mean ) const;

    Bool                            ConvertToLogScale();
    Bool                            ConvertToLinearScale();

    Bool                            RemoveContinuum( CContinuum& remover );
    const Bool                      IsFluxValid(Float64 LambdaMin, Float64 LambdaMax) const;
    const Bool                      IsNoiseValid(Float64 LambdaMin, Float64 LambdaMax) const;

private:

    std::string                     m_Name;
    CSpectrumFluxAxis               m_FluxAxis;
    CSpectrumSpectralAxis           m_SpectralAxis;
};

inline
UInt32 CSpectrum::GetSampleCount() const
{
    return m_SpectralAxis.GetSamplesCount();
}

inline
const CSpectrumSpectralAxis& CSpectrum::GetSpectralAxis() const
{
    return m_SpectralAxis;
}

inline
const CSpectrumFluxAxis& CSpectrum::GetFluxAxis() const
{
    return m_FluxAxis;
}

inline
CSpectrumSpectralAxis& CSpectrum::GetSpectralAxis()
{
    return m_SpectralAxis;
}

inline
CSpectrumFluxAxis& CSpectrum::GetFluxAxis()
{
    return m_FluxAxis;
}


}

#endif
