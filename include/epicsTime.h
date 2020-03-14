/*************************************************************************\
* Copyright (c) 2009 UChicago Argonne LLC, as Operator of Argonne
*     National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
*     Operator of Los Alamos National Laboratory.
* EPICS BASE is distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution.
\*************************************************************************/
/** \file epicsTime.h
  * \brief EPICS time-stamps (epicsTimeStamp), epicsTime C++ class
  * and C functions for handling wall-clock times.
  * \author Jeffrey O. Hill
  */

#ifndef epicsTimehInclude
#define epicsTimehInclude

#include <time.h>

#include "shareLib.h"
#include "epicsTypes.h"
#include "osdTime.h"
#include "errMdef.h"

/** \brief The EPICS Epoch is 00:00:00 Jan 1, 1990 UTC */
#define POSIX_TIME_AT_EPICS_EPOCH 631152000u

/** \brief EPICS time stamp, for use from C code.
 *
 * Because it uses an unsigned 32-bit integer to hold the seconds count, an
 * epicsTimeStamp can safely represent time stamps until the year 2106.
 */
typedef struct epicsTimeStamp {
    epicsUInt32    secPastEpoch;   /**< \brief seconds since 0000 Jan 1, 1990 */
    epicsUInt32    nsec;           /**< \brief nanoseconds within second */
} epicsTimeStamp;

/** \brief Old time-stamp data type, deprecated.
 * \deprecated TS_STAMP was provided for compatibility with Base-3.13 code.
 * It will be removed in some future release of EPICS 7.
 */
#define TS_STAMP epicsTimeStamp


/** \struct timespec
 * \brief Deﬁned by POSIX Real Time
 *
 * This is deﬁned by POSIX Real Time. It requires two mandatory ﬁelds:
 * \li <tt>time_t tv_sec</tt> - Number of seconds since 1970 (The POSIX epoch)
 * \li <tt>long tv_nsec</tt> - nanoseconds within a second
 */
struct timespec; /* POSIX real time */

/** \struct timeval
 * \brief BSD and SRV5 Unix timestamp
 *
 * BSD and SRV5 Unix timestamp. It has two ﬁelds:
 * \li <tt>time_t tv_sec</tt> - Number of seconds since 1970 (The POSIX epoch)
 * \li <tt>time_t tv_nsec</tt> - nanoseconds within a second
 */
struct timeval; /* BSD */

/** \struct l_fp
 * \brief Network Time Protocol timestamp
 *
 * Network Time Protocol timestamp. The ﬁelds are:
 * \li \c lui - Number of seconds since 1900 (The NTP epoch)
 * \li \c luf - Fraction of a second. For example 0x800000000 represents 1/2 second.
 */
struct l_fp; /* NTP timestamp */

#ifdef __cplusplus

/** \brief C++ only ANSI C <tt>struct tm</tt> with nanoseconds, local timezone
 *
 * Extend ANSI C "struct tm" to include nano seconds within a second
 * and a struct tm that is adjusted for the local timezone.
 */
struct local_tm_nano_sec {
    struct tm ansi_tm;  /**< \brief ANSI C time details */
    unsigned long nSec; /**< \brief nanoseconds extension */
};

/** \brief C++ only ANSI C <tt>sruct tm</tt> with nanoseconds, UTC
 *
 * Extend ANSI C "struct tm" to include nanoseconds within a second
 * and a struct tm that is adjusted for GMT (UTC).
 */
struct gm_tm_nano_sec {
    struct tm ansi_tm; /**< \brief ANSI C time details */
    unsigned long nSec; /**< \brief nanoseconds extension */
};

/** \brief C++ only ANSI C time_t
 *
 * This is for converting to/from the ANSI C \c time_t. Since \c time_t
 * is usually an elementary type providing a conversion operator from
 * \c time_t to/from epicsTime could cause undesirable implicit
 * conversions. Providing a conversion operator to/from the
 * \c time_t_wrapper instead prevents implicit conversions.
 */
