#pragma once

/*
*  Eddy Ojode 3/22/21
*
 -- Purpose of this class is baby sitter Kata coding excercise
 -- chose to use C++ and do a console application
 -- Input: Start time, leave time and family
 -- Additional rules govern the times and the pay rate is specific to families

*/
class BabySitterPay
{
public:
    // -- // In minutes, 24 * 60
    int StartTime;
    int StopTime;
    int EnteredTime;
    int Family;
    bool TimesValid;

    BabySitterPay() { StartTime = 0, StopTime = 0; EnteredTime = 0; Family = 0; TimesValid = 0; };
    ~BabySitterPay() {};
    int ValidateString(std::string input);
    int ValidateTimes();

    int GetPay();
    int GetPay(int rate[3], int time[3]);

    int TimeInMinutes(int hour, int minutes = 0);
};