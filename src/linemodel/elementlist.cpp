#include <epic/redshift/linemodel/elementlist.h>
#include <epic/redshift/linemodel/singleline.h>
#include <epic/redshift/linemodel/multiline.h>

#include <epic/core/debug/assert.h>
#include <epic/core/log/log.h>

#include <algorithm>

using namespace NSEpic;

CLineModelElementList::CLineModelElementList( const CSpectrum& spectrum, const CRayCatalog::TRayVector& restRayList)
{
    //PFS
    m_nominalWidthDefaultEmission = 4.0; //suited to PFS RJLcont simulations
    m_nominalWidthDefaultAbsorption = 4.0; //suited to PFS LBG simulations

    //m_nominalWidthDefaultEmission = 3.3; //suited to PFS RJLcont simulations
    //m_nominalWidthDefaultAbsorption = 2.3; //suited to PFS LBG simulations
    //VVDS
    //m_nominalWidthDefaultEmission = 3.3; //VVDS DEEP
    //m_nominalWidthDefaultAbsorption = m_nominalWidthDefaultEmission; //


    LoadCatalog(restRayList);
    //LoadCatalogMultilineBalmer(restRayList);
    //LoadCatalogSingleLines(restRayList);
    m_RestRayList = restRayList;
    m_SpectrumModel = new CSpectrum(spectrum);
    m_SpcFluxAxis = spectrum.GetFluxAxis();
}

CLineModelElementList::~CLineModelElementList()
{
}

const CSpectrum& CLineModelElementList::GetModelSpectrum() const
{
    return *m_SpectrumModel;
}

