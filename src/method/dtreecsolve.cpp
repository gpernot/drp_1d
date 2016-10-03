#include <epic/redshift/method/dtreecsolve.h>
#include <epic/redshift/method/dtreecsolveresult.h>

#include <epic/core/log/log.h>
#include <epic/core/debug/assert.h>
#include <epic/redshift/spectrum/template/catalog.h>
#include <epic/redshift/extremum/extremum.h>
#include <epic/redshift/processflow/datastore.h>

#include <epic/redshift/operator/chisquare2.h>
#include <epic/redshift/method/chisquare2solve.h>

#include <epic/redshift/operator/linemodel.h>


#include <float.h>

using namespace NSEpic;
using namespace std;


COperatorDTreeCSolve::COperatorDTreeCSolve()
{

}

COperatorDTreeCSolve::~COperatorDTreeCSolve()
{

}

const std::string COperatorDTreeCSolve::GetDescription()
{
    std::string desc;

    desc = "Method Amazed0_3:\n";

    desc.append("\tparam: linemodel.linetypefilter = {""no"", ""E"", ""A""}\n");
    desc.append("\tparam: linemodel.lineforcefilter = {""no"", ""S""}\n");
    desc.append("\tparam: linemodel.fittingmethod = {""hybrid"", ""individual""}\n");
    desc.append("\tparam: linemodel.continuumcomponent = {""fromspectrum"", ""nocontinuum"", ""zero""}\n");
    desc.append("\tparam: linemodel.linewidthtype = {""instrumentdriven"", ""combined"",  ""nispsim2016"", ""fixed""}\n");
    desc.append("\tparam: linemodel.instrumentresolution = <float value>\n");
    desc.append("\tparam: linemodel.velocityemission = <float value>\n");
    desc.append("\tparam: linemodel.velocityabsorption = <float value>\n");

    desc.append("\tparam: linemodel.rules = {""all"", ""no""}\n");
    desc.append("\tparam: linemodel.continuumreestimation = {""no"", ""onlyextrema"", ""always""}\n");
    desc.append("\tparam: linemodel.extremacount = <float value>\n");
    desc.append("\tparam: linemodel.fastfitlargegridstep = <float value>, deactivated if negative or zero\n");

    desc.append("\tparam: chisquare.overlapthreshold = <float value>\n");
    desc.append("\tparam: chisquare.redshiftsupport = {""full"", ""extremaextended""}\n");
    desc.append("\tparam: chisquare.interpolation = {""precomputedfinegrid"", ""lin""}\n");
    desc.append("\tparam: chisquare.spectrum.component = {""raw"", ""continuum"", ""nocontinuum""}\n");
    desc.append("\tparam: chisquare.extinction = {""no"", ""yes""}\n");


    return desc;

}

std::shared_ptr<const CDTreeCSolveResult> COperatorDTreeCSolve::Compute( CDataStore& resultStore, const CSpectrum& spc, const CSpectrum& spcWithoutCont,
                                                        const CTemplateCatalog& tplCatalog, const TStringList &tplCategoryList, const CRayCatalog &restRayCatalog,
                                                        const TFloat64Range& lambdaRange, const TFloat64List &redshifts)
{
    Bool storeResult = false;

    CDataStore::CAutoScope resultScope( resultStore, "dtreeCsolve" );

    storeResult = Solve(resultStore, spc, spcWithoutCont,
                                       tplCatalog, tplCategoryList, restRayCatalog,
                                       lambdaRange, redshifts );

    //storeResult = true;
    if( storeResult )
    {
        return std::shared_ptr<const CDTreeCSolveResult>( new CDTreeCSolveResult() );
    }

    return NULL;
}

