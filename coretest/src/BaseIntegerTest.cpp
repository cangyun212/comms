
#include "coretest.hpp"

#include <limits>
#include <list>
#include <vector>

#include "BaseInteger.hpp"

using namespace sg;

static int s_base20CodeSet[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };
static char s_base8CodeSet[] = { '0', '1', '2', '3', '4', '5', '6', '7' };
static char s_base16CodeSet[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

static BaseType sHexCharToNum(char c)
{
    SG_ASSERT((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'));
    if (c >= '0' && c <= '9')
        return c - '0';

    return c - 'A' + 10;
}

TEST(BaseInteger, BaseStringCodeSet)
{
    EXPECT_EQ(BaseHexCodeSet::CodeSet(),
        std::string("0123456789ABCDEF"));
    EXPECT_EQ(BaseDecimalCodeSet::CodeSet(),
        std::string("0123456789"));
    EXPECT_EQ(BaseBinaryCodeSet::CodeSet(),
        std::string("01"));

    typedef BaseStringCodeSet<'~', '!', '@', '#', '$', '%', '^', '&'> CustomOctCodeSet;
    EXPECT_EQ(CustomOctCodeSet::CodeSet(),
        std::string("~!@#$%^&"));
}

TEST(BaseInteger, BaseCode)
{
    typedef BaseStringCodeSet<'~', '!', '@', '#', '$', '%', '^', '&'> CustomOctCodeSet;

    BaseHex hexCode1, hexCode2;
    BaseDecimal decCode;
    BaseBinary binCode;
    BaseCode<8, CustomOctCodeSet> octCode;

    EXPECT_EQ(16,hexCode1.GetBase());
    EXPECT_EQ(16, hexCode1.GetBase());
    EXPECT_EQ(10, decCode.GetBase());
    EXPECT_EQ(2, binCode.GetBase());
    EXPECT_EQ(8, octCode.GetBase());

    auto rhex1 = hexCode1.Translate('0');
    EXPECT_TRUE(rhex1.valid);
    EXPECT_EQ(0, rhex1.num);
    rhex1 = hexCode1.Translate('F');
    EXPECT_TRUE(rhex1.valid);
    EXPECT_EQ(0xF, rhex1.num);
    rhex1 = hexCode1.Translate('W');
    EXPECT_FALSE(rhex1.valid);
    auto rhex2 = hexCode2.Translate('8');
    EXPECT_TRUE(rhex2.valid);
    EXPECT_EQ(8, rhex2.num);
    rhex2 = hexCode2.Translate('B');
    EXPECT_TRUE(rhex2.valid);
    EXPECT_EQ(0xB, rhex2.num);
    rhex2 = hexCode2.Translate('$');
    EXPECT_FALSE(rhex2.valid);
    
    auto rdec = decCode.Translate('1');
    EXPECT_TRUE(rdec.valid);
    EXPECT_EQ(1, rdec.num);
    rdec = decCode.Translate('A');
    EXPECT_FALSE(rdec.valid);

    auto rbin = binCode.Translate('0');
    EXPECT_TRUE(rbin.valid);
    EXPECT_EQ(0, rbin.num);
    rbin = binCode.Translate('1');
    EXPECT_TRUE(rbin.valid);
    EXPECT_EQ(1, rbin.num);
    rbin = binCode.Translate('3');
    EXPECT_FALSE(rbin.valid);

    auto roct = octCode.Translate('~');
    EXPECT_TRUE(roct.valid);
    EXPECT_EQ(0, roct.num);
    roct = octCode.Translate('&');
    EXPECT_TRUE(roct.valid);
    EXPECT_EQ(7, roct.num);
    roct = octCode.Translate('4');
    EXPECT_FALSE(roct.valid);

    std::vector<char> customHexCodeSet{{
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
        }};
    BaseCode<16, std::vector<char>> customHexCode(customHexCodeSet);
    EXPECT_EQ(16, customHexCode.GetBase());
    auto rhex = customHexCode.Translate('1');
    EXPECT_TRUE(rhex.valid);
    EXPECT_EQ(1, rhex.num);
    rhex = customHexCode.Translate('E');
    EXPECT_TRUE(rhex.valid);
    EXPECT_EQ(0xE, rhex.num);
    rhex = customHexCode.Translate('G');
    EXPECT_FALSE(rhex.valid);

    BaseCode<20, int[20]> customBase20Code(s_base20CodeSet);
    EXPECT_EQ(20, customBase20Code.GetBase());
    auto r20 = customBase20Code.Translate(0);
    EXPECT_TRUE(r20.valid);
    EXPECT_EQ(0, r20.num);
    r20 = customBase20Code.Translate(19);
    EXPECT_TRUE(r20.valid);
    EXPECT_EQ(19, r20.num);
    r20 = customBase20Code.Translate(20);
    EXPECT_FALSE(r20.valid);
}

TEST(BaseInteger, BaseIntegerWithBaseCode)
{
    BaseHex hexCode;
    HexInteger hex1("7E6FA1DB460B0", hexCode);

    BaseCode<16, char[16]> customHexCode(s_base16CodeSet);
    std::vector<char> hexValueSeq{ {'7', 'E', '6', 'F', 'A', '1', 'D', 'B', '4', '6', '0', 'B', '0'} };
    HexInteger hex2(hexValueSeq, customHexCode);

    ASSERT_EQ(hexValueSeq.size(), hex1.GetCounts());
    ASSERT_EQ(hex1.GetCounts(), hex2.GetCounts());
    for (size_t i = 0; i < hexValueSeq.size(); ++i)
    {
        BaseType num = sHexCharToNum(hexValueSeq[hexValueSeq.size() - i - 1]);
        EXPECT_EQ(num, hex1.GetDigit(i));
        EXPECT_EQ(num, hex2.GetDigit(i));
    }

    BaseCode<8, char[8]> octBaseCode(s_base8CodeSet);
    const char *octValue = "1000256";
    BaseInteger<8> oct(octValue, 7, octBaseCode);

    ASSERT_EQ(7, oct.GetCounts());
    for (size_t i = 0; i < oct.GetCounts(); ++i)
    {
        BaseType num = sHexCharToNum(octValue[oct.GetCounts() - i - 1]);
        EXPECT_EQ(num, oct.GetDigit(i));
    }

    BaseCode<20, int[20]> customBase20Code(s_base20CodeSet);
    std::vector<int> base20ValueSeq{ {19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0} };
    BaseInteger<20> base20Num(base20ValueSeq, customBase20Code);
    
    ASSERT_EQ(base20ValueSeq.size(), base20Num.GetCounts());
    for (size_t i = 0; i < base20ValueSeq.size(); ++i)
    {
        EXPECT_EQ(base20ValueSeq[base20ValueSeq.size() - i - 1], base20Num.GetDigit(i));
    }
}

TEST(BaseInteger, BaseIntegerWithUint64)
{
    uint64_t a = 3;
    HexInteger hex(a);
    ASSERT_EQ(1, hex.GetCounts());
    EXPECT_EQ(3, hex.GetDigit(0));
    DecimalInteger dec(a);
    ASSERT_EQ(1, dec.GetCounts());
    EXPECT_EQ(3, dec.GetDigit(0));
    BinaryInteger bin(a);
    ASSERT_EQ(2, bin.GetCounts());
    EXPECT_EQ(1, bin.GetDigit(0));
    EXPECT_EQ(1, bin.GetDigit(1));

    uint64_t b = 83442;
    hex = b;
    BaseType hexB[] = { 0x1, 0x4, 0x5, 0xF, 0x2 };
    ASSERT_EQ(SG_ARRAY_SIZE(hexB), hex.GetCounts());
    for (size_t i = 0; i < hex.GetCounts(); ++i)
    {
        EXPECT_EQ(hexB[hex.GetCounts() - i - 1], hex.GetDigit(i));
    }
    dec = b;
    BaseType decB[] = { 8, 3, 4, 4, 2 };
    ASSERT_EQ(SG_ARRAY_SIZE(decB), dec.GetCounts());
    for (size_t i = 0; i < dec.GetCounts(); ++i)
    {
        EXPECT_EQ(decB[dec.GetCounts() - i - 1], dec.GetDigit(i));
    }
    bin = b;
    BaseType binB[] = { 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0 };
    ASSERT_EQ(SG_ARRAY_SIZE(binB), bin.GetCounts());
    for (size_t i = 0; i < bin.GetCounts(); ++i)
    {
        EXPECT_EQ(binB[bin.GetCounts() - i - 1], bin.GetDigit(i));
    }

    uint64_t c = 426;
    hex = c;
    ASSERT_EQ(3, hex.GetCounts());
    EXPECT_EQ(0xA, hex.GetDigit(0));
    EXPECT_EQ(0xA, hex.GetDigit(1));
    EXPECT_EQ(0x1, hex.GetDigit(2));
    dec = c;
    ASSERT_EQ(3, dec.GetCounts());
    EXPECT_EQ(6, dec.GetDigit(0));
    EXPECT_EQ(2, dec.GetDigit(1));
    EXPECT_EQ(4, dec.GetDigit(2));
    bin = c;
    BaseType binC[] = { 1, 1, 0, 1, 0, 1, 0, 1, 0 };
    ASSERT_EQ(SG_ARRAY_SIZE(binC), bin.GetCounts());
    for (size_t i = 0; i < bin.GetCounts(); ++i)
    {
        EXPECT_EQ(binC[bin.GetCounts() - i - 1], bin.GetDigit(i));
    }

    uint64_t d = 518797;
    hex = d;
    BaseType hexD[] = { 0x7, 0xE, 0xA, 0x8, 0xD };
    ASSERT_EQ(SG_ARRAY_SIZE(hexD), hex.GetCounts());
    for (size_t i = 0; i < hex.GetCounts(); ++i)
    {
        EXPECT_EQ(hexD[hex.GetCounts() - i - 1], hex.GetDigit(i));
    }
    dec = d;
    BaseType decD[] = { 5, 1, 8, 7, 9, 7 };
    ASSERT_EQ(SG_ARRAY_SIZE(decD), dec.GetCounts());
    for (size_t i = 0; i < dec.GetCounts(); ++i)
    {
        EXPECT_EQ(decD[dec.GetCounts() - i - 1], dec.GetDigit(i));
    }
    bin = d;
    BaseType binD[] = { 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1};
    ASSERT_EQ(SG_ARRAY_SIZE(binD), bin.GetCounts());
    for (size_t i = 0; i < bin.GetCounts(); ++i)
    {
        EXPECT_EQ(binD[bin.GetCounts() - i - 1], bin.GetDigit(i));
    }

    uint64_t e = std::numeric_limits<uint64_t>::max();
    hex = e;
    ASSERT_EQ(16, hex.GetCounts());
    for (size_t i = 0; i < hex.GetCounts(); ++i)
    {
        EXPECT_EQ(0xF, hex.GetDigit(i));
    }
    dec = e;
    BaseType decE[] = { 1, 8, 4, 4, 6, 7, 4, 4, 0, 7, 3, 7, 0, 9, 5, 5, 1, 6, 1, 5 };
    ASSERT_EQ(SG_ARRAY_SIZE(decE), dec.GetCounts());
    for (size_t i = 0; i < dec.GetCounts(); ++i)
    {
        EXPECT_EQ(decE[dec.GetCounts() - i - 1], dec.GetDigit(i));
    }
    bin = e;
    ASSERT_EQ(64, bin.GetCounts());
    for (size_t i = 0; i < bin.GetCounts(); ++i)
    {
        EXPECT_EQ(1, bin.GetDigit(i));
    }
}

TEST(BaseInteger, BaseConvertion)
{
    BaseHex hexCode;
    HexInteger hex("7E6FA1DB460B0", hexCode);
    DecimalInteger dec(hex);
    BaseType decA[] = { 2, 2, 2, 4, 2, 8, 6, 7, 5, 1, 5, 4, 7, 5, 6, 8 };
    ASSERT_EQ(SG_ARRAY_SIZE(decA), dec.GetCounts());
    for (size_t i = 0; i < dec.GetCounts(); ++i)
    {
        EXPECT_EQ(decA[dec.GetCounts() - i - 1], dec.GetDigit(i));
    }

    BaseDecimal decCode;
    DecimalInteger dec2("999999999999999999", decCode);
    hex = dec2;
    BaseType hexA[] = { 0xD, 0xE, 0x0, 0xB, 0x6, 0xB, 0x3, 0xA, 0x7, 0x6, 0x3, 0xF, 0xF, 0xF, 0xF };
    ASSERT_EQ(SG_ARRAY_SIZE(hexA), hex.GetCounts());
    for (size_t i = 0; i < hex.GetCounts(); ++i)
    {
        EXPECT_EQ(hexA[hex.GetCounts() - i - 1], hex.GetDigit(i));
    }

    DecimalInteger dec3("576460752303423488", decCode);
    HexInteger hec2(dec3);
    ASSERT_EQ(15, hec2.GetCounts());
    EXPECT_EQ(0x8, hec2.GetDigit(14));
    for (size_t i = 0; i < (hec2.GetCounts() - 1); ++i)
    {
        EXPECT_EQ(0x0, hec2.GetDigit(i));
    }

    HexInteger hec3(hec2);
    ASSERT_EQ(15, hec3.GetCounts());
    EXPECT_EQ(0x8, hec3.GetDigit(14));
    for (size_t i = 0; i < (hec3.GetCounts() - 1); ++i)
    {
        EXPECT_EQ(0x0, hec3.GetDigit(i));
    }

    DecimalInteger dec4(0);
    ASSERT_EQ(1, dec4.GetCounts());
    EXPECT_EQ(0, dec4.GetDigit(0));
    dec4 = dec2;
    ASSERT_EQ(dec2.GetCounts(), dec4.GetCounts());
    for (size_t i = 0; i < dec2.GetCounts(); ++i)
    {
        EXPECT_EQ(dec2.GetDigit(i), dec4.GetDigit(i));
    }
}

TEST(BaseInteger, OperatorAddEqual)
{
    DecimalInteger dec1(9), dec2(1);
    dec1 += dec2;
    ASSERT_EQ(2, dec1.GetCounts());
    EXPECT_EQ(0, dec1.GetDigit(0));
    EXPECT_EQ(1, dec1.GetDigit(1));

    DecimalInteger dec3(58);
    dec3 += 3;
    ASSERT_EQ(2, dec3.GetCounts());
    EXPECT_EQ(1, dec3.GetDigit(0));
    EXPECT_EQ(6, dec3.GetDigit(1));

    BaseDecimal decCode;
    DecimalInteger dec4("346", decCode);
    std::list<char> decA{ {'7', '1', '1'} };
    DecimalInteger dec5(decA, decCode);
    dec4 += dec5;
    ASSERT_EQ(4, dec4.GetCounts());
    EXPECT_EQ(7, dec4.GetDigit(0));
    EXPECT_EQ(5, dec4.GetDigit(1));
    EXPECT_EQ(0, dec4.GetDigit(2));
    EXPECT_EQ(1, dec4.GetDigit(3));

    HexInteger hex1(0x3), hex2(0x8);
    hex1 += hex2;
    ASSERT_EQ(1, hex1.GetCounts());
    EXPECT_EQ(0xB, hex1.GetDigit(0));

    HexInteger hex3(0x9);
    hex3 += 0x7;
    ASSERT_EQ(2, hex3.GetCounts());
    EXPECT_EQ(0x0, hex3.GetDigit(0));
    EXPECT_EQ(0x1, hex3.GetDigit(1));

    BaseHex hexCode;
    HexInteger hex4("3F", hexCode);
    hex4 += 0x2;
    ASSERT_EQ(2, hex4.GetCounts());
    EXPECT_EQ(0x1, hex4.GetDigit(0));
    EXPECT_EQ(0x4, hex4.GetDigit(1));

    hex3 = 0xA3B80FF;
    hex4 = 0xD5396EB;
    hex3 += hex4;
    ASSERT_EQ(8, hex3.GetCounts());
    BaseType hexValue[] = { 0x1, 0x7, 0x8, 0xF, 0x1, 0x7, 0xE, 0xA };
    for (size_t i = 0; i < SG_ARRAY_SIZE(hexValue); ++i)
    {
        EXPECT_EQ(hexValue[SG_ARRAY_SIZE(hexValue) - i - 1], hex3.GetDigit(i));
    }
}

TEST(BaseInteger, OperatorMulEqual)
{
    DecimalInteger dec1(0), dec2(7);
    dec1 *= dec2;
    ASSERT_EQ(1, dec1.GetCounts());
    EXPECT_EQ(0, dec1.GetDigit(0));

    BaseDecimal decCode;
    DecimalInteger dec3(256);
    DecimalInteger dec4("0", decCode);
    dec3 *= dec4;
    ASSERT_EQ(1, dec3.GetCounts());
    EXPECT_EQ(0, dec3.GetDigit(0));

    DecimalInteger dec5(4), dec6(5);
    dec5 *= dec6;
    ASSERT_EQ(2, dec5.GetCounts());
    EXPECT_EQ(0, dec5.GetDigit(0));
    EXPECT_EQ(2, dec5.GetDigit(1));

    std::vector<char> decValue{ {'2', '1'} };
    DecimalInteger dec7(decValue, decCode);
    const char *p = "3";
    DecimalInteger dec8(p, 1, decCode);
    dec7 *= dec8;
    ASSERT_EQ(2, dec7.GetCounts());
    EXPECT_EQ(3, dec7.GetDigit(0));
    EXPECT_EQ(6, dec7.GetDigit(1));

    DecimalInteger dec9(635), dec10(28);
    dec9 *= dec10;
    ASSERT_EQ(5, dec9.GetCounts());
    BaseType decA[] = { 1, 7, 7, 8, 0 };
    for (size_t i = 0; i < SG_ARRAY_SIZE(decA); ++i)
    {
        EXPECT_EQ(decA[SG_ARRAY_SIZE(decA) - i - 1], dec9.GetDigit(i));
    }

    DecimalInteger dec11(11), dec12(132);
    dec11 *= dec12;
    ASSERT_EQ(4, dec11.GetCounts());
    EXPECT_EQ(2, dec11.GetDigit(0));
    EXPECT_EQ(5, dec11.GetDigit(1));
    EXPECT_EQ(4, dec11.GetDigit(2));
    EXPECT_EQ(1, dec11.GetDigit(3));

    DecimalInteger dec13(2051), dec14(4007);
    dec13 *= dec14;
    BaseType decB[] = { 8, 2, 1, 8, 3, 5, 7 };
    ASSERT_EQ(SG_ARRAY_SIZE(decB), dec13.GetCounts());
    for (size_t i = 0; i < SG_ARRAY_SIZE(decB); ++i)
    {
        EXPECT_EQ(decB[SG_ARRAY_SIZE(decB) - i - 1], dec13.GetDigit(i));
    }

    HexInteger hex1(0x0), hex2(0xFF);
    hex1 *= hex2;
    ASSERT_EQ(1, hex1.GetCounts());
    EXPECT_EQ(0, hex1.GetDigit(0));

    BaseHex hexCode;
    HexInteger hex3("CD", hexCode);
    HexInteger hex4(0x15);
    hex3 *= hex4;
    BaseType hexA[] = { 1, 0, 0xD, 1 };
    ASSERT_EQ(SG_ARRAY_SIZE(hexA), hex3.GetCounts());
    for (size_t i = 0; i < SG_ARRAY_SIZE(hexA); ++i)
    {
        EXPECT_EQ(hexA[SG_ARRAY_SIZE(hexA) - i - 1], hex3.GetDigit(i));
    }

    HexInteger hex5("F16AE", hexCode), hex6("BD0", hexCode);
    hex5 *= hex6;
    BaseType hexB[] = { 0xB, 2, 3, 0xB, 0xE, 7, 6, 0 };
    ASSERT_EQ(SG_ARRAY_SIZE(hexB), hex5.GetCounts());
    for (size_t i = 0; i < SG_ARRAY_SIZE(hexB); ++i)
    {
        EXPECT_EQ(hexB[SG_ARRAY_SIZE(hexB) - i - 1], hex5.GetDigit(i));
    }
}

TEST(BaseInteger, ZeroCheck)
{
    DecimalInteger dec1(10), dec2(0);
    EXPECT_FALSE(dec1.IsZero());
    EXPECT_TRUE(dec2.IsZero());

    HexInteger hex1(dec2);
    EXPECT_TRUE(hex1.IsZero());
    hex1 *= 55;
    EXPECT_TRUE(hex1.IsZero());

    HexInteger hex2(78);
    hex2 *= hex1;
    EXPECT_TRUE(hex2.IsZero());
}