void CLineModelElementList::LoadCatalog(const CRayCatalog::TRayVector& restRayList)
{
    //Load OIII multilines
    std::vector<Int32> OIIIaIdx = findLineIdxInCatalog( restRayList, "[OIII](doublet-1)", CRay::nType_Emission);
    std::vector<Int32> OIIIbIdx = findLineIdxInCatalog( restRayList, "[OIII](doublet-1/3)", CRay::nType_Emission);
    if(OIIIaIdx.size()==1 && OIIIbIdx.size()==1){
        addDoubleLine(restRayList[OIIIaIdx[0]], restRayList[OIIIbIdx[0]], OIIIaIdx[0], OIIIbIdx[0], m_nominalWidthDefaultEmission, 1.0, 1.0/3.0);
    }else{
        if(OIIIaIdx.size()==1){
            addSingleLine(restRayList[OIIIaIdx[0]], OIIIaIdx[0], m_nominalWidthDefaultEmission);
        }
        if(OIIIbIdx.size()==1){
            addSingleLine(restRayList[OIIIbIdx[0]], OIIIbIdx[0], m_nominalWidthDefaultEmission);
        }
    }

    //Load NII multilines
    std::vector<Int32> NII1dx = findLineIdxInCatalog( restRayList, "[NII](doublet-1)", CRay::nType_Emission);
    std::vector<Int32> NII2dx = findLineIdxInCatalog( restRayList, "[NII](doublet-1/2.95)", CRay::nType_Emission);
    if(NII1dx.size()==1 && NII2dx.size()==1){
        addDoubleLine(restRayList[NII1dx[0]], restRayList[NII2dx[0]], NII1dx[0], NII2dx[0], m_nominalWidthDefaultEmission, 1.0, 1.0/2.95);
    }else{
        if(NII1dx.size()==1){
            addSingleLine(restRayList[NII1dx[0]], NII1dx[0], m_nominalWidthDefaultEmission);
        }
        if(NII2dx.size()==1){
            addSingleLine(restRayList[NII2dx[0]], NII2dx[0], m_nominalWidthDefaultEmission);
        }
    }

    //Load OII line doublet
    std::vector<Int32> OII1dx = findLineIdxInCatalog( restRayList, "[OII]3729", CRay::nType_Emission);
    std::vector<Int32> OII2dx = findLineIdxInCatalog( restRayList, "[OII]3726", CRay::nType_Emission);
    if(OII1dx.size()==1 && OII2dx.size()==1){
        addDoubleLine(restRayList[OII1dx[0]], restRayList[OII2dx[0]], OII1dx[0], OII2dx[0], m_nominalWidthDefaultEmission, 1.0, 1.0/2.9);
    }else{
        if(OII1dx.size()==1){
            addSingleLine(restRayList[OII1dx[0]], OII1dx[0], m_nominalWidthDefaultEmission);
        }
        if(OII2dx.size()==1){
            addSingleLine(restRayList[OII2dx[0]], OII2dx[0], m_nominalWidthDefaultEmission);
        }
    }


    //Load FeII 2586/2600 multilines
    std::vector<Int32> aIdx = findLineIdxInCatalog( restRayList, "FeII2600", CRay::nType_Absorption);
    std::vector<Int32> bIdx = findLineIdxInCatalog( restRayList, "FeII2586", CRay::nType_Absorption);
    std::vector<Int32> cIdx = findLineIdxInCatalog( restRayList, "FeII2382", CRay::nType_Absorption);
    std::vector<Int32> dIdx = findLineIdxInCatalog( restRayList, "FeII2374", CRay::nType_Absorption);
    std::vector<Int32> eIdx = findLineIdxInCatalog( restRayList, "FeII2344", CRay::nType_Absorption);
    std::vector<Int32> fIdx = findLineIdxInCatalog( restRayList, "FeII2260", CRay::nType_Absorption);
    std::vector<Int32> gIdx = findLineIdxInCatalog( restRayList, "FeII2249", CRay::nType_Absorption);
    std::vector<Int32> hIdx = findLineIdxInCatalog( restRayList, "FeII1608", CRay::nType_Absorption);
    std::vector<CRay> lines;
    std::vector<Float64> amps;
    std::vector<Int32> inds;
    if(aIdx.size()==1){
        lines.push_back(restRayList[aIdx[0]]);
        amps.push_back(0.239);//2600
        inds.push_back(aIdx[0]);
    }
    if(bIdx.size()==1){
        lines.push_back(restRayList[bIdx[0]]);
        amps.push_back(0.0691); //2586
        inds.push_back(bIdx[0]);
    }
    if(cIdx.size()==1){
        lines.push_back(restRayList[cIdx[0]]);
        amps.push_back(0.32); //2382
        inds.push_back(cIdx[0]);
    }
    if(dIdx.size()==1){
        lines.push_back(restRayList[dIdx[0]]);
        amps.push_back(0.0313); //2374
        inds.push_back(dIdx[0]);
    }
    if(eIdx.size()==1){
        lines.push_back(restRayList[eIdx[0]]);
        amps.push_back(0.114); //2344
        inds.push_back(eIdx[0]);
    }
    if(fIdx.size()==1){
        lines.push_back(restRayList[fIdx[0]]);
        amps.push_back(0.00244); //2260
        inds.push_back(fIdx[0]);
    }
    if(gIdx.size()==1){
        lines.push_back(restRayList[gIdx[0]]);
        amps.push_back(0.001821);//2249
        inds.push_back(gIdx[0]);
    }
    if(hIdx.size()==1){
        lines.push_back(restRayList[hIdx[0]]);
        amps.push_back(0.058);//1608
        inds.push_back(hIdx[0]);
    }
    m_Elements.push_back(boost::shared_ptr<CLineModelElement> (new CMultiLine(lines, amps, m_nominalWidthDefaultAbsorption, inds)));


    //Load MgII multilines
    aIdx = findLineIdxInCatalog( restRayList, "MgII2803", CRay::nType_Absorption);
    bIdx = findLineIdxInCatalog( restRayList, "MgII2796", CRay::nType_Absorption);
    std::vector<CRay> linesMgII;
    std::vector<Float64> ampsMgII;
    std::vector<Int32> indsMgII;
    if(aIdx.size()==1){
        linesMgII.push_back(restRayList[aIdx[0]]);
        ampsMgII.push_back(0.3054);//MgII2803
        indsMgII.push_back(aIdx[0]);
    }
    if(bIdx.size()==1){
        linesMgII.push_back(restRayList[bIdx[0]]);
        ampsMgII.push_back(0.6123); //MgII2796
        indsMgII.push_back(bIdx[0]);
    }
    m_Elements.push_back(boost::shared_ptr<CLineModelElement> (new CMultiLine(linesMgII, ampsMgII, m_nominalWidthDefaultAbsorption, indsMgII)));


    //Load SiIV multilines
    aIdx = findLineIdxInCatalog( restRayList, "SiIV1402", CRay::nType_Absorption);
    bIdx = findLineIdxInCatalog( restRayList, "SiIV1393", CRay::nType_Absorption);
    std::vector<CRay> linesSiIV;
    std::vector<Float64> ampsSiIV;
    std::vector<Int32> indsSiIV;
    if(aIdx.size()==1){
        linesSiIV.push_back(restRayList[aIdx[0]]);
        ampsSiIV.push_back(0.262);//SiIV1402
        indsSiIV.push_back(aIdx[0]);
    }
    if(bIdx.size()==1){
        linesSiIV.push_back(restRayList[bIdx[0]]);
        ampsSiIV.push_back(0.528); //SiIV1393
        indsSiIV.push_back(bIdx[0]);
    }
    m_Elements.push_back(boost::shared_ptr<CLineModelElement> (new CMultiLine(linesSiIV, ampsSiIV, m_nominalWidthDefaultAbsorption, indsSiIV)));

    //Load CIV multilines
    aIdx = findLineIdxInCatalog( restRayList, "CIV1550", CRay::nType_Absorption);
    bIdx = findLineIdxInCatalog( restRayList, "CIV1548", CRay::nType_Absorption);
    std::vector<CRay> linesCIV;
    std::vector<Float64> ampsCIV;
    std::vector<Int32> indsCIV;
    if(aIdx.size()==1){
        linesCIV.push_back(restRayList[aIdx[0]]);
        ampsCIV.push_back(0.09522);//CIV1550
        indsCIV.push_back(aIdx[0]);
    }
    if(bIdx.size()==1){
        linesCIV.push_back(restRayList[bIdx[0]]);
        ampsCIV.push_back(0.1908); //CIV1548
        indsCIV.push_back(bIdx[0]);
    }
    m_Elements.push_back(boost::shared_ptr<CLineModelElement> (new CMultiLine(linesCIV, ampsCIV, m_nominalWidthDefaultAbsorption, indsCIV)));

//    //Load CaII multilines
//    aIdx = findLineIdxInCatalog( restRayList, "CaII_H", CRay::nType_Absorption);
//    bIdx = findLineIdxInCatalog( restRayList, "CaII_K", CRay::nType_Absorption);
//    std::vector<CRay> linesCaII;
//    std::vector<Float64> ampsCaII;
//    std::vector<Int32> indsCaII;
//    if(aIdx.size()==1){
//        linesCaII.push_back(restRayList[aIdx[0]]);
//        ampsCaII.push_back(1.0);//CaII_H
//        indsCaII.push_back(aIdx[0]);
//    }
//    if(bIdx.size()==1){
//        linesCaII.push_back(restRayList[bIdx[0]]);
//        ampsCaII.push_back(0.6); //CaII_K
//        indsCaII.push_back(bIdx[0]);
//    }
//    m_Elements.push_back(boost::shared_ptr<CLineModelElement> (new CMultiLine(linesCaII, ampsCaII, m_nominalWidthDefaultAbsorption, indsCaII)));


    //Load the rest of the single lines
    for( UInt32 iRestRay=0; iRestRay<restRayList.size(); iRestRay++ )
    {
        if ( FindElementIndex(iRestRay)==-1 )
        {
            Float64 nominalwidth = m_nominalWidthDefaultEmission;
            if(restRayList[iRestRay].GetType() == CRay::nType_Emission){
                nominalwidth = m_nominalWidthDefaultEmission;
            }else{
                nominalwidth = m_nominalWidthDefaultAbsorption;
            }
            addSingleLine(restRayList[iRestRay], iRestRay, nominalwidth);
        }
    }
}

