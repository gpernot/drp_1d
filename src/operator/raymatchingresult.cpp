#include <epic/redshift/operator/raymatchingresult.h>
#include <stdio.h>
#include <math.h>

using namespace NSEpic;

IMPLEMENT_MANAGED_OBJECT( CRayMatchingResult )

CRayMatchingResult::CRayMatchingResult()
{

}

CRayMatchingResult::~CRayMatchingResult()
{

}

Void CRayMatchingResult::Save( const COperatorResultStore& store, std::ostream& stream ) const
{

    // We should also save best result:

    /*
    const CRayMatchingResult* rayMatchingResult = (CRayMatchingResult*)ctx.GetGlobalResult( "raymatching" );
    if(rayMatchingResult != NULL){
        rayMatchingResult->GetBestRedshift( redshift, matchingNum );
    }

    outputFile.precision(6);
    outputFile  << bfs::path( m_SpectrumPath ).filename().string()  << "\t"
                << redshift << "\t"
                << matchingNum << "\t"
                << "Ray Matching" << std::endl;

    */
    TSolutionSetList selectedResults = GetSolutionsListOverNumber(0);

    if(selectedResults.size()>0){
        std::string strList;
        char tmpChar[256];
        strList.append("#MATCH_NUM\tDETECTED_LINES\tREST_LINES\tZ\n");
        for( UInt32 iSol=0; iSol<selectedResults.size(); iSol++ )
        {
            std::string strMNUM= "";
            TSolutionSet currentSet = selectedResults[iSol];
            sprintf(tmpChar, "%d", currentSet.size());
            strMNUM.append(tmpChar);
            std::string strZ= "";
            sprintf(tmpChar, "%.10f", GetMeanRedshiftSolution(currentSet) );
            strZ.append(tmpChar);

            std::string strDetected = "(";
            std::string strRest = "(";
            for( UInt32 i=0; i<currentSet.size(); i++ )
            {
                sprintf(tmpChar, "%.1f, ", currentSet[i].DetectedRay);
                strDetected.append(tmpChar);
                sprintf(tmpChar, "%.1f, ", currentSet[i].RestRay);
                strRest.append(tmpChar);
            }
            strDetected = strDetected.substr(0, strDetected.size()-2);
            strDetected.append(")");
            strRest = strRest.substr(0, strRest.size()-2);
            strRest.append(")");

            strList.append(strMNUM);
            strList.append("\t");
            strList.append(strDetected);
            strList.append("\t");
            strList.append(strRest);
            strList.append("\t");
            strList.append(strZ);
            strList.append("\n");
        }
        stream << strList.c_str();
    }

    return;
}

Void CRayMatchingResult::SaveLine( const COperatorResultStore& store, std::ostream& stream ) const
{

}

/**
 * @brief CRayMatchingResult::GetBestRedshift
 *
 * select the best redshift given a set of rules:
 * - the solution with the most strong lines
 * - (TODO) if no solution with strong lines, the missing strong lines' absence should be excused by:
 *      * high noise in the theoretical position of the strong lines
 *      * lambda range not able to cover the strong lines
 * @return
 */
Bool CRayMatchingResult::GetBestRedshift(Float64& Redshift, Int32& MatchingNumber) const
{
    Int32 thresMatchingNumber = 2; //minimum matching number for a solution
    TSolutionSetList selectedResults = GetSolutionsListOverNumber(thresMatchingNumber-1);

    if(selectedResults.size()>0){
        int iStrongMax = -1;
        int nStrongMax = -1;
        for( UInt32 iSol=0; iSol<selectedResults.size(); iSol++ )
        {
            int currentNStrongRestLines = getNStrongRestLines(selectedResults[iSol]);
            if(currentNStrongRestLines > nStrongMax){
                iStrongMax = iSol;
                nStrongMax = currentNStrongRestLines;
            }else if(currentNStrongRestLines == nStrongMax && selectedResults[iSol].size()>selectedResults[iStrongMax].size()){
                iStrongMax = iSol;
                nStrongMax = currentNStrongRestLines;
            }
        }

        if(iStrongMax>-1){
            Redshift = GetMeanRedshiftSolution(selectedResults[iStrongMax]);
            MatchingNumber = selectedResults[iStrongMax].size();
        }
    }else{
        return false;
    }

    return true;
}

