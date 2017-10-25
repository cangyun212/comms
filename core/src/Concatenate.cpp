#include "Core.hpp"

#include "Concatenate.hpp"

namespace sg
{
    namespace
    {
        int s_floatPrecision = 6;
    }

    int GetConcatenateFloatPrecision()
    {
        return s_floatPrecision;
    }

    void SetConcatenateFloatPrecision(int precision)
    {
        s_floatPrecision = precision;
    }
}

