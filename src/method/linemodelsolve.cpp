

#include <epic/redshift/method/linemodelsolve.h>

#include <epic/core/debug/assert.h>
#include <epic/redshift/spectrum/template/catalog.h>
#include <epic/redshift/operator/linemodel.h>
#include <epic/redshift/extremum/extremum.h>
#include <epic/redshift/processflow/datastore.h>

using namespace NSEpic;
using namespace std;

IMPLEMENT_MANAGED_OBJECT( CLineModelSolve )

CLineModelSolve::CLineModelSolve()
{

}

CLineModelSolve::~CLineModelSolve()
{

}


const CLineModelSolveResult* CLineModelSolve::Compute(  CDataStore& resultStore, const CSpectrum& spc, const CSpectrum& spcWithoutCont, const CRayCatalog& restraycatalog,
                                                        const TFloat64Range& lambdaRange, const TFloat64List& redshifts, Int32 spcType)
{
    Bool storeResult = false;
    CDataStore::CAutoScope resultScope( resultStore, "linemodelsolve" );

    Solve( resultStore, spc, spcWithoutCont, restraycatalog, lambdaRange, redshifts, spcType);
    storeResult = true;


    if( storeResult )
    {
        CLineModelSolveResult*  result = new CLineModelSolveResult();
        return result;
    }

    return NULL;
}

Bool CLineModelSolve::Solve( CDataStore& resultStore, const CSpectrum& spc, const CSpectrum& spcWithoutCont, const CRayCatalog& restraycatalog,
                             const TFloat64Range& lambdaRange, const TFloat64List& redshifts, Int32 spcType )
{
    CSpectrum _spc;

    std::string scopeStr = "linemodel";
    Int32 _spctype = spcType;

    if(_spctype == CLineModelSolveResult::nType_continuumOnly){
        // use continuum only
        _spc = spc;
        CSpectrumFluxAxis spcfluxAxis = _spc.GetFluxAxis();
        spcfluxAxis.Subtract(spcWithoutCont.GetFluxAxis());
        CSpectrumFluxAxis& sfluxAxisPtr = _spc.GetFluxAxis();
        sfluxAxisPtr = spcfluxAxis;
        //scopeStr = "linemodel_continuum";
    }else if(_spctype == CLineModelSolveResult::nType_raw){
        // use full spectrum
        _spc = spc;
        //scopeStr = "linemodel";
    }else if(_spctype == CLineModelSolveResult::nType_noContinuum){
        // use spectrum without continuum
        _spc = spc;
        CSpectrumFluxAxis spcfluxAxis = spcWithoutCont.GetFluxAxis();
        CSpectrumFluxAxis& sfluxAxisPtr = _spc.GetFluxAxis();
        sfluxAxisPtr = spcfluxAxis;
        //scopeStr = "linemodel_nocontinuum";
    }

    // Compute merit function
    COperatorLineModel linemodel;
    CRef<CLineModelResult>  result = (CLineModelResult*)linemodel.Compute( _spc, restraycatalog, lambdaRange, redshifts);

    if( !result )
    {
        //Log.LogInfo( "Failed to compute linemodel");
        return false;
    }else{
        // Store results
        resultStore.StoreScopedGlobalResult( scopeStr.c_str(), *result );
    }


    return true;
}
