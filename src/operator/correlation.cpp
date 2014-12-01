#include <epic/redshift/operator/correlation.h>

#include <epic/core/debug/assert.h>
#include <epic/core/log/log.h>
#include <epic/redshift/spectrum/fluxaxis.h>
#include <epic/redshift/spectrum/spectralaxis.h>
#include <epic/redshift/spectrum/spectrum.h>
#include <epic/redshift/spectrum/template/template.h>
#include <epic/redshift/spectrum/tools.h>
#include <epic/redshift/common/mask.h>

#include <math.h>
#include <assert.h>

#define NOT_OVERLAP_VALUE NAN
#include <stdio.h>

using namespace NSEpic;
using namespace std;

COperatorCorrelation::COperatorCorrelation()
{
    m_TotalDuration = -1.0f;
}

COperatorCorrelation::~COperatorCorrelation()
{

}


const TFloat64List& COperatorCorrelation::GetResults() const
{
    return m_Correlation;
}

const COperatorCorrelation::TStatusList& COperatorCorrelation::GetStatus() const
{
    return m_Status;
}

Float64 COperatorCorrelation::GetComputationDuration() const
{
    return m_TotalDuration;
}


/**
 * Compute correlation factor between spectrum and tpl for each value specified in redshifts.
 * \note Spectrum AND template MUST be in log scale
 */
Bool COperatorCorrelation::Compute(   const CSpectrum& spectrum, const CTemplate& tpl,
                                      const TFloat64Range& lambdaRange, const CRedshifts& redshifts,
                                      Float64 overlapThreshold )
{
    Bool retVal;

    // Input spectrum MUST be in log scales
    if( spectrum.GetSpectralAxis().IsInLogScale() == false || tpl.GetSpectralAxis().IsInLogScale() == false )
    {
        Log.LogError("Failed to compute Cross correlation, input spectrum or template are not in log scale");
        return false;
    }

    boost::posix_time::ptime  startTime = boost::posix_time::microsec_clock::local_time();

    DebugAssert( overlapThreshold > 0.0 && overlapThreshold <= 1.0 );

    CSpectrumSpectralAxis shiftedTplSpectralAxis( tpl.GetSampleCount(), true );

    const CSpectrumSpectralAxis& spcSpectralAxis = spectrum.GetSpectralAxis();
    const CSpectrumFluxAxis& spcFluxAxis = spectrum.GetFluxAxis();

    const CSpectrumSpectralAxis& tplSpectralAxis = tpl.GetSpectralAxis();
    const CSpectrumFluxAxis& tplFluxAxis = tpl.GetFluxAxis();

    m_Correlation.resize( redshifts.GetRedshiftsCount() );
    m_Overlap.resize( redshifts.GetRedshiftsCount() );
    m_Status.resize( redshifts.GetRedshiftsCount() );

    CMask spcMask( spectrum.GetSampleCount() );
    CMask tplMask( tpl.GetSampleCount() );

    // Compute clamped lambda range over spectrum
    TFloat64Range spcLambdaRange;
    retVal = spcSpectralAxis.ClampLambdaRange( lambdaRange, spcLambdaRange );
    DebugAssert( retVal );


    for ( Int32 i=0; i<redshifts.GetRedshiftsCount(); i++)
    {
        m_Correlation[i] = NAN;
        m_Status[i] = nStatus_InvalidInputData;
        m_Overlap[i] = 0;

        // Shift Template
        Float64 onePlusRedshift = 1.0 + redshifts[i];
        shiftedTplSpectralAxis.ShiftByWaveLength( tplSpectralAxis, onePlusRedshift, CSpectrumSpectralAxis::nShiftForward );

        TFloat64Range intersectedLambdaRange( 0.0, 0.0 );

        // Compute clamped lambda range over template
        TFloat64Range tplLambdaRange;
        retVal = shiftedTplSpectralAxis.ClampLambdaRange( lambdaRange, tplLambdaRange );
        DebugAssert( retVal );

        // if there is any intersection between the lambda range of the spectrum and the lambda range of the template
        // Compute the intersected range
        if( TFloat64Range::Intersect( tplLambdaRange, spcLambdaRange, intersectedLambdaRange ) )
        {
            m_Overlap[i] = intersectedLambdaRange.GetLength() / spcLambdaRange.GetLength();
        }

        if( m_Overlap[i] < overlapThreshold )
        {
            m_Status[i] = nStatus_AboveOverlapThreshold;
            continue;
        }

        const Float64* error = spcFluxAxis.GetError();

        DebugAssert( error!= NULL );

        Float64 spcMean = 0.0;
        Float64 spcSDev = 0.0;
        spcSpectralAxis.GetMask( intersectedLambdaRange, spcMask );
        if( !spcFluxAxis.ComputeMeanAndSDev( spcMask, spcMean, spcSDev, error ) )
        {
            m_Status[i] = nStatus_InvalidInputData;
            continue;
        }

        Float64 tplMean = 0.0;
        Float64 tplSDev = 0.0;
        shiftedTplSpectralAxis.GetMask( intersectedLambdaRange, tplMask );
        if( !tplFluxAxis.ComputeMeanAndSDev( tplMask, tplMean, tplSDev, NULL ) )
        {
            m_Status[i] = nStatus_InvalidInputData;
            continue;
        }

        const Float64* Xtpl = shiftedTplSpectralAxis.GetSamples();
        const Float64* Ytpl = tplFluxAxis.GetSamples();
        const Float64* Xspc = spcSpectralAxis.GetSamples();
        const Float64* Yspc = spcFluxAxis.GetSamples();

        TFloat64Range logIntersectedLambdaRange( log( intersectedLambdaRange.GetBegin() ), log( intersectedLambdaRange.GetEnd() ) );

        // Move cursors up to lambda range start
        Int32 j = 0;
        while( Xspc[j] < logIntersectedLambdaRange.GetBegin() )
            j++;

        Int32 k = 0;
        while( Xtpl[k] < logIntersectedLambdaRange.GetBegin() )
            k++;


        Float64 sumCorr=0;
        Float64 sumWeight=0;
        Float64 tplInterpolatedFlux=-1;
        Float32 t = 0;
        while( k<tpl.GetSampleCount()-1 && Xtpl[k] <= logIntersectedLambdaRange.GetEnd() )
        {
            while( j<spectrum.GetSampleCount() && Xspc[j] <= Xtpl[k+1] )
            {
                t = ( Xspc[j] - Xtpl[k] ) / ( Xtpl[k+1] - Xtpl[k] );

                tplInterpolatedFlux = Ytpl[k] + ( Ytpl[k+1] - Ytpl[k] ) * t;

                sumWeight += 1.0 / error[j];
                sumCorr += ( tplInterpolatedFlux - tplMean ) * ( Yspc[j] - spcMean ) / error[j];

                j++;
            }

            k++;
        }

        if( sumWeight>0 )
        {
            m_Correlation[i] = sumCorr / ( tplSDev * spcSDev * sumWeight );
            m_Status[i] = nStatus_OK;
        }
    }

    boost::posix_time::time_duration diff = boost::posix_time::microsec_clock::local_time() - startTime;

    m_TotalDuration = diff.total_seconds();

    return true;
}