void CLineModelElementList::LoadCatalogMultilineBalmer(const CRayCatalog::TRayVector& restRayList)
{
    //Load OIII multilines
    std::vector<Int32> OIIIaIdx = findLineIdxInCatalog( restRayList, "[OIII](doublet-1)", CRay::nType_Emission);
    std::vector<Int32> OIIIbIdx = findLineIdxInCatalog( restRayList, "[OIII](doublet-1/3)", CRay::nType_Emission);
    if(OIIIaIdx.size()==1 && OIIIbIdx.size()==1){
        addDoubleLine(restRayList[OIIIaIdx[0]], restRayList[OIIIbIdx[0]], OIIIaIdx[0], OIIIbIdx[0], m_nominalWidthDefaultEmission, 1.0, 1.0/3.0);
    }else{
        if(OIIIaIdx.size()==1){
            addSingleLine(restRayList[OIIIaIdx[0]], OIIIaIdx[0], m_nominalWidthDefaultEmission);
        }
        if(OIIIbIdx.size()==1){
            addSingleLine(restRayList[OIIIbIdx[0]], OIIIbIdx[0], m_nominalWidthDefaultEmission);
        }
    }

    //Load NII multilines
    std::vector<Int32> NII1dx = findLineIdxInCatalog( restRayList, "[NII](doublet-1)", CRay::nType_Emission);
    std::vector<Int32> NII2dx = findLineIdxInCatalog( restRayList, "[NII](doublet-1/2.95)", CRay::nType_Emission);
    if(NII1dx.size()==1 && NII2dx.size()==1){
        addDoubleLine(restRayList[NII1dx[0]], restRayList[NII2dx[0]], NII1dx[0], NII2dx[0], m_nominalWidthDefaultEmission, 1.0, 1.0/2.95);
    }else{
        if(NII1dx.size()==1){
            addSingleLine(restRayList[NII1dx[0]], NII1dx[0], m_nominalWidthDefaultEmission);
        }
        if(NII2dx.size()==1){
            addSingleLine(restRayList[NII2dx[0]], NII2dx[0], m_nominalWidthDefaultEmission);
        }
    }

    //Load OII line doublet
    std::vector<Int32> OII1dx = findLineIdxInCatalog( restRayList, "[OII](doublet-1)", CRay::nType_Emission);
    std::vector<Int32> OII2dx = findLineIdxInCatalog( restRayList, "[OII](doublet-1/3)", CRay::nType_Emission);
    if(OII1dx.size()==1 && OII2dx.size()==1){
        addDoubleLine(restRayList[OII1dx[0]], restRayList[OII2dx[0]], OII1dx[0], OII2dx[0], 3.2, 1.0, 1.0/2.9);
    }else{
        if(OII1dx.size()==1){
            addSingleLine(restRayList[OII1dx[0]], OII1dx[0], m_nominalWidthDefaultEmission);
        }
        if(OII2dx.size()==1){
            addSingleLine(restRayList[OII2dx[0]], OII2dx[0], m_nominalWidthDefaultEmission);
        }
    }

    //Load Balmer multilines
    std::vector<Int32> Halphaidx = findLineIdxInCatalog( restRayList, "Halpha", CRay::nType_Emission);
    std::vector<Int32> Hbetaidx = findLineIdxInCatalog( restRayList, "Hbeta", CRay::nType_Emission);
    std::vector<Int32> Hgammaidx = findLineIdxInCatalog( restRayList, "Hgamma", CRay::nType_Emission);
    std::vector<Int32> Hdeltaidx = findLineIdxInCatalog( restRayList, "Hdelta", CRay::nType_Emission);
    if(Halphaidx.size()==1 && Hbetaidx.size()==1 && Hgammaidx.size()==1 && Hdeltaidx.size()==1){
        std::vector<CRay> lines;
        lines.push_back(restRayList[Halphaidx[0]]);
        lines.push_back(restRayList[Hbetaidx[0]]);
        lines.push_back(restRayList[Hgammaidx[0]]);
        lines.push_back(restRayList[Hdeltaidx[0]]);

        std::vector<Float64> amps;
        amps.push_back(1.0);
        amps.push_back(0.190);
        amps.push_back(0.071);
        amps.push_back(0.035);

        std::vector<Int32> inds;
        inds.push_back(Halphaidx[0]);
        inds.push_back(Hbetaidx[0]);
        inds.push_back(Hgammaidx[0]);
        inds.push_back(Hdeltaidx[0]);

        m_Elements.push_back(boost::shared_ptr<CLineModelElement> (new CMultiLine(lines, amps, m_nominalWidthDefaultEmission, inds)));
    }else{
        if(Halphaidx.size()==1){
            addSingleLine(restRayList[Halphaidx[0]], Halphaidx[0], m_nominalWidthDefaultEmission);
        }
        if(Hbetaidx.size()==1){
            addSingleLine(restRayList[Hbetaidx[0]], Hbetaidx[0], m_nominalWidthDefaultEmission);
        }
        if(Hgammaidx.size()==1){
            addSingleLine(restRayList[Hgammaidx[0]], Hgammaidx[0], m_nominalWidthDefaultEmission);
        }
        if(Hdeltaidx.size()==1){
            addSingleLine(restRayList[Hdeltaidx[0]], Hdeltaidx[0], m_nominalWidthDefaultEmission);
        }
    }

    //Load the rest of the single lines
    for( UInt32 iRestRay=0; iRestRay<restRayList.size(); iRestRay++ )
    {
        if ( FindElementIndex(iRestRay)==-1 )
        {
            addSingleLine(restRayList[iRestRay], iRestRay, m_nominalWidthDefaultEmission);
        }
    }
}