Bool COperatorDTreeCSolve::Solve(CDataStore &dataStore, const CSpectrum &spc, const CSpectrum &spcWithoutCont, const CTemplateCatalog &tplCatalog, const TStringList &tplCategoryList, const CRayCatalog &restRayCatalog, const TFloat64Range &lambdaRange, const TFloat64List &redshifts)
{
    CSpectrum _spcContinuum = spc;
    CSpectrumFluxAxis spcfluxAxis = _spcContinuum.GetFluxAxis();
    spcfluxAxis.Subtract(spcWithoutCont.GetFluxAxis());
    CSpectrumFluxAxis& sfluxAxisPtr = _spcContinuum.GetFluxAxis();
    sfluxAxisPtr = spcfluxAxis;

    std::string opt_linetypefilter;
    dataStore.GetScopedParam( "linemodel.linetypefilter", opt_linetypefilter, "no" );
    std::string opt_lineforcefilter;
    dataStore.GetScopedParam( "linemodel.lineforcefilter", opt_lineforcefilter, "no" );
    std::string opt_fittingmethod;
    dataStore.GetScopedParam( "linemodel.fittingmethod", opt_fittingmethod, "hybrid" );
    std::string opt_continuumcomponent;
    //dataStore.GetScopedParam( "linemodel.continuumcomponent", opt_continuumcomponent, "nocontinuum" );
    dataStore.GetScopedParam( "linemodel.continuumcomponent", opt_continuumcomponent, "fromspectrum" );
    std::string opt_lineWidthType;
    dataStore.GetScopedParam( "linemodel.linewidthtype", opt_lineWidthType, "combined" );
    Float64 opt_resolution;
    dataStore.GetScopedParam( "linemodel.instrumentresolution", opt_resolution, 2350.0 );
    Float64 opt_velocity_emission;
    dataStore.GetScopedParam( "linemodel.velocityemission", opt_velocity_emission, 100.0 );
    Float64 opt_velocity_absorption;
    dataStore.GetScopedParam( "linemodel.velocityabsorption", opt_velocity_absorption, 300.0 );
    std::string opt_velocityfit;
    dataStore.GetScopedParam( "linemodel.velocityfit", opt_velocityfit, "no" );
    std::string opt_continuumreest;
    dataStore.GetScopedParam( "linemodel.continuumreestimation", opt_continuumreest, "no" );
    std::string opt_rules;
    dataStore.GetScopedParam( "linemodel.rules", opt_rules, "all" );
    Float64 opt_extremacount;
    dataStore.GetScopedParam( "linemodel.extremacount", opt_extremacount, 10.0 );
    Float64 opt_twosteplargegridstep;
    dataStore.GetScopedParam( "linemodel.fastfitlargegridstep", opt_twosteplargegridstep, 0.001 );


    //_///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Compute Linemodel
    Log.LogInfo( "dtreeCsolve: Computing the linemodel");
    COperatorLineModel linemodel;
    auto result = dynamic_pointer_cast<CLineModelResult>(linemodel.Compute(dataStore,
                                                                           spc,
                                                                           _spcContinuum,
                                                                           restRayCatalog,
                                                                           opt_linetypefilter,
                                                                           opt_lineforcefilter,
                                                                           lambdaRange,
                                                                           redshifts,
                                                                           opt_extremacount,
                                                                           opt_fittingmethod,
                                                                           opt_continuumcomponent,
                                                                           opt_lineWidthType,
                                                                           opt_resolution,
                                                                           opt_velocity_emission,
                                                                           opt_velocity_absorption,
                                                                           opt_continuumreest,
                                                                           opt_rules,
                                                                           opt_velocityfit,
                                                                           opt_twosteplargegridstep) );

    /*
    Todo: this is the place to decide wether there is a candidate robust enough
    //*/

    if( !result )
    {
        //Log.LogInfo( "Failed to compute linemodel");
        return false;
    }else{
        // Store results
        dataStore.StoreScopedGlobalResult( "linemodel", result );
        //save linemodel fitting and spectrum-model results
        linemodel.storeGlobalModelResults(dataStore);
    }



    //*
    //_///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // compute chisquare
    if( result->Extrema.size() == 0 )
    {
        return false;
    }
    Float64 overlapThreshold;
    dataStore.GetScopedParam( "chisquare.overlapthreshold", overlapThreshold, 1.0 );
    std::string opt_interp;
    dataStore.GetScopedParam( "chisquare.interpolation", opt_interp, "precomputedfinegrid" );
    std::string opt_spcComponent;
    dataStore.GetScopedParam( "chisquare.spectrum.component", opt_spcComponent, "continuum" );
    std::string opt_extinction;
    dataStore.GetScopedParam( "chisquare.extinction", opt_extinction, "no" );

    std::string scopeStr = "chisquare";
    if(opt_spcComponent == "continuum"){
        scopeStr = "chisquare_continuum";
    }else if(opt_spcComponent == "raw"){
        scopeStr = "chisquare";
    }else if(opt_spcComponent == "nocontinuum"){
        scopeStr = "chisquare_nocontinuum";
    }


    Log.LogInfo( "dtreeCsolve: Computing the template fitting : %s", scopeStr.c_str());

    CMethodChisquare2Solve chiSolve;
    std::vector<Float64> redshiftsChi2Continuum;
    /*
    Int32 enableFastContinuumFitLargeGrid = 1;
    TFloat64List redshiftsChi2 = redshifts;
    if(enableFastContinuumFitLargeGrid==1){
        //calculate on a wider grid, defined by a minimum step
        Float64 dz_chi2c_thres = 1e-3;
        std::vector<Int32> removed_inds;
        Int32 lastKeptInd = 0;
        for(Int32 i=1; i<redshiftsChi2.size()-1; i++){
            if( abs(redshiftsChi2[i]-redshiftsChi2[lastKeptInd])<dz_chi2c_thres && abs(redshiftsChi2[i+1]-redshiftsChi2[i])<dz_chi2c_thres ){
                removed_inds.push_back(i);
            }else{
                lastKeptInd=i;
            }
        }
        Int32 rmInd = 0;
        for(Int32 i=1; i<redshiftsChi2.size()-1; i++){
            if(removed_inds[rmInd]==i){
                rmInd++;
            }else{
                redshiftsChi2Continuum.push_back(redshiftsChi2[i]);
            }
        }
    }else{
        redshiftsChi2Continuum=redshiftsChi2;
    }
    //*/


    //*
    //Compute the tpl fitting only on the candidates/extrema
    for( Int32 i=0; i<result->Extrema.size(); i++ )
    {
        redshiftsChi2Continuum.push_back(result->Extrema[i]);
    }
    std::sort(redshiftsChi2Continuum.begin(), redshiftsChi2Continuum.end());
    //*/

    //*/
    auto chisolveResultcontinuum = chiSolve.Compute( dataStore, spc, spcWithoutCont,
                                                                        tplCatalog, tplCategoryList,
                                                                        lambdaRange, redshiftsChi2Continuum, overlapThreshold, opt_spcComponent, opt_interp, opt_extinction);

    if( !chisolveResultcontinuum )
    {
        Log.LogInfo( "dtreeCsolve: Failed to compute tpl fitting continuum");
        return false;
    }
    //_///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //*/



    // Calculate the Combination //////////////////////////////////////////////////
    GetCombinedRedshift(dataStore, scopeStr);
    // /////////////////////////////////////////////////////////////////////////////

    return true;
}


