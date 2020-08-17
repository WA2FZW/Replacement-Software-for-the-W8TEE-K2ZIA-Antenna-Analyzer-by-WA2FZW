/*
 * AAStats.cpp
 * 
 * This class, by George-KB1HFT, was written to accompany My_Analyzer_V03.6
 * in order to provide a means to compare the effects of different
 * methods of noise supression on A/D readings, as well as to help
 * in initial debugging of the analog circuits.  
 * 
 * The class's methods can be controlled via settings in the Maintenance > Options menu
 * as follows:
 * 
 * Log Statistics Summary 
 *      "Log Statistics Summary" set to "yes", 
 *          activates calculation of average min, max, and standard
 *          deviations of readings across _all_ of the MAX_POINTS_PER_SAMPLE
 *          groups of A/D readings in a scan, and logging the results to the
 *          serial monitor after the scan is finished.
 *          
 *          NOTE that "Log Statistics Summary" must be set to yes
 *          in order to activate the following two options.
 *   
 * Log SWR Point Stats
 *      "Log Statistics Summary" set to "yes", and
 *          "Log SWR Point Stats" set to "yes",
 *              activates logging of the min, max, and standard deviation of
 *              _each group_ of MAX_POINTS_PER_SAMPLEs.
 *  
 * Log Indiv Readings
 *      "Log Statistics Summary" set to "yes", and
 *             "Log Indiv Readings" set to "yes",
 *                  activates logging of _each individual_ A/D reading in
 *                  each group of readings.   This option can generate
 *                  a _LOT_ of output! 
 *                  
 *                  The .ino file has code that will cancel out of a lengthy
 *                  scan if you hold down the rotary encoder switch.
 */

#include <SPI.h>        // Use the standard IDE library instead
#include "AAStats.h"

AAStats::AAStats(int Max_PP, int Max_PPS)
{
    MaxPlotPoints = Max_PP;
    MaxPointsPerSample = Max_PPS;
    InitAVGReadingsForScan();
    ResetStatsCounters();
};


void AAStats::InitAVGReadingsForScan()
{
    avgFwdReadingMin = 0;
    avgFwdReadingMax = 0;
    avgFwdSD = 0;
      
    avgRevReadingMin = 0;
    avgRevReadingMax = 0;
    avgRevSD = 0;
    maxSWR = 0;
    minSWR = 100;
}

void AAStats::ResetStatsCounters()
{
    //Initialize the counters
    fwdSD = 0;
    fwdMinReading = 1024;
    fwdMaxReading = 0;
    SumFwdSquares = 0;
  
    revSD = 0;
    revMinReading = 1024;
    revMaxReading = 0;
    SumRevSquares = 0;
};

void AAStats::CollectStatsDataPerPoint(uint16_t fwdReading, uint16_t revReading, int i)
{
    //build _READINGS lists
    fwdReadings[i]= fwdReading;
    revReadings[i] = revReading;
};

void AAStats::LogIndivReadings(uint16_t fwdReading, uint16_t revReading)
{
    //Print each reading
    Serial.print(F("fwdReading: "));
    Serial.print(fwdReading);          
    Serial.print(F("    revReading: "));
    Serial.println(revReading);
};

void AAStats::LogSWR_PointStats()
{
    //First the Forward Stats
    Serial.println(F("FWD"));
    Serial.print(F("  fwdMinReading: "));
    Serial.println( fwdMinReading );
    Serial.print(F("  fwdMaxReading: "));
    Serial.println( fwdMaxReading );
    Serial.print(F("  fwdSD: "));
    Serial.println( fwdSD, 2 );
    Serial.println(F(" "));
        
    //Then the Reverse ones
    Serial.println(F("REV"));
    Serial.print(F("  revMinReading: "));
    Serial.println( revMinReading );
    Serial.print(F("  revMaxReading: "));
    Serial.println( revMaxReading);
    Serial.print(F("  revSD: "));
    Serial.println( revSD, 2 );
};

