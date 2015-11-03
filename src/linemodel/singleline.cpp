
#include <epic/redshift/linemodel/element.h>
#include <epic/redshift/linemodel/singleline.h>

#include <epic/core/debug/assert.h>
#include <epic/redshift/spectrum/spectrum.h>
#include <epic/core/log/log.h>

#include <float.h>
#include <algorithm>

using namespace NSEpic;

CSingleLine::CSingleLine(const CRay& r , Int32 widthType, Float64 nominalWidth, std::vector<Int32> catalogIndexes):CLineModelElement(widthType)
{
    m_ElementType = "CSingleLine";
    m_Ray = r;

    if( m_Ray.GetType()==CRay::nType_Emission ){
        m_SignFactor = 1.0;
    }else{
        m_SignFactor = -1.0;
    }

    m_NominalWidth = nominalWidth;
    m_FittedAmplitude = -1;
    m_FittedAmplitudeErrorSigma = -1;

    m_NSigmaSupport = 8.0;
    m_Start = -1;
    m_End = -1;

    for(int i=0; i<catalogIndexes.size(); i++){
        m_LineCatalogIndexes.push_back(catalogIndexes[i]);
    }

    //initialize fitted amplitude
    SetFittedAmplitude(-1.0, -1.0);
}

CSingleLine::~CSingleLine()
{
}

std::string CSingleLine::GetRayName(Int32 subeIdx)
{
    return m_Ray.GetName();
}


Float64 CSingleLine::GetSignFactor(Int32 subeIdx)
{
    return m_SignFactor;
}

Float64 CSingleLine::GetWidth(Int32 subeIdx, Float64 redshift)
{
    Float64 mu = m_Ray.GetPosition()*(1+redshift);
    Float64 c = GetLineWidth(mu, redshift);

    return c;
}

void CSingleLine::prepareSupport(const CSpectrumSpectralAxis& spectralAxis, Float64 redshift, const TFloat64Range &lambdaRange)
{
    Float64 mu = m_Ray.GetPosition()*(1+redshift);
    Float64 c = GetLineWidth(mu, redshift);
    Float64 winsize = m_NSigmaSupport*c;
    Float64 lambda_start = mu-winsize/2.0;
    if(lambda_start < lambdaRange.GetBegin()){
        lambda_start = lambdaRange.GetBegin();
    }
    m_Start = spectralAxis.GetIndexAtWaveLength(lambda_start);

    Float64 lambda_end = mu+winsize/2.0;
    if(lambda_end > lambdaRange.GetEnd()){
        lambda_end = lambdaRange.GetEnd();
    }
    m_End = spectralAxis.GetIndexAtWaveLength(lambda_end);

    Int32 minLineOverlap = m_OutsideLambdaRangeOverlapThreshold*winsize;
    if( m_Start >= (spectralAxis.GetSamplesCount()-1-minLineOverlap) || m_End <=minLineOverlap){
        m_OutsideLambdaRange=true;
    }else{
        m_OutsideLambdaRange=false;
    }

    if(m_OutsideLambdaRange){
        m_FittedAmplitude = -1.0;
        m_FittedAmplitudeErrorSigma = -1.0;
        return;
    }
}

TInt32RangeList CSingleLine::getSupport()
{
    TInt32RangeList support;
    if(m_OutsideLambdaRange==false){
        support.push_back(TInt32Range(m_Start, m_End));
    }
    return support;
}

TInt32Range CSingleLine::getSupportSubElt(Int32 subeIdx)
{
    TInt32Range support;
     if(m_OutsideLambdaRange==false){
         support = TInt32Range(m_Start, m_End);
     }
     return support;

}