Bool COperatorDTreeCSolve::GetCombinedRedshift(CDataStore& store, std::string scopeStr)
{
    std::string scope = "dtreeCsolve.linemodel";
    auto results = std::dynamic_pointer_cast<const CLineModelResult>( store.GetGlobalResult(scope.c_str()).lock() );


    //*
    //Create the combined grid: extrema only
    TFloat64List zcomb;
    for( Int32 i=0; i<results->Extrema.size(); i++ )
    {
        zcomb.push_back(results->Extrema[i]);
    }
    std::sort(zcomb.begin(), zcomb.end());
    //*/
    /*
    //Create the combined grid: all redshifts
    TFloat64List zcomb;
    for( Int32 i=0; i<results->Redshifts.size(); i++ )
    {
        zcomb.push_back(results->Redshifts[i]);
    }
    //*/

    //*
    //***********************************************************
    //retrieve the extrema results indexes
    std::vector<Int32> idxLMResultsExtrema;
    for( Int32 i=0; i<zcomb.size(); i++ )
    {
        for( Int32 iExtrema=0; iExtrema<results->Extrema.size(); iExtrema++ )
        {
            if(results->Extrema[iExtrema] == zcomb[i]){
                idxLMResultsExtrema.push_back(iExtrema);
                break;
            }
        }
    }

    //*
    //***********************************************************
    //retrieve linemodel values
    TFloat64List chi2lm;

    for( Int32 i=0; i<zcomb.size(); i++ )
    {
        for( Int32 iall=0; iall<results->Redshifts.size(); iall++ )
        {
            if(results->Redshifts[iall] == zcomb[i]){
                chi2lm.push_back(results->ChiSquare[iall]);
                break;
            }
        }
    }
    //Log.LogInfo( "dtreeCsolve : chi2lm size=%d", chi2lm.size());
    //*/

    //*
    //***********************************************************
    //retrieve chi2 continuum values
    TFloat64List chi2continuum;
    TFloat64List chi2continuum_calcGrid;

    TFloat64List zcontinuum_calcGrid;
    Float64 minchi2continuum= DBL_MAX;

    chi2continuum_calcGrid = GetBestRedshiftChi2List(store, scopeStr, minchi2continuum, zcontinuum_calcGrid);

    //option 1: interpolate the continuum results on the continuum fit grid
    if(false){
        Int32 iCalcGrid=0;
        for( Int32 i=0; i<zcomb.size(); i++ )
        {
            if((zcomb[i]>=zcontinuum_calcGrid[iCalcGrid] && iCalcGrid<zcontinuum_calcGrid.size()) || iCalcGrid==0){
                chi2continuum.push_back(chi2continuum_calcGrid[iCalcGrid]);
                iCalcGrid++;
            }else if(iCalcGrid>=zcontinuum_calcGrid.size()){
                chi2continuum.push_back(chi2continuum_calcGrid[zcontinuum_calcGrid.size()-1]);
            }else{
                //interpolate between iCalcGrid-1 and iCalcGrid
                Float64 a = (chi2continuum_calcGrid[iCalcGrid]-chi2continuum_calcGrid[iCalcGrid-1])/(zcontinuum_calcGrid[iCalcGrid]-zcontinuum_calcGrid[iCalcGrid-1]);
                Float64 continuumVal = chi2continuum_calcGrid[iCalcGrid-1]+a*(zcomb[i]-zcontinuum_calcGrid[iCalcGrid-1]);
                chi2continuum.push_back(continuumVal);
            }
        }
    }else{//option 2
        chi2continuum = chi2continuum_calcGrid;
    }



    //*
    //***********************************************************
    //Estimate the continuum prior
    std::shared_ptr<CChisquareResult> resultPriorContinuum = std::shared_ptr<CChisquareResult>( new CChisquareResult() );
    resultPriorContinuum->ChiSquare.resize( zcomb.size() );
    resultPriorContinuum->Redshifts.resize( zcomb.size() );
    resultPriorContinuum->Overlap.resize( zcomb.size() );

    //Set the coefficients
    Float64 chi2cCoeff = 1.0;
    /*
    // coeffs for Keck
    lmCoeff = 1.0;
    chi2cCoeff = 2e-3;
    //*/
    // coeffs for VUDS F34
    chi2cCoeff = 2e-2;
    //*/

    for( Int32 i=0; i<zcomb.size(); i++ )
    {
        Float64 post = DBL_MAX;
        post = chi2cCoeff*chi2continuum[i];

        resultPriorContinuum->ChiSquare[i] = post;
        resultPriorContinuum->Redshifts[i] = zcomb[i];
        resultPriorContinuum->Overlap[i] = -1.0;
    }
    if( resultPriorContinuum ) {
        store.StoreScopedGlobalResult( "priorContinuum", resultPriorContinuum );
    }

    //*
    //***********************************************************
    //Estimate the SELSP prior (strong EL SNR prior)
    std::shared_ptr<CChisquareResult> resultPriorSELSP = std::shared_ptr<CChisquareResult>( new CChisquareResult() );
    resultPriorSELSP->ChiSquare.resize( zcomb.size() );
    resultPriorSELSP->Redshifts.resize( zcomb.size() );
    resultPriorSELSP->Overlap.resize( zcomb.size() );

    for( Int32 i=0; i<zcomb.size(); i++ )
    {
        Float64 coeff =10.0;
        Float64 post = -coeff*results->StrongELSNR[idxLMResultsExtrema[i]];

        resultPriorSELSP->ChiSquare[i] = post;
        resultPriorSELSP->Redshifts[i] = zcomb[i];
        resultPriorSELSP->Overlap[i] = -1.0;
    }
    if( resultPriorSELSP ) {
        store.StoreScopedGlobalResult( "priorStrongELSnrP", resultPriorSELSP );
    }

    //*
    //***********************************************************
    //Estimate the combined merit curve
    std::shared_ptr<CChisquareResult> resultCombined = std::shared_ptr<CChisquareResult>( new CChisquareResult() );
    resultCombined->ChiSquare.resize( zcomb.size() );
    resultCombined->Redshifts.resize( zcomb.size() );
    resultCombined->Overlap.resize( zcomb.size() );

    for( Int32 i=0; i<zcomb.size(); i++ )
    {
        Float64 post = chi2lm[i] + resultPriorSELSP->ChiSquare[i] + resultPriorContinuum->ChiSquare[i];

        resultCombined->ChiSquare[i] = post;
        resultCombined->Redshifts[i] = zcomb[i];
        resultCombined->Overlap[i] = -1.0;
    }

    if( resultCombined ) {
        store.StoreScopedGlobalResult( "resultdtreeCCombined", resultCombined );
    }
    //*/

    return true;
    //*/

}


