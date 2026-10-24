/* 
 * File:   CronExpr.h
 * Author: alex
 *
 * Created on February 24, 2015, 9:35 AM
 * Modfied on January 20, 2025 by Matthew Elgert
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <math.h>

#define CRON_USE_LOCAL_TIME

#if defined(__cplusplus) && !defined(CRON_COMPILE_AS_CXX)
extern "C" {
#endif

#ifndef ANDROID
#include <time.h>
#else /* ANDROID */
#include <time64.h>
#endif /* ANDROID */

#include <stdint.h> /*added for use if uint*_t data types*/

/**
 * Parsed cron expression
 */
typedef struct {
    uint8_t seconds[8];
    uint8_t minutes[8];
    uint8_t hours[3];
    uint8_t days_of_week[1];
    uint8_t days_of_month[4];
    uint8_t months[2];
} cron_expr;

/**
 * Parses specified cron expression.
 * 
 * @param expression cron expression as nul-terminated string,
 *        should be no longer that 256 bytes
 * @param pointer to cron expression structure, it's client code responsibility
 *        to free/destroy it afterwards
 * @param error output error message, will be set to string literal
 *        error message in case of error. Will be set to NULL on success.
 *        The error message should NOT be freed by client.
 */
void cron_parse_expr(const char* expression, cron_expr* target, const char** error);

/**
 * Uses the specified expression to calculate the next 'fire' date after
 * the specified date. All dates are processed as UTC (GMT) dates 
 * without timezones information. To use local dates (current system timezone) 
 * instead of GMT compile with '-DCRON_USE_LOCAL_TIME'
 * 
 * @param expr parsed cron expression to use in next date calculation
 * @param date start date to start calculation from
 * @return next 'fire' date in case of success, '((time_t) -1)' in case of error.
 */
time_t cron_next(cron_expr* expr, time_t date);

/**
 * Uses the specified expression to calculate the previous 'fire' date after
 * the specified date. All dates are processed as UTC (GMT) dates 
 * without timezones information. To use local dates (current system timezone) 
 * instead of GMT compile with '-DCRON_USE_LOCAL_TIME'
 * 
 * @param expr parsed cron expression to use in previous date calculation
 * @param date start date to start calculation from
 * @return previous 'fire' date in case of success, '((time_t) -1)' in case of error.
 */
time_t cron_prev(cron_expr* expr, time_t date);


#if defined(__cplusplus) && !defined(CRON_COMPILE_AS_CXX)
} /* extern "C"*/
#endif

/*
Cron Expression Format (Extended with Seconds and Year):
--------------------------------------------------------
* * * * * * * /path/to/command
│ │ │ │ │ │ │
│ │ │ │ │ │ └── Year (1970 - 2099) [Optional, defaults to * (every year)]
│ │ │ │ │ │
│ │ │ │ │ └──── Day of the week (0 - 6) (Sunday=0 or SUN-SAT)
│ │ │ │ │
│ │ │ │ └────── Month (1 - 12 or JAN-DEC)
│ │ │ │
│ │ │ └──────── Day of the month (1 - 31)
│ │ │
│ │ └───────── Hour (0 - 23)
│ │
│ └────────── Minute (0 - 59)
│
└─────────── Second (0 - 59) [Optional, defaults to 0]

Examples:
---------
1. 0 0 12 * * * * /path/to/command
   - Every day at 12:00 PM.

2. 30 15 9 1 1 * 2025 /path/to/command
   - At 9:15:30 AM on January 1, 2025.

3. * * 9-17 * * * * /path/to/command
   - Every second between 9 AM and 5 PM daily.

4. 0 30 9 ? JAN MON * /path/to/command
   - At 9:30 AM on Mondays in January, every year.

*/