Float64 CSingleLine::GetLineWidth(Float64 lambda, Float64 z){
    Float64 instrumentSigma = -1;
    if( m_LineWidthType == nWidthType_PSFInstrumentDriven){
        instrumentSigma = lambda/m_Resolution/m_FWHM_factor;
    }else if( m_LineWidthType == nWidthType_ZDriven){
        instrumentSigma = m_NominalWidth*(1+z);
    }else if( m_LineWidthType == nWidthType_Fixed){
        instrumentSigma = m_NominalWidth;
    }

//    Float64 v = 200;
//    Float64 c = 300000.0;
//    Float64 velocitySigma = v/c*lambda;//, useless /(1+z)*(1+z);
//    Float64 sigma = sqrt(instrumentSigma*instrumentSigma + velocitySigma*velocitySigma);

    return instrumentSigma;
}

void CSingleLine::fitAmplitude(const CSpectrumSpectralAxis& spectralAxis, const CSpectrumFluxAxis& fluxAxis, Float64  redshift)
{
    if(m_OutsideLambdaRange){
        m_FittedAmplitude = -1.0;
        m_FittedAmplitudeErrorSigma = -1.0;
        return;
    }


    const Float64* flux = fluxAxis.GetSamples();
    const Float64* spectral = spectralAxis.GetSamples();
    const Float64* error = fluxAxis.GetError();
    Float64 mu = m_Ray.GetPosition()*(1+redshift);
    Float64 c = GetLineWidth(mu, redshift);

    Float64 y = 0.0;
    Float64 x = 0.0;
    Float64 yg = 0.0;

    Float64 sumCross = 0.0;
    Float64 sumGauss = 0.0;
    Float64 err2 = 0.0;
    Int32 num = 0;

    //A estimation
    for ( Int32 i = m_Start; i <= m_End; i++)
    {
        y = flux[i];
        x = spectral[i];
        yg = m_SignFactor * exp (-1.*(x-mu)*(x-mu)/(2*c*c));

        num++;
        err2 = 1.0 / (error[i] * error[i]);
        //err2 = 1.0;
        sumCross += yg*y*err2;
        sumGauss += yg*yg*err2;
    }

    if ( num==0 || sumGauss==0 )
    {
        return;
    }

    m_FittedAmplitude = sumCross / sumGauss;
    if(m_FittedAmplitude < 0){
        m_FittedAmplitude = 0.0;
        m_FittedAmplitudeErrorSigma = 1.0/sqrt(sumGauss) - sumCross / sumGauss; //warning: sigma estimated = rough approx.
    }else{
        m_FittedAmplitudeErrorSigma = 1.0/sqrt(sumGauss);
//        //SNR estimation
//        Float64 SNRThres = 1.0;
//        if(m_FittedAmplitudeErrorSigma/m_FittedAmplitudeErrorSigma < SNRThres){
//            m_FittedAmplitudeErrorSigma = 0;
//            m_FittedAmplitude=0;
//        }
    }


    return;

}


//Float64 CSingleLine::FitAmplitudeIterative( const CSpectrumSpectralAxis& spectralAxis, const CSpectrumFluxAxis& fluxAxis, Float64 lambda, Float64 width, Int32 start, Int32 end)
//{
//    Float64 A = boost::numeric::bounds<float>::lowest();
//    const Float64* flux = fluxAxis.GetSamples();
//    const Float64* spectral = spectralAxis.GetSamples();
//    const Float64* error = fluxAxis.GetError();

//    //A first guess
//    for ( Int32 i = start; i < end; i++)
//    {
//        Float64 y = flux[i];
//        if(y>A){
//            A = y;
//        }
//    }

//    if(A<=0){
//        return 0.0;
//    }
//    //A fitting iteration loop
//    A = A*1.5;
//    Float64 mu = lambda;
//    Float64 c = width;
//    Float64 thres = 1e-5;
//    Int32 maxIteration = 100;
//    Float64 AstepDown = A/((Float64)(maxIteration+1));
//    Float64 sum2 = boost::numeric::bounds<float>::highest();
//    Float64 sum2prev = boost::numeric::bounds<float>::highest();
//    Int32 icmpt = 0;
//    while( sum2prev>=sum2 && sum2>thres && icmpt<maxIteration){
//        sum2prev = sum2;
//        sum2 = 0.0;
//        for ( Int32 i = start; i < end; i++)
//        {
//            Float64 x = spectral[i];
//            Float64 Yi = A * exp (-1.*(x-mu)*(x-mu)/(2*c*c));
//            //sum2 += Yi-flux[i];
//            sum2 += pow( Yi - flux[i] , 2.0 ) / pow( error[i], 2.0 );
//        }
//        //sum2 /= (Float64)(end-start+1);
//        icmpt++;
//        A = A-AstepDown;
//    }

