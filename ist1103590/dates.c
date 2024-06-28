/**
 * @file dates.c
 * 
 * @author Tiago Firmino - ist1103590 
 * 
 * File containing date functions used in the program.
 * 
*/

#include "project.h"
#include "prototypes.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*
 * Turns a date in a timestamp format into a value of minutes.
 * Returns an integer corresponding to the minute sum.
 */
int get_time_in_mins(timestamp_t ts) {
	int yr = ts.y, mth = ts.mth, days = ts.d, hrs = ts.h, mins = ts.min;
	int min_sum = yr * MINS_IN_YEAR + get_month_mins(mth - 1, ts.y);

	// Using days - 1 to not include current full day
	min_sum += (days - 1) * MINS_IN_DAY + hrs * 60 + mins;
	return min_sum;
}

/*
 * Returns the sum of the minutes of the months since the start
 * of the year until the month corresponding 
 * to the integer received.
 */
int get_month_mins(int month, int year) {
	/* Array containing the amout of days per
	 * month from january to december */
	const int days_in_month[] = {
		31, 28 + is_leap_year(year), 31, 30,
		31, 30, 31, 31,
		30, 31, 30, 31
	};
	int i, min_sum = 0;

	for (i = 0; i < month; i++) {
		min_sum += days_in_month[i] * MINS_IN_DAY;
	}
	return min_sum;
}

/**
 * Compares dates and time in minutes
 * returning TRUE if d1 is after d2, 
 * FALSE if they are equal and INVALID otherwise.
*/
int compare_date_time(timestamp_t d1, timestamp_t d2) {
	int date1, date2;
	date1 = get_time_in_mins(d1);
	date2 = get_time_in_mins(d2);

	if (date1 > date2) return TRUE;
	else if (date1 == date2) return FALSE;
	return INVALID;
}

/**
 * Compares dates excluding their hour and minute parameters.
 * Returns TRUE if d1 is after d2,
 * FALSE if they are equal and INVALID otherwise.
*/
int compare_date(timestamp_t d1, timestamp_t d2) {
    if (d1.y > d2.y) return TRUE;
    else if (d1.y < d2.y) return INVALID;
    else if (d1.mth > d2.mth) return TRUE;
    else if (d1.mth < d2.mth) return INVALID;
    else if (d1.d > d2.d) return TRUE;
    else if (d1.d < d2.d) return INVALID;
    return FALSE;
}


/*
 * Checks if the date received is invalid,
 * has a check if the given date is for facturation
 * and compares the given date with the last registered dates.
 * Returns TRUE if the date is invalid, FALSE otherwise.
 */
int invalid_date(timestamp_t ts, system_t* sys, int is_facturation) {
	int days_in_month[] = {31, 28 + is_leap_year(ts.y), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (ts.y < 2024) return TRUE;
    if (ts.mth < 1 || ts.mth > 12) return TRUE;
    if (ts.d < 1 || ts.d > days_in_month[ts.mth - 1]) return TRUE;
    if (ts.h < 0 || ts.h > 23) return TRUE;
    if (ts.min < 0 || ts.min > 59) return TRUE;

    if (!is_facturation) {
		timestamp_t latest_date = sys->date_registry;
		if (compare_date_time(ts, latest_date) < 0) return TRUE;
	}

    return FALSE;
}

/*
 * Function to determine if a year is a leap year.
 * Returns 1 if the year is a leap year, 0 otherwise.
 */
int is_leap_year(int year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

/**
 * Checks if there is a need to adjust the facturation 
 * for February 29th.
*/
int check_feb29(timestamp_t entry, timestamp_t exit) {

	int entry_min = get_time_in_mins(entry);
	int exit_min = get_time_in_mins(exit);

	if ((entry.y % 4 == 0 && 
        (entry.y % 100 != 0 || entry.y % 400 == 0)) &&
        (entry.mth < 3 || (entry.mth == 3 && entry.d == 1))) {

        timestamp_t feb29 = {entry.y, 29, 2, 0, 0};
        int feb29_min = get_time_in_mins(feb29);
        if (exit_min > feb29_min && entry_min < feb29_min + 1440) {
            return TRUE;
        }
    }
	return FALSE;
}