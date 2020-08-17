#ifndef AA_STATS_H
#define AA_STATS_H

class AAStats
{
    public:
        AAStats(int MaxPlotPoints, int MAX_POINTS_PER_SAMPLE);
        void InitAVGReadingsForScan();
        void ResetStatsCounters();
        void LogScanStatsSummary();
        void CalculateStats(float FWD, float REV);
        void CollectStatsDataPerPoint(uint16_t FW, uint16_t RV, int i);  
        void LogSWR_PointStats();
        void LogIndivReadings(uint16_t FW, uint16_t RV);
        void FindMaxMinSWR(float VSWR);
 
    private:
        //vars used in Standard Deviation and min/max calculations
        int MaxPointsPerSample;
        int MaxPlotPoints;
        
        float maxSWR;
        float minSWR;

        //Variables for FWD calculations
        uint16_t fwdReadings[75];     
        uint16_t fwdMinReading;
        uint16_t fwdMaxReading;  
        float SumFwdSquares;
        float avgFwdReadingMin;
        float avgFwdSD;
        float avgFwdReadingMax;
        float fwdSD;
        
        //Variables for REV calculations
        uint16_t revReadings[75];  
        uint16_t revMinReading;
        uint16_t revMaxReading;
        float SumRevSquares;
        float avgRevReadingMin;
        float avgRevSD;
        float avgRevReadingMax;
        float revSD;
};

#endif