TFloat64List COperatorDTreeCSolve::GetBestRedshiftChi2List( CDataStore& store, std::string scopeStr,  Float64& minmerit, TFloat64List& zList)
{
    std::string scope = "dtreeCsolve.chisquare2solve.";
    scope.append(scopeStr.c_str());

    TOperatorResultMap meritResults = store.GetPerTemplateResult(scope.c_str());

    TFloat64List meritList;
    //init meritresults
    TOperatorResultMap::const_iterator it0 = meritResults.begin();
    {
        auto meritResult = std::dynamic_pointer_cast<const CChisquareResult> ( (*it0).second );
        for( Int32 i=0; i<meritResult->ChiSquare.size(); i++ )
        {
            meritList.push_back(DBL_MAX);
            zList.push_back(meritResult->Redshifts[i]);
        }
    }

    //find best merit for each tpl
    minmerit = DBL_MAX;
    for( TOperatorResultMap::const_iterator it = meritResults.begin(); it != meritResults.end(); it++ )
    {
        auto meritResult = std::dynamic_pointer_cast<const CChisquareResult>((*it).second);
        for( Int32 i=0; i<meritResult->ChiSquare.size(); i++ )
        {
            if( meritList[i] > meritResult->ChiSquare[i]){
                meritList[i] = meritResult->ChiSquare[i];
            }
            if( minmerit > meritResult->ChiSquare[i]){
                minmerit = meritResult->ChiSquare[i];
            }
        }
    }

    return meritList;

}