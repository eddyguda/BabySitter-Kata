// BabySitter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdlib.h>
#include <string>
#include "BabySitter.h"





/*
Function:       TimeInMinutes(int hour, int minutes)
Description:    Utility to convert time into minutes
Params:         int hour, int minutes, time in hours and minitues
*/
int BabySitterPay::TimeInMinutes(int hour, int minutes)
{
    return ((hour * 60) + minutes);
}




/*
Function:       ValidateString(std::string input)
Description:    Utility to validate input string for proper format and numbers
Params:         string input
Return:         Code to decipher error
*/
int BabySitterPay::ValidateString(std::string input)
{
    //-- errorCode
    int errorCode = 1;
    // -- Check Size - code 1 fail
    if (input.size() < 8)
        return errorCode;
    ++errorCode;
    // -- Bad formate missing colon error 2
    if (input[2] != ':')
        return errorCode;
    ++errorCode;
    // -- No space before AM, PM error 3
    if (input[5] != ' ')
        return errorCode;
    ++errorCode;
    // -- Hour and minute, and AM/ PM
    char hourCh[3], minuteCh[3], ampmCh[3];
    int hourInt = 0, minuteInt = 0;
    //Two digit hour
    try
    {
        std::size_t length = input.copy(hourCh, 2, 0);
        hourCh[length] = '\0';
        // Convert to int
        hourInt = std::stoi(hourCh, nullptr, 10);
        // -- Only 12 hours are valid
        ++errorCode;
        if (hourInt > 12)
            return errorCode;
    }
    catch (...)
    {
        // -- Badly formed hour errorcode 4
        return errorCode;
    }
    ++errorCode;
    // -- Two digit minute
    try
    {            
        std::size_t length = input.copy(minuteCh, 2, 3);
        minuteCh[length] = '\0';
        minuteInt = std::stoi(minuteCh, nullptr, 10);
        ++errorCode;
        // -- Minute greater than 60 - errorcode 7
        if (minuteInt > 60)
            return errorCode;
    }
    catch (...)
    {
        // -- Badly formed mintue - error code 6
        return errorCode;
    }
    ++errorCode;
    // -- 12 hour marker
    try
    {
        std::size_t length = input.copy(ampmCh, 2, 6);
        ampmCh[length] = '\0';        
        
        ++errorCode;
        // -- Is it AM or PM
        if (ampmCh[1] != 'm' && ampmCh[1] != 'M')
        {
            // -- no m in AM PM - error code 9
            return errorCode;
        }
        EnteredTime = TimeInMinutes(hourInt, minuteInt);
        // -- AM no adjustment
        if (ampmCh[0] == 'a' || ampmCh[0] == 'A')
        {
            TimesValid = true;
            return 0;
        }
        // -- If PM add 12 hours to make 24 hour clock
        else if (ampmCh[0] == 'p' || ampmCh[0] == 'P')
        {
            EnteredTime += TimeInMinutes(12); 
            TimesValid = true;
            return 0;
        }
        ++errorCode;
        // -- No A or P for am pm, error code 10
        return errorCode;

    }
    catch (...)
    {
        // -- Badly formed AM PM -- error code 8
        return errorCode;      
    }
    return errorCode;
}




/*
Function:       ValidateTimes
Description:    Check for errors in start and stop time
                For instance start later than stop or outside work hours
                        
Params:         none
Return:         Error code
*/
int BabySitterPay::ValidateTimes()
{
    // -- Rule for start time: no earlier than 5PM and no later than 3AM for full hour of pay
    int earliestStart = TimeInMinutes(17); // Can't start before 5PM
    int latestStart = TimeInMinutes(3); // Can't start after 3AM for full hour
    int earliestEnd = TimeInMinutes(18); // Can't end before 6PM for full hour
    int latestEnd = TimeInMinutes(4); // Can't work beyond 4AM
    int midnight = TimeInMinutes(24); //time switches at midnight

    int errorCode = 0;
    // -- Can't work with negative times
    if (StartTime < 0 || StopTime < 0)
    {
        errorCode |= 1 << 0;
    }
    // -- Invalid times
    else if (StartTime > midnight || StopTime > midnight)
    {
        errorCode |= 1 << 1;
    }
    // -- Block 1 between 3:01AM and 4:59PM, can't start
    if (StartTime > latestStart && StartTime < earliestStart)
    {
        errorCode |= 1 << 2;
    }
    // -- Block 2 between 4:01AM and 5:59 PM can't stop
    else if (StopTime > latestEnd && StopTime < earliestEnd)
    {
        errorCode |= 1 << 3;
    }
    // -- Final check , if stop less than start
    if (StopTime > StartTime && ((StopTime - StartTime) < 60))
    {
        errorCode |= 1 << 4;
    }
    // -- No time at all
    else if (StartTime == StopTime)
    {
        errorCode |= 1 << 5;
    }
    // -- Accross the time zone
    else if (StartTime > StopTime)
    {
        // -- Add 24 hours to correct
        int tempStop = StopTime + midnight;
        int hoursWorked = tempStop - StartTime;
        // -- Can only work a maximum of 11 hours
        if (hoursWorked > TimeInMinutes(11))
            errorCode |= 1 << 6;
        // -- Can only work full hours
        else if (hoursWorked < 60)
            errorCode |= 1 << 7;
    }
    if(errorCode == 0)
        TimesValid = true;
    return errorCode;
}