void CLineModelElementList::LoadCatalogSingleLines(const CRayCatalog::TRayVector& restRayList)
{
    //Load OIII lines
    std::vector<Int32> OIIIaIdx = findLineIdxInCatalog( restRayList, "[OIII](doublet-1)", CRay::nType_Emission);
    if(OIIIaIdx.size()==1){
        addSingleLine(restRayList[OIIIaIdx[0]], OIIIaIdx[0], m_nominalWidthDefaultEmission);
    }
    std::vector<Int32> OIIIbIdx = findLineIdxInCatalog( restRayList, "[OIII](doublet-1/3)", CRay::nType_Emission);
    if(OIIIbIdx.size()==1){
        addSingleLine(restRayList[OIIIbIdx[0]], OIIIbIdx[0], m_nominalWidthDefaultEmission);
    }

    //Load NII lines
    std::vector<Int32> NIIdx = findLineIdxInCatalog( restRayList, "[NII]", CRay::nType_Emission);
    if(NIIdx.size()==2){
        addSingleLine(restRayList[NIIdx[0]], NIIdx[0], m_nominalWidthDefaultEmission);
        addSingleLine(restRayList[NIIdx[1]], NIIdx[1], m_nominalWidthDefaultEmission);
    }

    //Load OII line
    std::vector<Int32> OIIdx = findLineIdxInCatalog( restRayList, "[OII]", CRay::nType_Emission);
    if(OIIdx.size()==1){
        addSingleLine(restRayList[OIIdx[0]], OIIdx[0], 3.55);
    }

    //Load the rest of the single lines
    for( UInt32 iRestRay=0; iRestRay<restRayList.size(); iRestRay++ )
    {
        if ( FindElementIndex(iRestRay)==-1 )
        {
            addSingleLine(restRayList[iRestRay], iRestRay, m_nominalWidthDefaultEmission);
        }
    }
}


