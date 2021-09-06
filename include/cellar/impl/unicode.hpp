

namespace feiparser
{
    typedef int CodePoint;

    static const int InvalidCodePoint = -1;

    int utf8length(unsigned char c)
    {
        if((c>>7)==0) return 1;
        if((c>>5)==6) return 2;
        if((c>>4) == 14) return 3;
        if((c>>3) == 30) return 4;
        return 1;
    }

    bool iscontinuation(unsigned char c)
    {
        return (c>>6)==2;
    }

    template<typename It>
    bool isSafeUtf8Stream(It start, It end)
    {
        int len=0;
        for(It i=end; i!=start;)
        {
            --i;
            ++len;

            if(!iscontinuation(*i))
            {
                return len == utf8length(*i);
            }
        }

        return false;
    }

    template<typename It>
    CodePoint readutf8unchecked(It current)
    {
        switch(utf8length(*current))
        {
        case 1:
            return *current;
        case 2:
            return iscontinuation(current[1]) ?
                (CodePoint(current[0]&31)<<6) | (current[1]&63) :
                InvalidCodePoint;
        case 3:
            return iscontinuation(current[1]) && iscontinuation(current[2]) ? 
                    (CodePoint(current[0]&15)<<12) | (CodePoint(current[1]&63)<<6) | (current[2]&63) :
                    InvalidCodePoint;
        case 4:
            return iscontinuation(current[1]) && iscontinuation(current[2]) && iscontinuation(current[3]) ? 
                    (CodePoint(current[0]&7)<<18) | (CodePoint(current[1]&63)<<12) |(CodePoint(current[2]&63)<<6) | (current[3]&63) :
                    InvalidCodePoint;
        default:
            return InvalidCodePoint;
        }
    }

    template<typename It>
    CodePoint readutf8(It start, It end)
    {
        if(start == end) return InvalidCodePoint;
        int len = utf8length(*start);
        if(start+len >= end) return InvalidCodePoint;
        return readutf8unchecked(start);
    }

    template<typename It>
    class utf8iterator
    {
        It current;

        typedef CodePoint value_type;

        // Beware buffer overflows...
        // The stream *must* be terminated by a zero, or be validated
        value_type operator*() const
        {
            return readutf8unchecked(current);
        }

        utf8iterator & operator++()
        {
            current += utf8length(*current);
            return *this;
        }
    };

    template<typename It>
    class unicode_stream
    {

    };

    enum class UnicodeClass
    {

    };

    UnicodeClass unicodeClass(int codepoint);
}