/*
Function:       GetPay()
Description:    Will launch calculate pay utility with appropriate pay rates and times
                        
Params:         None
Return:         Required pay
*/
int BabySitterPay::GetPay()
{
    int fam[3][3] = { { TimeInMinutes(23), TimeInMinutes(4), TimeInMinutes(4) },
        { TimeInMinutes(22), TimeInMinutes(0), TimeInMinutes(4) },
        { TimeInMinutes(21), TimeInMinutes(4), TimeInMinutes(4) } };

    int rate[3][3] = { {15, 20, 0},
        {12, 8, 16},
        {21, 15, 0} };

    return GetPay(rate[Family], fam[Family]);
}




/*
Function:       GetPay(int rate[3], int time[3])
Description:    Calculate pay based on hours worked and pay rate across different time zones
                    In the case where worked hour straddles two pay zones, not pay will be calculated
                    E.g. If hour is between 6:30 and 7:30, and pay zone changes at 7, no pay since those are partial hours
                    Requirement was ambigous so this may need to be adjusted if requirements are updated

Params:         int rate[3] - pay rate, there are a max of 3
                int hours[3] - from start to end there are 3 zones with differing pay
Return:         Pay
*/
int BabySitterPay::GetPay(int rate[3], int time[3])
{
    // -- Two key times are noon and midnight
    // -- Times before midnight indicated times between midnight and 4AM 
    // -- and these need to be adusted since there are 24 hours off in terms of calculation
    const int noon = TimeInMinutes(12);
    const int adjust24 = TimeInMinutes(24);
    //-- This variable will be used to adjust time for calculation
    int newTime[3] = { 0, 0, 0 };
    int newStart = StartTime;
    int newStop = StopTime;
    // -- Will add pay across the different pay zones
    int pay[3] = { 0, 0, 0 };
    // - Add 24 to times past midnight to deal with rollover
    if (newStart < noon)
        newStart += adjust24;
    if (newStop < noon)
        newStop += adjust24;

    // -- Now loop through the pay zones and adjust for rollover
    for (int i = 0; i < 3; i++)
    {
        newTime[i] = time[i];
        if (newTime[i] < noon)
            newTime[i] += adjust24;
    }
    // -- No search for hours across the pay zones and calculate pay based on rate
    for(int i = 0; i < 3; i++)
    { 
        // -- First, if the end of work is after the pay zone end, calculate pay and continue
        if (newStop > newTime[i])
        {
            // -- Find the hours worked
            int hours = 0;
            // -- Prior code ensure start is not greater than stop but prevent negative pay
            // -- Also will not deal with fractional hours. Requirement is vague about this,
            // -- -- It doesn't say whether a full hour accross pay zone should be honored with diffent pay rates
            if(newTime[i] > newStart)
                hours = (newTime[i] - newStart) / 60;
            pay[i] = (hours * rate[i]);
            // -- Now we adust the start time to the new pay zone
            newStart = newTime[i];
        }
        // -- Next, if the end of work is before the pay zone end, calculate pay and we are done
        else
        {
            int hours = 0;
            if(newStop > newStart)
                hours = (newStop - newStart) / 60;
            pay[i] = (hours * rate[i]);
            newStart = newTime[i];
            newStop = newTime[i];
        }
    }
    // -- Return sum accross pay periods
    return (pay[0] + pay[1] + pay[2]);
}




/*
        Function: FindInputError
        Description:
        Params:   input string with time in required formate
*/
void FindInputError(BabySitterPay *newSitter, std::string input)
{
    int testValidateString = newSitter->ValidateString(input);

    switch (testValidateString)
    {
    default:
        std::cout << "\r\nUnknown error\r\n";
        break;
    case 0:
        break;
    case 1:
        std::cout << "\r\nError! Input length too short\r\n";
        break;
    case 2:
        std::cout << "\r\nError! Missing semi colon\r\n";
        break;
    case 3:
        std::cout << "\r\nError! Space needed before AM or PM\r\n";
        break;
    case 4:
        std::cout << "\r\nError! Hour field error\r\n";
        break;
    case 5:
        std::cout << "\r\nError! Using 12 Hour clock, hour must be 0 - 12\r\n";
        break;
    case 6:
        std::cout << "\r\nError! Minute field error\r\n";
        break;
    case 7:
        std::cout << "\r\nError! Minutes must be between 0 and 60\r\n";
        break;
    case 8:
        std::cout << "\r\nError! AM/ PM error\r\n";
        break;
    case 9:
        std::cout << "\r\nError! No M or m in AM/ PM\r\n";
        break;
    case 10:
        std::cout << "\r\nError No A/a or p/P in AM/ PM\r\n";
        break;
    }

}