struct time_t_wrapper {
    time_t ts;
};

/** \brief C++ Event number wrapper class
 *
 * Stores an event number for use by the epicsTime::getEvent() static
 * class method.
 */
class epicsShareClass epicsTimeEvent
{
public:
    epicsTimeEvent (const int &number); /**< \brief Constructor */
    operator int () const;              /**< \brief Extractor */
private:
    int eventNumber;
};

/** \brief C++ time stamp object
 *
 * Holds an EPICS time stamp, and provides conversion functions for both
 * input and output from/to other types.
 *
 * \note Time conversions: The epicsTime implementation will properly
 * convert between the various formats from the beginning of the EPICS
 * epoch until at least 2038. Unless the underlying architecture support
 * has defective POSIX, BSD/SRV5, or standard C time support the EPICS
 * implementation should be valid until 2106.
 */
class epicsShareClass epicsTime
{
public:
     /// \brief Exception: Time provider problem
    class unableToFetchCurrentTime {};
     /// \brief Exception: Bad field(s) in <tt>struct tm</tt>
    class formatProblemWithStructTM {};

    /** \brief The default constructor sets the time to the EPICS epoch. */
    epicsTime ();

    /** \brief Get time of event system event.
     *
     * Returns an epicsTime indicating when the associated event system
     * event last occurred.
     */
    static epicsTime getEvent ( const epicsTimeEvent & );
    /** \brief Get current clock time
     *
     * Returns an epicsTime containing the current time. For example:
     * \code{.cpp}
     *   epicsTime now = epicsTime::getCurrent();
     * \endcode
     */
    static epicsTime getCurrent ();

    /** \name epicsTimeStamp conversions
     * Convert to and from EPICS epicsTimeStamp format
     * @{ */
    /** \brief Convert to epicsTimeStamp */
    operator epicsTimeStamp () const;
    /** \brief Construct from epicsTimeStamp */
    epicsTime ( const epicsTimeStamp & ts );
    /** \brief Assign from epicsTimeStamp */
    epicsTime & operator = ( const epicsTimeStamp & );
    /** @} */

    /** \name ANSI C time_t conversions
     * Convert to and from ANSI C \c time_t wrapper .
     * @{ */
    /** \brief Convert to ANSI C \c time_t */
    operator time_t_wrapper () const;
    /** \brief Construct from ANSI C \c time_t */
    epicsTime ( const time_t_wrapper & );
    /** \brief Assign from ANSI C \c time_t */
    epicsTime & operator = ( const time_t_wrapper & );
    /** @} */

    /** \name ANSI C struct tm local-time conversions
     * Convert to and from ANSI Cs <tt>struct tm</tt> (with nano seconds),
     * adjusted for the local time zone.
     * @{ */
    /** \brief Convert to <tt>struct tm</tt> in local time zone */
    operator local_tm_nano_sec () const;
    /** \brief Construct from <tt>struct tm</tt> in local time zone */
    epicsTime ( const local_tm_nano_sec & );
    /** \brief Assign from <tt>struct tm</tt> in local time zone */
    epicsTime & operator = ( const local_tm_nano_sec & );
    /** @} */

    /** \name ANSI C struct tm UTC conversions
     * Convert to and from ANSI Cs <tt>struct tm</tt> (with nano seconds),
     * adjusted for Greenwich Mean Time (UTC).
     * @{ */
    /** \brief Convert to <tt>struct tm</tt> in UTC/GMT */
    operator gm_tm_nano_sec () const;
    /** \brief Construct from <tt>struct tm</tt> in UTC/GMT */
    epicsTime ( const gm_tm_nano_sec & );
    /** \brief Assign from <tt>struct tm</tt> in UTC */
    epicsTime & operator = ( const gm_tm_nano_sec & );
    /** @} */

