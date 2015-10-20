#ifndef __TIME_HELPER_H__
#define __TIME_HELPER_H__

/**
 * This file has the definitions for some 
 * interesting macros used to handle times
 * e.g. printing in the right format,
 * obtaining current times and so on.
 **/

#include <time.h>

/**
 * Prints a time_t type, timet, as H:M:S.
 * This is achieved by restricting the string obtained by ctime()
 **/
#define PRINT_TIME_T_AS_HMS(timet) printf("%.8s", &(ctime(&(timet)))[11])

/**
 * Gets the current time as a time_t type.
 */
#define GET_CURRENT_TIME() time(NULL)

/**
 * Gets the difference between time1-time0, returns double.
 */
#define GET_DURATION_TIME(time1,time0) difftime(time1,time0)



#endif /* __TIME_HELPER_H__ */ 