/*
Function:       FindTimeError
Description:    Find out why there was an error in time
Params:         BabySitterPay pointer
*/
void FindTimeError(BabySitterPay *newSitter)
{
    int returnCode = newSitter->ValidateTimes();
    if (returnCode == 0)
    {
        std::cout << "\r\nNo Error!\r\n";
    }
    else
    {
        if (returnCode & (1 << 0))
        {
            std::cout << "\r\nERROR! Negative time detected\r\n";
        }
        if (returnCode & (1 << 1))
        {
            std::cout << "\r\nERROR! More than 24 hours detected\r\n";
        }
        if (returnCode & (1 << 2))
        {
            std::cout << "\r\nERROR! Starting after 3AM and before 5PM is invalid\r\n";
        }
        if (returnCode & (1 << 3))
        {
            std::cout << "\r\nERROR! Error can't stop after 4AM or before 6PM\r\n";
        }
        if (returnCode & (1 << 4))
        {
            std::cout << "\r\nERROR! Can't work less than 1 hour\r\n";
        }
        if (returnCode & (1 << 5))
        {
            std::cout << "\r\nERROR! Start and stop can't be equal\r\n";
        }
        if (returnCode & (1 << 6))
        {
            std::cout << "\r\nERROR! Start time can't be after stop time\r\n";
        }
        if (returnCode & (1 << 7))
        {
            std::cout << "\r\nERROR! Can't work less than 1 hour across midnight\r\n";
        }
    }

}



/*
Function:       PrintNewLine
Description: 
Params:   
*/
void PrintNewLine()
{
    std::cout << "\r\n";
}



/*
Function:       TestForExit
Description:    Find out if user typed exit to leave application
Params:         string input
*/
bool TestForExit(std::string input)
{
    if (input.compare("exit") == 0 || input.compare("Exit") == 0 || input.compare("EXIT") == 0)
        return true;
    return false;

}





/*
Function:       UserPrompt
Description:    Prompts user to enter baby sitter pay information
Params:         None
*/
void UserPrompt()
{
    BabySitterPay newSitter;
    std::string starttime = "0";
    int validInput = false;
    bool exitApp = false;

    PrintNewLine();
    std::cout << "Type 'Exit' to leave applicaton\r\n\r\n";
    // -- Using enter as escape sequence
    while(exitApp == false)
    { 
        TestForExit(starttime);
        validInput = false;
        while(!validInput && !exitApp)
        { 
            // -- First get start time
            newSitter.TimesValid = false;
            std::cout << "Enter start time in 12 hour format XX:XX PM";
            PrintNewLine();
            // -- Process input
            std::getline(std::cin, starttime);
            exitApp = TestForExit(starttime);
            if (exitApp)
                break;
            newSitter.ValidateString(starttime);
            validInput = newSitter.TimesValid;
            if (validInput == false)
            {
                FindInputError(&newSitter, starttime);
            }
            else
            {
                newSitter.StartTime = newSitter.EnteredTime;
            }
            
            PrintNewLine();
        }
        validInput = false;
        // -- Next Get Stop time
        while (!validInput && !exitApp)
        {
            newSitter.TimesValid = false;
            std::cout << "Enter Stop time in 12 hour format XX:XX PM";
            PrintNewLine();
            std::getline(std::cin, starttime);
            exitApp = TestForExit(starttime);
            if (exitApp)
                break;            
            newSitter.ValidateString(starttime);
            validInput = newSitter.TimesValid;
            if (validInput == false)
            {
                FindInputError(&newSitter, starttime);
            }
            else
            {
                newSitter.StopTime = newSitter.EnteredTime;
            }
            PrintNewLine();
        }
        validInput = false;
        // -- Now Get Family
        while (!validInput && !exitApp)
        {
            newSitter.TimesValid = false;
            newSitter.ValidateTimes();
            validInput = newSitter.TimesValid;
            if (validInput == false)
            { 
                FindTimeError(&newSitter);
                break;
            }
            else
            {
                std::cout << "Enter famlily letter A, B or C";
                PrintNewLine();
                std::getline(std::cin, starttime);
                exitApp = TestForExit(starttime);
                if (exitApp)
                    break;
                if (starttime.compare("A") == 0 || starttime.compare("a") == 0)
                {
                    newSitter.TimesValid = true;
                    newSitter.Family = 0;
                }
                else if (starttime.compare("B") == 0 || starttime.compare("b") == 0)
                {
                    newSitter.TimesValid = true;
                    newSitter.Family = 1;
                }
                else if (starttime.compare("C") == 0 || starttime.compare("c") == 0)
                {
                    newSitter.TimesValid = true;
                    newSitter.Family = 2;
                }
                else
                    std::cout << "Enter A, B or C for family" << "\r\n";
            }

            validInput = newSitter.TimesValid;

            if (newSitter.TimesValid == true)
            {
                std::cout << "Total pay for this session: $" << newSitter.GetPay() << "\r\n\r\n";

            }
        }
    }
}


int main()
{
    std::cout << "Hello World!\n";
    UserPrompt();

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