void CLineModelElementList::fit(Float64 redshift, CLineModelResult::SLineModelSolution& modelSolution)
{
    //initialize the model
    const CSpectrumSpectralAxis& spectralAxis = m_SpectrumModel->GetSpectralAxis();
    CSpectrumFluxAxis& modelFluxAxis = m_SpectrumModel->GetFluxAxis();
    for(UInt32 i=0; i<modelFluxAxis.GetSamplesCount(); i++){
        modelFluxAxis[i] = 0.0;
    }

    //fit the model amplitudes
    for( UInt32 iElts=0; iElts<m_Elements.size(); iElts++ )
    {
        m_Elements[iElts]->fitAmplitude(spectralAxis, m_SpcFluxAxis, redshift);
    }

    //eventually apply rules,
    // WARNING: no noise taken into account for now...
    applyRules();

    //create spectrum model
    modelSolution = GetModelSolution();
    for( UInt32 iElts=0; iElts<m_Elements.size(); iElts++ )
    {
        m_Elements[iElts]->addToSpectrumModel(spectralAxis, modelFluxAxis, redshift);
    }
}

std::vector<int> CLineModelElementList::findLineIdxInCatalog(const CRayCatalog::TRayVector& restRayList, std::string strTag, Int32 type)
{
    std::vector<Int32> indexes;
    for( UInt32 iRestRay=0; iRestRay<restRayList.size(); iRestRay++ )
    {
        if(restRayList[iRestRay].GetType() != type){
            continue;
        }
        std::string name = restRayList[iRestRay].GetName();
        std::size_t foundstra = name.find(strTag.c_str());
        if (foundstra!=std::string::npos){
            indexes.push_back(iRestRay);
        }
    }
    return indexes;
}

