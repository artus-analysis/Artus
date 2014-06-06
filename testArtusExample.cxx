
void checkHistMean ( TFile * histFile, TString histName, 
                        double expMeanMin, double expMeanMax ) { 

    hist = (TH1D *) histFile->Get( histName );
    if ( hist == NULL ) {
        std::cout << "histogram " << histName << " not found" << std::endl;
        gApplication->Terminate(1);
    }

    double mean = hist->GetMean();

    if ( ( mean < expMeanMin ) || ( mean > expMeanMax ) ) {
        std::cout << "histogram " << histName << " deviates from expected range" << std::endl;
        std::cout << "[ " << expMeanMin << "," << expMeanMax << " ]"
                  << " but found " << mean << std::endl;

        gApplication->Terminate(1);
    }

}

int testArtusExample() {

    TFile * histFile = new TFile( "Example/data/sample_output.root", "READ" );
    
    checkHistMean( histFile, "lowPt/pt", 0.950568, 0.95057 );
    checkHistMean( histFile, "lowPt/pt_corr", 1.09790, 1.09792 );
    checkHistMean( histFile, "lowPt/theta", 0.83138, 0.83140 );

    checkHistMean( histFile, "highPt/pt", 1.05040, 1.05042 );
    checkHistMean( histFile, "highPt/pt_corr", 1.10292, 1.10294 );
    checkHistMean( histFile, "highPt/theta", 0.834291, 0.834293 );

    // pt_over_theta

    gApplication->Terminate(0);
    return 0;
}
