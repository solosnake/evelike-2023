#include "solosnake/html.hpp"

namespace solosnake
{
    const char* htmlescape( const char* txt )
    {
        if( txt )
        {
            if( txt[0] < 127 )
            {
                switch( txt[0] )
                {
                    case 0:
                        return "&#00;";
                    case 1:
                        return "&#01;";
                    case 2:
                        return "&#02;";
                    case 3:
                        return "&#03;";
                    case 4:
                        return "&#04;";
                    case 5:
                        return "&#05;";
                    case 6:
                        return "&#06;";
                    case 7:
                        return "&#07;";
                    case 8:
                        return "&#08;";
                    case '\t':
                        return "&#09;";

                    case '\n':
                        return "&#10;";
                    case 11:
                        return "&#11;";
                    case 12:
                        return "&#12;";
                    case '\r':
                        return "&#13;";
                    case 14:
                        return "&#14;";

                    case 15:
                        return "&#15;";
                    case 16:
                        return "&#16;";
                    case 17:
                        return "&#17;";
                    case 18:
                        return "&#18;";
                    case 19:
                        return "&#19;";

                    case 20:
                        return "&#20;";
                    case 21:
                        return "&#21;";
                    case 22:
                        return "&#22;";
                    case 23:
                        return "&#23;";
                    case 24:
                        return "&#24;";

                    case 25:
                        return "&#25;";
                    case 26:
                        return "&#26;";
                    case 27:
                        return "&#27;";
                    case 28:
                        return "&#28;";
                    case 29:
                        return "&#29;";

                    case 30:
                        return "&#30;";
                    case 31:
                        return "&#31;";

                    case ' ':
                        return "&#32;";
                    case '!':
                        return "&#33;";
                    case '\"':
                        return "&#34;";

                    case '#':
                        return "&#35;";
                    case '$':
                        return "&#36;";
                    case '%':
                        return "&#37;";
                    case '&':
                        return "&#38;";
                    case '\'':
                        return "&#39;";

                    case '(':
                        return "&#40;";
                    case ')':
                        return "&#41;";
                    case '*':
                        return "&#42;";
                    case '+':
                        return "&#43;";
                    case ',':
                        return "&#44;";

                    case '-':
                        return "&#45;";
                    case '.':
                        return "&#46;";
                    case '/':
                        return "&#47;";

                    case ':':
                        return "&#58;";
                    case ';':
                        return "&#59;";

                    case '<':
                        return "&#60;";
                    case '=':
                        return "&#61;";
                    case '>':
                        return "&#62;";
                    case '?':
                        return "&#63;";
                    case '@':
                        return "&#64;";

                    case '[':
                        return "&#91;";
                    case '\\':
                        return "&#92;";
                    case ']':
                        return "&#93;";
                    case '^':
                        return "&#94;";

                    case '_':
                        return "&#95;";
                    case '`':
                        return "&#96;";

                    case '{':
                        return "&#123;";
                    case '|':
                        return "&#124;";
                    case '}':
                        return "&#125;";
                    case '~':
                        return "&#126;";
                }
            }
        }

        return txt;
    }

    std::string htmlescape( const std::string& txt )
    {
        return htmlescape( txt.c_str() );
    }
}
