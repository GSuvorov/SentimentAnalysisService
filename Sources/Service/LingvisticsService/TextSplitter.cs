using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace Lingvistics
{
    /// <summary>
    /// 
    /// </summary>
    public static class TextSplitter
    {
        private static Regex rxSentenceEnd = new Regex( @"\w{5,}[\.!\?]+", RegexOptions.Compiled | RegexOptions.Singleline | RegexOptions.RightToLeft );

        public static string[] Split( string text, int maxLength )
        {
            var rc = new List< string >();
            int minBlockLength = maxLength / 3;
            int start = 0;
            while ( start < text.Length )
            {
                //конец текста
                if ( start + maxLength >= text.Length )
                {
                    rc.Add( text.Substring( start ) );
                    break;
                }
                //найден конец предложения и блок не очень маленький
                var m = rxSentenceEnd.Match( text, start, maxLength );
                if ( m.Success && m.Index + m.Length - start > minBlockLength )
                {
                    int end = m.Index + m.Length;
                    rc.Add( text.Substring( start, end - start ) );
                    start = end;
                }
                //слишком маленький блок или не найден разделитель
                else
                {
                    rc.Add( text.Substring( start, maxLength ) );
                    start += maxLength;
                }
            }
            return rc.ToArray();
        }
    }
}