    /** \name POSIX RT struct timespec conversions
     * Convert to and from the POSIX RealTime <tt>struct timespec</tt>
     * format.
     * @{ */
    /** \brief Convert to <tt>struct timespec</tt> */
    operator struct timespec () const;
    /** \brief Construct from <tt>struct timespec</tt> */
    epicsTime ( const struct timespec & );
    /** \brief Assign from <tt>struct timespec</tt> */
    epicsTime & operator = ( const struct timespec & );
    /** @} */

    /** \name BSD's struct timeval conversions
     * Convert to and from the BSD <tt>struct timeval</tt> format.
     * @{ */
    /** \brief Convert to <tt>struct timeval</tt> */
    operator struct timeval () const;
    /** \brief Construct from <tt>struct timeval</tt> */
    epicsTime ( const struct timeval & );
    /** \brief Assign from <tt>struct timeval</tt> */
    epicsTime & operator = ( const struct timeval & );
    /** @} */

    /** \name NTP timestamp conversions
     * Convert to and from the NTP timestamp structure \c l_fp
     * @{ */
    /** \brief Convert to NTP format */
    operator l_fp () const;
    /** \brief Construct from NTP format */
    epicsTime ( const l_fp & );
    /** \brief Assign from NTP format */
    epicsTime & operator = ( const l_fp & );
    /** @} */

    /** \name WIN32 FILETIME conversions
     * Convert to and from WIN32s <tt> _FILETIME</tt>
     * \note These are only implemented on Windows targets.
     * @{ */
    /** \brief Convert to Windows <tt>struct _FILETIME</tt> */
    operator struct _FILETIME () const;
    /** \brief Constuct from Windows <tt>struct _FILETIME</tt> */
    epicsTime ( const struct _FILETIME & );
    /** \brief Assign from Windows <tt>struct _FILETIME</tt> */
    epicsTime & operator = ( const struct _FILETIME & );
    /** @} */

    /** \name Arithmetic operators
     * Standard operators involving epicsTime objects and time differences
     * which are always expressed as a \c double in seconds.
     * @{ */
     /// \brief \p lhs minus \p rhs, in seconds
    double operator- ( const epicsTime & ) const;
     /// \brief \p lhs plus rhs seconds
    epicsTime operator+ ( const double & ) const;
     /// \brief \p lhs minus rhs seconds
    epicsTime operator- ( const double & ) const;
     /// \brief add rhs seconds to  \p lhs
    epicsTime operator+= ( const double & );
     /// \brief subtract rhs seconds from \p lhs
    epicsTime operator-= ( const double & );
    /** @} */

    /** \name Comparison operators
     * Standard comparisons between epicsTime objects.
     * @{ */
     /// \brief \p lhs equals \p rhs
     bool operator == ( const epicsTime & ) const;
     /// \brief \p lhs not equal to \p rhs
     bool operator != ( const epicsTime & ) const;
     /// \brief \p rhs no later than \p lhs
     bool operator <= ( const epicsTime & ) const;
     /// \brief \p lhs was before \p rhs
     bool operator < ( const epicsTime & ) const;
     /// \brief \p rhs not before \p lhs
     bool operator >= ( const epicsTime & ) const;
     /// \brief \p lhs was after \p rhs
     bool operator > ( const epicsTime & ) const;
    /** @} */

    /** \brief Convert to string in user-specified format
     *
     * This method extends the standard C library routine strftime().
     * See your OS documentation for details about the standard routine.
     * The epicsTime method adds support for printing the fractional
     * portion of the time. It searches the format string for the
     * sequence <tt>%0<i>n</i>f</tt> where \a n is the desired precision,
     * and uses this format to convert the fractional seconds with the
     * requested precision. For example:
     * \code{.cpp}
     *   epicsTime time = epicsTime::getCurrent();
     *   char buf[30];
     *   time.strftime(buf, 30, "%Y-%m-%d %H:%M:%S.%06f");
     *   printf("%s\n", buf);
     * \endcode
     * This will print the current time in the format:
     * \code
     *   2001-01-26 20:50:29.813505
     * \endcode
     */
    size_t strftime ( char * pBuff, size_t bufLength, const char * pFormat ) const;

