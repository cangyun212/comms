#ifndef _QCOM_INLINE_H_
#define _QCOM_INLINE_H_

static __inline__ bool _QComIsPrint(const char *str, int len)
{
    while (len--)
    {
      if ( *str < 0x20 || *str > 0x7A )       //this range is defined by QCOM protocol
        return FALSE;
      ++str;
    }
    return TRUE;
}

static __inline__ void _QComPutBCD(u32 value, u8 *bcdArray, int byteCount)
{
    while (byteCount--)
    {
        u8 digit = value % 10;

        value /= 10;
        digit |= ((value % 10) << 4);
        value /= 10;
        *bcdArray++ = digit;
    }
}

static __inline__ bool _QComGetBCD(u32 *value, const u8 *bcdArray, int byteCount)
{
    *value = 0;
    while (byteCount--)
    {
        u8 digit = bcdArray[byteCount];

        if (((digit & 0x0F) > 9) || ((digit >> 4) > 9))
            return FALSE;
        *value = *value * 100 + (digit >> 4) * 10 + (digit & 0x0F);
    }
    return TRUE;
}

static __inline__ void SecondsToCalendar(time_t time, struct tm *cTime)
{
  localtime_r(&time, cTime);
  //  gmtime_r(&time, cTime);

}

static __inline__ bool _QComTestBCD(u8 *bcdArray, int byteCount)
{
    while (byteCount--)
    {
        u8 digit = bcdArray[byteCount];

        if (((digit & 0x0F) > 9) || ((digit >> 4) > 9))
            return FALSE;
    }
    return TRUE;
}

#endif
