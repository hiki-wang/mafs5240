//
// Created by HUAWEI on 2024/10/5.
//
#include <set>
#include <fstream>
#include <iostream>
#include <cassert>
#include <math.h>
#include <cmath>

using namespace std;
typedef int Date;
enum DayOfWeek {
    SUN, MON, TUE, WED, THU, FRI, SAT
};
set<int> holidays;

// Function to determine if a given year is a leap year
static bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// Function to validate a date represented as an integer in the format yyyymmdd
static bool isValidDate(Date dt) {
    if (dt > 99999999 || dt < 10000000) return false;
    int year = dt / 10000;
    int month = (dt / 100) % 100;
    int day = dt % 100;

    if (month < 1 || month > 12 || day < 1) return false;
    int maxDaysInMonth = 31;
    if (month == 2) {
        maxDaysInMonth = isLeapYear(year) ? 29 : 28;
    } else if (month == 4 || month == 6 || month == 9 || month == 11) {
        maxDaysInMonth = 30;
    }
    return day <= maxDaysInMonth;
}

// Function to determine the number of days in a given month of a specific year
static int daysInMonth(Date dt) {
    int year = dt / 10000;
    int month = (dt / 100) % 100;
    if (month == 2) {
        return isLeapYear(year) ? 29 : 28;
    }
    return (month == 4 || month == 6 || month == 9 || month == 11) ? 30 : 31;
}

// Function to calculate the total number of days from the initial date (00010101) to a given date.
static int daysFromReference(Date dt) {
    int year = dt / 10000;
    int month = (dt / 100) % 100;
    int day = dt % 100;

    int totalDays = 0;
    // Add days for the complete years
    for (int y = 1; y < year; ++y) {
        totalDays += isLeapYear(y) ? 366 : 365;
    }
    // Add days for the complete months in the current year
    for (int m = 1; m < month; ++m) {
        totalDays += daysInMonth(year * 10000 + m * 100 + 1);
    }
    // Add the days in the current month
    totalDays += day;
    return totalDays;
}

Date add1Day(Date dt) {
    int year = dt / 10000;
    int month = (dt / 100) % 100;
    int day = dt % 100;

    int days_in_month = daysInMonth(dt);
    if (day < days_in_month) return dt + 1;
    if (month < 12) return year * 10000 + (month + 1) * 100 + 1;
    return (year + 1) * 10000 + 101;
}

Date minus1Day(Date dt) {
    int year = dt / 10000;
    int month = (dt / 100) % 100;
    int day = dt % 100;

    int days_in_last_month = daysInMonth(dt - 100);
    if (day > 1) return dt - 1;
    if (month > 1) return year * 10000 + (month - 1) * 100 + days_in_last_month;
    return (year - 1) * 10000 + 1231;
}

int dateDiff(Date d1, Date d2) {
    int days1 = daysFromReference(d1);
    int days2 = daysFromReference(d2);
    return abs(days2 - days1);
}


bool isWeekEnd(Date dt) {
    Date benchmark = 20000101; //SAT
    int diff = dateDiff(benchmark, dt);
    if (diff % 7 == 0 || diff % 7 == 1) return true;
    else return false;
}

bool isHoliday(Date dt) {
    if (holidays.find(dt) != holidays.end()) return true;
    else return false;
}

bool isBusinessDay(Date dt) {
    if (isValidDate(dt) && !isWeekEnd(dt) && !isHoliday(dt)) return true;
    else return false;
}


Date monthend(Date dt) {
    int year = dt / 10000;
    int month = (dt / 100) % 100;

    Date monthend = year * 10000 + month * 100 + daysInMonth(dt);
    while (isWeekEnd(monthend) || isHoliday(monthend)) monthend--;

    return monthend;
}

Date nextMonthend(Date dt) {
    int year = dt / 10000;
    int month = (dt / 100) % 100;
    if (month < 12) return monthend(year * 10000 + month * 100 + 101);
    else return monthend((year + 1) * 10000 + 101);
}

bool loadHoliday(const char *filename) {
    ifstream holFile(filename);
    if (!holFile) return false;

    Date dt;
    while (holFile >> dt) {
        holidays.insert(dt);
    }

    holFile.close();
    return true;
}

bool removeHoliday() {
    holidays.clear();
    return true;
}


double finalbalance(
        Date dt,            // Date (1/9/2024 - 31/12/2024) to buy product. 1st payment starts end-of-month
        int investPeriod,    // Investment period
        int payout,            // monthly payout
        int payoutPeriod,    // Payout period (years 1-15)
        double interest,        // Interest rate (%) on accrual amount
        double payment      // monthly payment in Investment period
) {
    int invest_months = investPeriod * 12;
    double balance = payment;
    Date month_end = monthend(dt);
    for (int i = 1; i < invest_months; i++) {
        double days_diff = dateDiff(month_end, nextMonthend(month_end));
        balance = floor(100 * balance * (1 + interest / 100 * days_diff / 365)) / 100 + payment;
        month_end = nextMonthend(month_end);
    }
    int payout_months = payoutPeriod * 12;
    for (int i = 1; i <= payout_months; i++) {
        double days_diff = dateDiff(month_end, nextMonthend(month_end));
        balance = floor(100 * balance * (1 + interest / 100 * days_diff / 365)) / 100 - payout;
        month_end = nextMonthend(month_end);
    }
    return balance;
}

double InvestAnnuity(
        Date dt,            // Date (1/9/2024 - 31/12/2024) to buy product. 1st payment starts end-of-month
        int investPeriod,    // Investment period
        int payout,            // monthly payout
        int payoutPeriod,    // Payout period (years 1-15)
        double interest        // Interest rate (%) on accrual amount
) {

    assert(20240901 <= dt && dt <= 20241231);
    assert(1 <= investPeriod && investPeriod <= 10);
    assert(1 <= payoutPeriod && payoutPeriod <= 15);

    auto func = [dt,investPeriod,payout,payoutPeriod,interest](double payment){
        return finalbalance(dt,investPeriod,payout,payoutPeriod,interest,payment);
    };
    double a = 0;
    double b = 100000;
    double tolerance = 0.1;
    if (func(a) * func(b) >= 0){
        std::cerr << "The function must have different signs at a and b." << std::endl;
        return NAN;
    }

    double c;
    while((b-a)>=tolerance){
        c = (a+b)/2;
        if(func(c) == 0.0) break;
        else if (func(c)*func(a)<0) b=c;
        else a=c;
    }
    return c;
}

int main(){
    loadHoliday("../Holiday.txt");
    double result = InvestAnnuity(20240924,10,10000,15,2);
    cout<<fixed<<result;
}