void AAStats::CalculateStats(float fwdMean, float revMean)
{
    //First, round up by .005
//    float localFwdMean = (fwdMean+.005);
//    float localRevMean = (revMean+.005);
    float localFwdMean = (fwdMean);
    float localRevMean = (revMean);
    float intermediateFloat = 0;   

    //Now, from the accumulted fwdReadings[] and revReadings[] lists, calculate
    //the find the mins & max-es and standard deviations.
    for ( int i = 0; i < MaxPointsPerSample; i++ ) 
    {
        //First, Find the FWD mins & max-es
        if(fwdReadings[i] < fwdMinReading)
        {
            fwdMinReading = fwdReadings[i];
        }

        if (fwdReadings[i] > fwdMaxReading)
        {
            fwdMaxReading = fwdReadings[i];
        }

        //Then find the REV mins & max-es
        if (revReadings[i] < revMinReading)
        {
            revMinReading = revReadings[i];
        }

        if (revReadings[i] > revMaxReading)
        {
            revMaxReading = revReadings[i];
        }
   
        //Standard Deviation = 
        //   sqrt( (sumof("each reading" - "mean of all readings")^2)/n-1)
        //
        //First build the Forward data terms
        intermediateFloat = (fwdReadings[i]) - (localFwdMean);     //subtract the localFwdMean from the i_th reading
        SumFwdSquares += (intermediateFloat * intermediateFloat);  //square it and increment the sum of the squares
        
        //Then the Reverse data terms
        intermediateFloat = (revReadings[i] - localRevMean);       //subtract the localRevMean from the i_th reading                                                            //formatted to fit into an array of int16_t's
        SumRevSquares += (intermediateFloat * intermediateFloat);  //square it and increment the sum of the squares
    }
 
    //Finally, find the Standard Deviations of the set of readings
    fwdSD = sqrt(SumFwdSquares/(MaxPointsPerSample -1 ));
    revSD = sqrt(SumRevSquares/(MaxPointsPerSample -1 ));

    //And update the Averages
    avgFwdSD += fwdSD;
    avgFwdReadingMin += fwdMinReading;
    avgFwdReadingMax += fwdMaxReading;
  
    avgRevSD += revSD;
    avgRevReadingMin += revMinReading;
    avgRevReadingMax += revMaxReading;
};
void AAStats::FindMaxMinSWR(float VSWR)
{
    if (VSWR < minSWR) minSWR = VSWR;
    if (VSWR > maxSWR) maxSWR = VSWR;
};
void AAStats::LogScanStatsSummary()
{
    //Print out the STATS
    Serial.println(F("===================================="));
    Serial.println(F("***Statistics Summary***"));
    Serial.println(F(" "));

    Serial.print(F("-----> minSWR: "));
    Serial.print(minSWR, 2);
    Serial.print(F("   maxSWR: "));    
    Serial.print(maxSWR, 2);
    Serial.println(F(" <----- "));
    Serial.println(F(" "));
    
    Serial.println(F("FWD READING STATS"));
    Serial.print(F("  avgFwdReadingMin: "));
    Serial.println(avgFwdReadingMin / (MaxPlotPoints) );
    Serial.print(F("  avgFwdSD: "));
    Serial.println((avgFwdSD / (MaxPlotPoints)), 4 );    
    Serial.print(F("  avgFwdReadingMax: "));
    Serial.println(avgFwdReadingMax / (MaxPlotPoints));
    Serial.println(F(" "));
    Serial.println(F("REV READING STATS"));
    Serial.print(F("  avgRevReadingMin: "));
    Serial.println(avgRevReadingMin / (MaxPlotPoints));
    Serial.print(("  avgRevSD: "));
    Serial.println((avgRevSD / MaxPlotPoints), 4 );    
    Serial.print(F("  avgRevReadingMax: "));
    Serial.println((avgRevReadingMax / (MaxPlotPoints)));
    Serial.println(F("===================================="));
};

