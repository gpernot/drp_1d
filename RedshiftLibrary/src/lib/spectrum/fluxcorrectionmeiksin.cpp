#include <RedshiftLibrary/log/log.h>
#include <RedshiftLibrary/spectrum/fluxcorrectionmeiksin.h>

#include <algorithm>    // std::sort
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string/predicate.hpp>



namespace bfs = boost::filesystem;
using namespace NSEpic;
using namespace std;
using namespace boost;


CSpectrumFluxCorrectionMeiksin::CSpectrumFluxCorrectionMeiksin()
{
    m_LambdaMin = 200.0;
    m_LambdaMax = 1299.0;
}

CSpectrumFluxCorrectionMeiksin::~CSpectrumFluxCorrectionMeiksin()
{

}

Bool CSpectrumFluxCorrectionMeiksin::Init( std::string calibrationPath )
{
    bfs::path calibrationFolder( calibrationPath.c_str() );
    std::vector<std::string> fileNamesList;
    fileNamesList.push_back("Meiksin_Var_curves_2.0.txt");
    fileNamesList.push_back("Meiksin_Var_curves_2.5.txt");
    fileNamesList.push_back("Meiksin_Var_curves_3.0.txt");
    fileNamesList.push_back("Meiksin_Var_curves_3.5.txt");
    fileNamesList.push_back("Meiksin_Var_curves_4.0.txt");
    fileNamesList.push_back("Meiksin_Var_curves_4.5.txt");
    fileNamesList.push_back("Meiksin_Var_curves_5.0.txt");
    fileNamesList.push_back("Meiksin_Var_curves_5.5.txt");
    fileNamesList.push_back("Meiksin_Var_curves_6.0.txt");
    fileNamesList.push_back("Meiksin_Var_curves_6.5.txt");
    fileNamesList.push_back("Meiksin_Var_curves_7.0.txt");

    for(Int32 k=0; k<fileNamesList.size(); k++)
    {
        bfs::path fPath = ( calibrationFolder/"igm"/"IGM_variation_curves_meiksin"/fileNamesList[k].c_str() ).string();
        std::string fPathStr = (fPath).string();

        bool ret = LoadFile(fPathStr.c_str());
        if(!ret)
        {
            Log.LogError("Unable to load the Meiksin flux correction data. aborting...");
            return false;
        }
    }
    return true;
}



Bool CSpectrumFluxCorrectionMeiksin::LoadFile( const char* filePath )
{
    bool loadSuccess=true;
    std::ifstream file;
    file.open( filePath, std::ifstream::in );
    bool fileOpenFailed = file.rdstate() & std::ios_base::failbit;
    if(fileOpenFailed)
    {
        loadSuccess = false;
    }else
    {
        MeiksinCorrection _correction;
        std::vector<Float64> fluxcorr0;
        std::vector<Float64> fluxcorr1;
        std::vector<Float64> fluxcorr2;
        std::vector<Float64> fluxcorr3;
        std::vector<Float64> fluxcorr4;
        std::vector<Float64> fluxcorr5;
        std::vector<Float64> fluxcorr6;

        std::string line;
        // Read file line by line
        while( getline( file, line ) )
        {
            if( !boost::starts_with( line, "#" ) )
            {
                std::istringstream iss( line );
                Float64 x, y0, y1, y2, y3, y4, y5, y6;
                iss >> x >> y0 >> y1 >> y2 >> y3 >> y4 >> y5 >> y6;
                _correction.lbda.push_back(x);
                fluxcorr0.push_back(y0);
                fluxcorr1.push_back(y1);
                fluxcorr2.push_back(y2);
                fluxcorr3.push_back(y3);
                fluxcorr4.push_back(y4);
                fluxcorr5.push_back(y5);
                fluxcorr6.push_back(y6);
            }
        }
        file.close();
        _correction.fluxcorr.push_back(fluxcorr0);
        _correction.fluxcorr.push_back(fluxcorr1);
        _correction.fluxcorr.push_back(fluxcorr2);
        _correction.fluxcorr.push_back(fluxcorr3);
        _correction.fluxcorr.push_back(fluxcorr4);
        _correction.fluxcorr.push_back(fluxcorr5);
        _correction.fluxcorr.push_back(fluxcorr6);

        m_corrections.push_back(_correction);
    }

    return loadSuccess;
}

Int32 CSpectrumFluxCorrectionMeiksin::GetIdxCount()
{
    return 7; //harcoded value from the number of cols in the ascii files
}

/**
 * @brief CSpectrumFluxCorrectionMeiksin::GetRedshiftIndex
 * @param z
 *
 * Returns the index corresponding to the table to be used for that redshift value
 *
 * @return
 */
Int32 CSpectrumFluxCorrectionMeiksin::GetRedshiftIndex(Float64 z)
{
    Int32 index = -1;

    Float64 zStart = 2.0;
    Float64 zStep = 0.5;
    Float64 zStop = 7.0;
    if(z<zStart){
        index = 0;
    }else if( z>=zStart && z<zStop)
    {
        index = (Int32)( (z-zStart)/zStep + 1.0);
    }else if( z>=zStop )
    {
        index = (Int32)((zStop-zStart)/zStep);
    }
    return index;
}

Float64 CSpectrumFluxCorrectionMeiksin::GetLambdaMin()
{
    return m_LambdaMin;
}

Float64 CSpectrumFluxCorrectionMeiksin::GetLambdaMax()
{
    return m_LambdaMax;
}