    /** \brief Dump current state to standard out */
    void show ( unsigned interestLevel ) const;

private:
    /*
     * private because:
     * a) application does not break when EPICS epoch is changed
     * b) no assumptions about internal storage or internal precision
     * in the application
     * c) it would be easy to forget which argument is nanoseconds
     * and which argument is seconds (no help from compiler)
     */
    epicsTime ( const unsigned long secPastEpoch, const unsigned long nSec );
    void addNanoSec ( long nanoSecAdjust );

    unsigned long secPastEpoch; /**< \brief seconds since 0000 Jan 1, 1990 */
    unsigned long nSec; /**< \brief nanoseconds within second */
};

extern "C" {
#endif /* __cplusplus */

/** \name Return status values
 * epicsTime routines return \c S_time_ error status values:
 * @{
 */
#define epicsTimeOK 0                       /**< \brief Okay */
#define S_time_noProvider       (M_time| 1) /**< \brief No time provider*/
#define S_time_badEvent         (M_time| 2) /**< \brief Bad event number*/
#define S_time_badArgs          (M_time| 3) /**< \brief Invalid arguments*/
#define S_time_noMemory         (M_time| 4) /**< \brief Out of memory*/
#define S_time_unsynchronized   (M_time| 5) /**< \brief Provider not synchronized*/
#define S_time_timezone         (M_time| 6) /**< \brief Invalid timeone*/
#define S_time_conversion       (M_time| 7) /**< \brief Time conversion error*/
/** @} */

/** \name epicsTimeEvent numbers
 * Some special values for eventNumber:
 * @{
 */
#define epicsTimeEventCurrentTime 0
#define epicsTimeEventBestTime -1
#define epicsTimeEventDeviceTime -2
/** @} */

/** \name generalTime functions
 * These are implemented in the "generalTime" framework:
 * @{
 */
/** \brief Get current time into \p *pDest */
epicsShareFunc int epicsShareAPI epicsTimeGetCurrent ( epicsTimeStamp * pDest );
/** \brief Get time of event \p eventNumber into \p *pDest */
epicsShareFunc int epicsShareAPI epicsTimeGetEvent (
    epicsTimeStamp *pDest, int eventNumber);
/** @} */

/** \name ISR-callable
 * These routines may be called from an Interrupt Service Routine, and
 * will return a value from the last current time or event time provider
 * that sucessfully returned a result from the equivalent non-ISR routine.
 * @{
 */
/** \brief Get current time into \p *pDest (ISR-safe) */
epicsShareFunc int epicsTimeGetCurrentInt(epicsTimeStamp *pDest);
/** \brief Get time of event \p eventNumber into \p *pDest (ISR-safe) */
epicsShareFunc int epicsTimeGetEventInt(epicsTimeStamp *pDest, int eventNumber);
/** @} */

/** \name ANSI C time_t conversions
 * Convert to and from ANSI C \c time_t
 * @{
 */
/** \brief Convert epicsTimeStamp to ANSI C \c time_t */
epicsShareFunc int epicsShareAPI epicsTimeToTime_t (
    time_t * pDest, const epicsTimeStamp * pSrc );
/** \brief Convert ANSI C \c time_t to epicsTimeStamp */
epicsShareFunc int epicsShareAPI epicsTimeFromTime_t (
    epicsTimeStamp * pDest, time_t src );
/** @} */

/** \name ANSI C struct tm conversions
 * Convert to and from ANSI C's <tt>struct tm</tt> with nanoseconds
 * @{
 */
/** \brief Convert epicsTimeStamp to <tt>struct tm</tt> in local time zone */
epicsShareFunc int epicsShareAPI epicsTimeToTM (
    struct tm * pDest, unsigned long * pNSecDest, const epicsTimeStamp * pSrc );
/** \brief Convert epicsTimeStamp to <tt>struct tm</tt> in UTC/GMT */
epicsShareFunc int epicsShareAPI epicsTimeToGMTM (
    struct tm * pDest, unsigned long * pNSecDest, const epicsTimeStamp * pSrc );
/** \brief Set epicsTimeStamp from <tt>struct tm</tt> in local time zone */
epicsShareFunc int epicsShareAPI epicsTimeFromTM (
    epicsTimeStamp * pDest, const struct tm * pSrc, unsigned long nSecSrc );
/** \brief Set epicsTimeStamp from <tt>struct tm</tt> in UTC/GMT */
epicsShareFunc int epicsShareAPI epicsTimeFromGMTM (
    epicsTimeStamp * pDest, const struct tm * pSrc, unsigned long nSecSrc );
/** @} */

/** \name POSIX RT struct timespec conversions
 * Convert to and from the POSIX RealTime <tt>struct timespec</tt>
 * format.
 * @{ */
/** \brief Convert epicsTimeStamp to <tt>struct timespec</tt> */
epicsShareFunc int epicsShareAPI epicsTimeToTimespec (
    struct timespec * pDest, const epicsTimeStamp * pSrc );
/** \brief Set epicsTimeStamp from <tt>struct timespec</tt> */
epicsShareFunc int epicsShareAPI epicsTimeFromTimespec (
    epicsTimeStamp * pDest, const struct timespec * pSrc );
/** @} */

/** \name BSD's struct timeval conversions
 * Convert to and from the BSD <tt>struct timeval</tt> format.
 * @{ */
/** \brief Convert epicsTimeStamp to <tt>struct timeval</tt> */
epicsShareFunc int epicsShareAPI epicsTimeToTimeval (
    struct timeval * pDest, const epicsTimeStamp * pSrc );
/** \brief Set epicsTimeStamp from <tt>struct timeval</tt> */
epicsShareFunc int epicsShareAPI epicsTimeFromTimeval (
    epicsTimeStamp * pDest, const struct timeval * pSrc );
/** @} */

/** \name Arithmetic operations
 * Arithmetic operations on epicsTimeStamp objects and time differences
 * which are always expressed as a \c double in seconds.
 * @{ */
/** \brief Time difference between \p left and \p right in seconds. */
epicsShareFunc double epicsShareAPI epicsTimeDiffInSeconds (
    const epicsTimeStamp * pLeft, const epicsTimeStamp * pRight );/* left - right */
/** \brief Add some number of seconds to \p dest */
epicsShareFunc void epicsShareAPI epicsTimeAddSeconds (
    epicsTimeStamp * pDest, double secondsToAdd ); /* adds seconds to *pDest */
/** @} */

/** \name Comparison operators
 * Comparisons between epicsTimeStamp objects, returning 0=false, 1=true.
 * @{ */
/** \brief \p left equals \p right */
epicsShareFunc int epicsShareAPI epicsTimeEqual (
    const epicsTimeStamp * pLeft, const epicsTimeStamp * pRight);
/** \brief \p left not equal to \p right */
epicsShareFunc int epicsShareAPI epicsTimeNotEqual (
    const epicsTimeStamp * pLeft, const epicsTimeStamp * pRight);
/** \brief \p left was before \p right */
epicsShareFunc int epicsShareAPI epicsTimeLessThan (
    const epicsTimeStamp * pLeft, const epicsTimeStamp * pRight);
/** \brief \p right was no later than \p left */
epicsShareFunc int epicsShareAPI epicsTimeLessThanEqual (
    const epicsTimeStamp * pLeft, const epicsTimeStamp * pRight);
/** \brief \p left was after \p right */
epicsShareFunc int epicsShareAPI epicsTimeGreaterThan (
    const epicsTimeStamp * pLeft, const epicsTimeStamp * pRight);
/** \brief \p right was not before \p left */
epicsShareFunc int epicsShareAPI epicsTimeGreaterThanEqual (
    const epicsTimeStamp * pLeft, const epicsTimeStamp * pRight);
/** @} */

/** \brief Convert epicsTimeStamp to string. See epicsTime::strftime() */
epicsShareFunc size_t epicsShareAPI epicsTimeToStrftime (
    char * pBuff, size_t bufLength, const char * pFormat, const epicsTimeStamp * pTS );

/** \brief Dump current state to stdout */
epicsShareFunc void epicsShareAPI epicsTimeShow (
    const epicsTimeStamp *, unsigned interestLevel );

/** \name Reentrant time_t to struct tm conversions
 * OS-specific reentrant versions of the ANSI C interface because the
 * vxWorks \c gmtime_r interface does not match POSIX standards
 * @{ */
/** \brief Break down a \c time_t into a <tt>struct tm</tt> in the local timezone */
epicsShareFunc int epicsShareAPI epicsTime_localtime ( const time_t * clock, struct tm * result );
/** \brief Break down a \c time_t into a <tt>struct tm</tt> in the UTC timezone */
epicsShareFunc int epicsShareAPI epicsTime_gmtime ( const time_t * clock, struct tm * result );
/** @} */

/** \name Monotonic time routines
* @{ */
/** \brief Monotonic time resolution, may not be accurate. Returns the
 * minimum non-zero time difference between two calls to epicsMonotonicGet()
 * in units of nanoseconds.
 */
epicsShareFunc epicsUInt64 epicsMonotonicResolution(void);
/** \brief Fetch monotonic counter, returns the number of nanoseconds since
 * some unspecified time. */
epicsShareFunc epicsUInt64 epicsMonotonicGet(void);
/** @} */

#ifdef EPICS_EXPOSE_LIBCOM_MONOTONIC_PRIVATE
epicsShareFunc void osdMonotonicInit(void);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

/* inline member functions ,*/
#ifdef __cplusplus

/* epicsTimeEvent */

inline epicsTimeEvent::epicsTimeEvent (const int &number) :
    eventNumber(number) {}

inline epicsTimeEvent::operator int () const
{
    return this->eventNumber;
}


/* epicsTime */

inline epicsTime epicsTime::operator - ( const double & rhs ) const
{
    return epicsTime::operator + ( -rhs );
}

inline epicsTime epicsTime::operator += ( const double & rhs )
{
    *this = epicsTime::operator + ( rhs );
    return *this;
}

inline epicsTime epicsTime::operator -= ( const double & rhs )
{
    *this = epicsTime::operator + ( -rhs );
    return *this;
}

inline bool epicsTime::operator == ( const epicsTime & rhs ) const
{
    if ( this->secPastEpoch == rhs.secPastEpoch && this->nSec == rhs.nSec ) {
    	return true;
    }
    return false;
}

inline bool epicsTime::operator != ( const epicsTime & rhs ) const
{
    return !epicsTime::operator == ( rhs );
}

inline bool epicsTime::operator >= ( const epicsTime & rhs ) const
{
    return ! ( *this < rhs );
}

inline bool epicsTime::operator > ( const epicsTime & rhs ) const
{
    return ! ( *this <= rhs );
}

inline epicsTime & epicsTime::operator = ( const local_tm_nano_sec & rhs )
{
    return *this = epicsTime ( rhs );
}

inline epicsTime & epicsTime::operator = ( const gm_tm_nano_sec & rhs )
{
    return *this = epicsTime ( rhs );
}

inline epicsTime & epicsTime::operator = ( const struct timespec & rhs )
{
    *this = epicsTime ( rhs );
    return *this;
}

inline epicsTime & epicsTime::operator = ( const epicsTimeStamp & rhs )
{
    *this = epicsTime ( rhs );
    return *this;
}

inline epicsTime & epicsTime::operator = ( const l_fp & rhs )
{
    *this = epicsTime ( rhs );
    return *this;
}

inline epicsTime & epicsTime::operator = ( const time_t_wrapper & rhs )
{
    *this = epicsTime ( rhs );
    return *this;
}
#endif /* __cplusplus */

#endif /* epicsTimehInclude */