//    if(A<0){
//        A=0;
//    }
//    return A;
//}

void CSingleLine::addToSpectrumModel(const CSpectrumSpectralAxis& modelspectralAxis, CSpectrumFluxAxis& modelfluxAxis, Float64 redshift )
{
    if(m_OutsideLambdaRange){
        return;
    }

    Float64 A = m_FittedAmplitude;
    if(A==0){
        return;
    }

    Float64* flux = modelfluxAxis.GetSamples();
    const Float64* spectral = modelspectralAxis.GetSamples();

    for ( Int32 i = m_Start; i <= m_End; i++)
    {
        Float64 x = spectral[i];
        Float64 Yi = getModelAtLambda( x, redshift );
        flux[i] += Yi;
    }

  return;
}

Float64 CSingleLine::getModelAtLambda(Float64 lambda, Float64 redshift )
{
    if(m_OutsideLambdaRange){
        return 0.0;
    }
    Float64 Yi=0.0;

    Float64 x = lambda;

    Float64 A = m_FittedAmplitude;
    Float64 mu = m_Ray.GetPosition()*(1+redshift);
    Float64 c = GetLineWidth(mu, redshift);
    Yi = m_SignFactor * A * exp (-1.*(x-mu)*(x-mu)/(2*c*c));

    return Yi;
}

void CSingleLine::initSpectrumModel(CSpectrumFluxAxis& modelfluxAxis, CSpectrumFluxAxis &continuumfluxAxis)
{
    if(m_OutsideLambdaRange){
        return;
    }


    Float64* flux = modelfluxAxis.GetSamples();
    for ( Int32 i = m_Start; i <= m_End; i++)
    {
        flux[i] = continuumfluxAxis[i];
    }

  return;
}

Float64 CSingleLine::GetFittedAmplitude(Int32 subeIdx){
//    if(m_OutsideLambdaRange){
//        m_FittedAmplitude = -1;
//    }

    return m_FittedAmplitude;
}

Float64 CSingleLine::GetFittedAmplitudeErrorSigma(Int32 subeIdx){
//    if(m_OutsideLambdaRange){
//        m_FittedAmplitude = -1;
//    }

    return m_FittedAmplitudeErrorSigma;
}

Float64 CSingleLine::GetNominalAmplitude(Int32 subeIdx){
    return 1.0;
}


Float64 CSingleLine::GetElementAmplitude(){
//    if(m_OutsideLambdaRange){
//        m_FittedAmplitude = -1;
//    }

    return m_FittedAmplitude;
}


void CSingleLine::SetFittedAmplitude(Float64 A, Float64 SNR)
{
    if(m_OutsideLambdaRange){
        m_FittedAmplitude = -1;
        m_FittedAmplitudeErrorSigma = -1;
    }else{
        m_FittedAmplitude = std::max(0.0, A);
        m_FittedAmplitudeErrorSigma = SNR;
    }
}

void CSingleLine::LimitFittedAmplitude(Int32 subeIdx, Float64 limit){
    if(m_FittedAmplitude > limit){
        m_FittedAmplitude = std::max(0.0, limit);
    }
}

Int32 CSingleLine::FindElementIndex(std::string LineTagStr)
{
    Int32 idx = -1;

    std::string name = m_Ray.GetName();
    std::size_t foundstra = name.find(LineTagStr.c_str());

    if (foundstra!=std::string::npos){
        idx = 0;
    }

    return idx;
}


bool CSingleLine::IsOutsideLambdaRange(Int32 subeIdx){
    return m_OutsideLambdaRange;
}