void CLineModelElementList::addSingleLine(const CRay &r, Int32 index, Float64 nominalWidth)
{
    //CSingleLine line = CSingleLine(r, nominalWidth);
    std::vector<Int32> a;
    a.push_back(index);
    //CSingleLine c(r, nominalWidth, a);
    m_Elements.push_back(boost::shared_ptr<CLineModelElement> (new CSingleLine(r, nominalWidth, a)));
    //m_Elements.push_back(new CSingleLine(r, nominalWidth, a));
}

void CLineModelElementList::addDoubleLine(const CRay &r1, const CRay &r2, Int32 index1, Int32 index2, Float64 nominalWidth, Float64 a1, Float64 a2)
{
    std::vector<CRay> lines;
    lines.push_back(r1);
    lines.push_back(r2);

    std::vector<Float64> amps;
    amps.push_back(a1);
    amps.push_back(a2);

    std::vector<Int32> a;
    a.push_back(index1);
    a.push_back(index2);
    //CSingleLine c(r, nominalWidth, a);
    m_Elements.push_back(boost::shared_ptr<CLineModelElement> (new CMultiLine(lines, amps, nominalWidth, a)));
    //m_Elements.push_back(new CSingleLine(r, nominalWidth, a));
}

void CLineModelElementList::applyRules()
{
    //todo: check for the noise when applying amplitudes rules...

    Apply2SingleLinesAmplitudeRule("Halpha", "Hbeta", 1.0/2.86);
    Apply2SingleLinesAmplitudeRule("Hbeta", "Hgamma", 0.47);
    Apply2SingleLinesAmplitudeRule("Hgamma", "Hdelta", 1.0);
//    Apply2SingleLinesAmplitudeRule("Hdelta", "Hepsilon", 1.0);
//    Apply2SingleLinesAmplitudeRule("Hepsilon", "H8", 1.0);
//    Apply2SingleLinesAmplitudeRule("H8", "H9", 1.0);
//    Apply2SingleLinesAmplitudeRule("H9", "H10", 1.0);
//    Apply2SingleLinesAmplitudeRule("H10", "H11", 1.0);

    //ApplyStrongHigherWeakRule(CRay::nType_Emission);
    //ApplyStrongHigherWeakRule(CRay::nType_Absorption);

    //    //*
    //    Float64 doublettol=0.2;
    //    Apply2LinesAmplitudeRule(restRayList, modelSolution.Amplitudes, modelSolution.OutsideLambdaRange, "[OIII](doublet-1)", "[OIII](doublet-1/3)", 0.334*(1.0+doublettol));
    //    Apply2LinesAmplitudeRule(restRayList, modelSolution.Amplitudes, modelSolution.OutsideLambdaRange, "[OIII](doublet-1/3)", "[OIII](doublet-1)", (1.0+doublettol)/0.334);
    //    Apply2LinesAmplitudeRule(restRayList, modelSolution.Amplitudes, modelSolution.OutsideLambdaRange, "Halpha", "Hbeta", 1.0/2.86);
    //    Apply2LinesAmplitudeRule(restRayList, modelSolution.Amplitudes, modelSolution.OutsideLambdaRange, "Hbeta", "Hgamma", 0.47);
    //    Apply2LinesAmplitudeRule(restRayList, modelSolution.Amplitudes, modelSolution.OutsideLambdaRange, "Hgamma", "Hdelta", 1.0);
    //    Apply2LinesAmplitudeRule(restRayList, modelSolution.Amplitudes, modelSolution.OutsideLambdaRange, "[NII](doublet-1)", "[NII](doublet-1/2.95)", (1.0+doublettol)/2.95);
    //    Apply2LinesAmplitudeRule(restRayList, modelSolution.Amplitudes, modelSolution.OutsideLambdaRange, "[NII](doublet-1/2.95)", "[NII](doublet-1)", 2.95*(1.0+doublettol));
    //    //*/
}