/**
 * @brief CRayMatchingResult::GetBestMatchNumRedshift
 *
 * get the best redshift in the sense of the highest matching number
 *
 * @return
 */
Bool CRayMatchingResult::GetBestMatchNumRedshift(Float64& Redshift, Int32& MatchingNumber) const
{
    MatchingNumber = GetMaxMatchingNumber();
    TSolutionSetList selectedResults = GetSolutionsListOverNumber(MatchingNumber-1);

    if(selectedResults.size()>0){
        Redshift = GetMeanRedshiftSolution(selectedResults[0]);
    }else{
        return false;
    }

    return true;
}

CRayMatchingResult::TSolutionSetList CRayMatchingResult::GetSolutionsListOverNumber(Int32 number) const
{
    //select results by matching number
    TSolutionSetList selectedResults;
    for( UInt32 iSol=0; iSol<SolutionSetList.size(); iSol++ )
    {
        TSolutionSet currentSet = SolutionSetList[iSol];
        if(currentSet.size() > number){
            selectedResults.push_back(currentSet);
        }

    }
    return selectedResults;
}

TFloat64List CRayMatchingResult::GetAverageRedshiftListOverNumber(Int32 number) const
{
    TFloat64List selectedRedshift;
    CRayMatchingResult::TSolutionSetList selectedResults = GetSolutionsListOverNumber(number);
    for( UInt32 j=0; j<selectedResults.size(); j++ )
    {
        Float64 z = GetMeanRedshiftSolution(selectedResults[j]);
        selectedRedshift.push_back(z);
    }
    return selectedRedshift;
}


TFloat64List CRayMatchingResult::GetRoundedRedshiftCandidatesOverNumber(Int32 number, Float64 step) const
{
    TFloat64List selectedRedshift = GetAverageRedshiftListOverNumber(number);
    TFloat64List roundedRedshift;
    for( UInt32 j=0; j<selectedRedshift.size(); j++ )
    {
        Float64 zround = Float64(int(selectedRedshift[j]/step+0.5f)*step);
        roundedRedshift.push_back(zround);
    }

    return roundedRedshift;
}

Float64 CRayMatchingResult::GetMeanRedshiftSolutionByIndex(Int32 index) const
{
    if(index > SolutionSetList.size()-1)
    {
        return -1.0;
    }

    Float64 redshiftMean = 0.0;
    const TSolutionSet& currentSet = SolutionSetList[index];
    for( UInt32 i=0; i<currentSet.size(); i++ )
    {
        redshiftMean += currentSet[i].Redshift;
    }
    redshiftMean /= currentSet.size();

    return redshiftMean;
}


Float64 CRayMatchingResult::GetMeanRedshiftSolution( const TSolutionSet& s ) const
{
    TSolutionSet currentSet = s;
    Float64 redshiftMean=0.0;
    for( UInt32 i=0; i<currentSet.size(); i++ )
    {
        redshiftMean += currentSet[i].Redshift;
    }
    redshiftMean /= (float)currentSet.size();

    return redshiftMean;
}

Int32 CRayMatchingResult::getNStrongRestLines( const TSolutionSet& s ) const
{
    CRayCatalog::TRayVector strongRestRayList = m_RestCatalog.GetFilteredList(CRay::nType_Emission, CRay::nForce_Strong);
    Int32 ncatalog = strongRestRayList.size();

    TSolutionSet currentSet = s;
    Int32 nStrong=0;
    Float64 tol = 0.11;
    for( UInt32 i=0; i<currentSet.size(); i++ )
    {
        Int32 found  = 0;
        for( UInt32 c=0; c<ncatalog; c++ )
        {
            if( fabs(currentSet[i].RestRay-strongRestRayList[c].GetPosition())<tol ){
                found = 1;
                break;
            }
        }
        if(found==1){
            nStrong++;
        }
    }

    return nStrong;
}

Int32 CRayMatchingResult::GetMaxMatchingNumber() const
{
    if(SolutionSetList.size() < 1)
    {
        return -1.0;
    }

    Int32 maxNumber = 0;
    for( UInt32 i=0; i<SolutionSetList.size() ; i++ )
    {
        TSolutionSet currentSet = SolutionSetList[i];
        if(maxNumber<currentSet.size()){
            maxNumber = currentSet.size();
        }

    }

    return maxNumber;
}