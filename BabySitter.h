#pragma once

/*
*  Eddy Ojode 3/22/21
*
 -- Purpose of this class is baby sitter Kata coding excercise
 -- chose to use C++ and do a console application
 -- Input: Start time, leave time and family
 -- Additional rules govern the times and the pay rate is specific to families

*/
#define MAX_PAY_ZONES 3
#define NUM_FAMILY 3
#define FAM_A 0
#define FAM_B 1
#define FAM_C 2


class BabySitterPay
{
public:
    struct PayStruct
    {
        int PayRate[MAX_PAY_ZONES];
        int PayZone[MAX_PAY_ZONES];
    };
   
    int CurrentStartTime;
    int CurrentStopTime;
    int EnteredTime;
    int CurrentFamily;
    bool CurrentTimesValid;

    PayStruct CurrentPay[NUM_FAMILY];

    BabySitterPay() ;
    ~BabySitterPay() {};
    int ValidateString(std::string input);
    int ValidateTimes();

    int GetPay();
    int GetPay(int rate[3], int time[3]);
    int GetPay(PayStruct currentSitter);

    int TimeInMinutes(int hour, int minutes = 0);
};