Void CLineModelElementList::ApplyStrongHigherWeakRule( Int32 linetype )
{
    Float64 coeff = 1.0;
    Float64 maxiStrong = FindHighestStrongLineAmp(linetype);


    for( UInt32 iRestRayWeak=0; iRestRayWeak<m_RestRayList.size(); iRestRayWeak++ ) //loop on the strong lines
    {
        if(m_RestRayList[iRestRayWeak].GetForce() != CRay::nForce_Weak){
            continue;
        }
        if(m_RestRayList[iRestRayWeak].GetType() != linetype){
            continue;
        }
        Int32 eIdxWeak = FindElementIndex(iRestRayWeak);
        Int32 subeIdxWeak = m_Elements[eIdxWeak]->FindElementIndex(iRestRayWeak);

        if(m_Elements[eIdxWeak]->IsOutsideLambdaRange(subeIdxWeak) == true){
            continue;
        }

        Float64 ampStrong = maxiStrong;
        m_Elements[eIdxWeak]->LimitFittedAmplitude(subeIdxWeak, coeff*ampStrong);

    }

}

Float64 CLineModelElementList::FindHighestStrongLineAmp( Int32 linetype )
{
    Float64 maxi = -1.0;
    for( UInt32 iRestRayStrong=0; iRestRayStrong<m_RestRayList.size(); iRestRayStrong++ ) //loop on the strong lines
    {
        if(m_RestRayList[iRestRayStrong].GetForce() != CRay::nForce_Strong){
            continue;
        }
        if(m_RestRayList[iRestRayStrong].GetType() != linetype){
            continue;
        }

        Int32 eIdxStrong = FindElementIndex(iRestRayStrong);
        Int32 subeIdxStrong = m_Elements[eIdxStrong]->FindElementIndex(iRestRayStrong);

        if(m_Elements[eIdxStrong]->IsOutsideLambdaRange(subeIdxStrong) == true){
            continue;
        }


        Float64 ampStrong = m_Elements[eIdxStrong]->GetFittedAmplitude(subeIdxStrong);
        if(maxi<ampStrong){
            maxi = ampStrong;
        }
    }
    return maxi;
}

Void CLineModelElementList::Apply2SingleLinesAmplitudeRule(std::string lineA, std::string lineB, Float64 coeff )
{
    Int32 iA = FindElementIndex(lineA);
    if(m_Elements[iA]->GetSize()>1){
        iA=-1;
    }
    Int32 iB = FindElementIndex(lineB);
    if(m_Elements[iB]->GetSize()>1){
        iB=-1;
    }
    if(iA==-1 || iB==-1 || iA==iB){
        return;
    }

    if(m_Elements[iA]->IsOutsideLambdaRange() == false){
        Float64 ampA = m_Elements[iA]->GetFittedAmplitude(0);
        m_Elements[iB]->LimitFittedAmplitude(0, coeff*ampA);
    }
}

CLineModelResult::SLineModelSolution CLineModelElementList::GetModelSolution()
{
    CLineModelResult::SLineModelSolution modelSolution;
    for( UInt32 iRestRay=0; iRestRay<m_RestRayList.size(); iRestRay++ )
    {
        Int32 eIdx = FindElementIndex(iRestRay);
        Int32 subeIdx = m_Elements[eIdx]->FindElementIndex(iRestRay);

        //modelSolution.fittingIndexRange.push_back( m_Elements[eIdx].);
        modelSolution.Amplitudes.push_back(m_Elements[eIdx]->GetFittedAmplitude(subeIdx));
        //modelSolution.Widths.push_back(-1.0);
        //modelSolution.OutsideLambdaRange.push_back(true);
    }

    return modelSolution;
}

Int32 CLineModelElementList::FindElementIndex(Int32 LineCatalogIndex)
{
    Int32 idx = -1;
    for( UInt32 iElts=0; iElts<m_Elements.size(); iElts++ )
    {
        if(m_Elements[iElts]->FindElementIndex(LineCatalogIndex) !=-1){
            idx = iElts;
            break;
        }
    }
    return idx;
}

Int32 CLineModelElementList::FindElementIndex(std::string LineTagStr)
{
    Int32 idx = -1;
    for( UInt32 iElts=0; iElts<m_Elements.size(); iElts++ )
    {
        if(m_Elements[iElts]->FindElementIndex(LineTagStr) !=-1){
            idx = iElts;
            break;
        }
    }
    return idx;
}

