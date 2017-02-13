using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TextMining.Core
{
    /// <summary>
    /// 
    /// </summary>
    public static partial class Extensions
    {
        private static readonly HashSet< char > VOWELS_RU = 
                            new HashSet< char >( new[] { 'а', 'у', 'и', 'е', 'о', 'ы', 'я', 'й',
                                                         'А', 'У', 'И', 'Е', 'О', 'Ы', 'Я', 'Й' });
        private static readonly char[] SIGN_OF_PUNCTUATION = new[] { ',', ';', ':', '-', '.' };

        public static bool IsNull( this object obj )
        {
            return (obj == null);
        }
        public static bool IsNotNull( this object obj )
        {
            return (obj != null);
        }
        public static void ThrowIfEmptyOrNull( this string text, string argName )
        {
            if ( text.IsEmptyOrNull())
                throw (new ArgumentNullException( argName ));
        }
        public static void ThrowIfNull( this object obj, string argName )
        {
            if ( obj == null )
                throw (new ArgumentNullException( argName ));
        }
        public static void ThrowIfZeroOrZeroLess( this decimal obj, string argName )
        {
            if ( obj <= 0 )
                throw (new ArgumentNullException( argName ));
        }
        
        public static List< string > CheckInquiriesSynonymsAndPrepare( this List< string > inquiriesSynonymList )
        {
            if ( inquiriesSynonymList == null )
                return (null);

            if ( inquiriesSynonymList.Any( synonym => synonym == null ) )
                throw (new ArgumentNullException("One of synonym is null"));

            var inquiriesSynonyms = (from synonym in inquiriesSynonymList
                                     select synonym
                                            .ToLowerEx()
                                            .TrimWhiteSpaces()                                            
                                    ).ToList();
            if ( inquiriesSynonyms.Any( synonym => synonym.IsEmptyOrNull() ) )
                throw (new ArgumentNullException("One of synonym is empty"));

            return (inquiriesSynonyms);
        }

        public static bool IsEmptyOrNull( this string text )
        {
            return (string.IsNullOrEmpty( text ));    
        }
        public static string InSingleQuote( this string text )
        {
            return ('\'' + text + '\'');
        }
        public static string InBrackets( this string text )
        {
            return ('(' + text + ')');
        }
        public static string FormatEx( this string formatText, params object[] args )
        {
            return (string.Format( formatText, args ));
        }
        public static string[] SplitBySpace( this string value )
        {
            return (value.Split( new[] { ' ' }, StringSplitOptions.RemoveEmptyEntries ));
        }
        public static string[] SplitByTabulation(this string value)
        {
            return (value.Split(new[] { '\t' }, StringSplitOptions.RemoveEmptyEntries));
        }
        public static string[] SplitByComma( this string value )
        {
            return (value.Split( new[] { ',' }, StringSplitOptions.RemoveEmptyEntries ));
        }
        public static string[] SplitBy( this string value, char ch )
        {
            return (value.Split( new[] { ch }, StringSplitOptions.RemoveEmptyEntries ));
        }
        public static string ToLowerEx( this string value )
        {
            return ((value != null) ? value.ToLower() : value);
        }
        public static string TrimWhiteSpaces( this string value )
        {
            return ((value != null) ? value.Trim( ' ', '\r', '\n', '\t' ) : value);
        }
        private static string ReplaceDoubleQuoteOnSingleQuote( this string value )
        {
            return ((value != null) ? value.Replace('"', '\'') : value);
        }
        public static string ReplaceSingleQuote( this string value )
        {
            return ((value != null) ? value.Replace("'", string.Empty) : value);
        }
        public static string JoinBySpace( this IEnumerable< string > values )
        {
            return (values.JoinBy( " " ));
            //return (string.Join(" ", values.Select( _ => _.TrimWhiteSpaces() ).ToArray() ));
        }
        public static string JoinBy( this IEnumerable< string > values, string separator )
        {
            return (string.Join( separator, values.Select( _ => _.TrimWhiteSpaces() ).ToArray() ));
        }

        public static IEnumerable< TResult > RepeatOnce< TResult >( this TResult element )
        {
            return (Enumerable.Repeat( element, 1 ));
        }

        public static T ToEnum< T >( this string value )
        {
            return ((T) Enum.Parse( typeof( T ), value, true ));
        }
        public static T? TryConvert2Enum< T >( this string value ) where T : struct
        {
            var v = default(T);
            if ( Enum.TryParse< T >( value, true, out v ) )
                return (v);
            return (null);
        }
        public static int? TryConvert2Int( this string value )
        {
            int v;
            if ( int.TryParse( value, out v ) )
                return (v);
            return (null);
        }
        public static bool? TryConvert2Bool( this string value )
        {
            bool v;
            if ( bool.TryParse( value, out v ) )
                return (v);
            return (null);
        }

        public static string ReplaceEnd2Vowels( this string value )
        {
            value = value.Trim();

            if ( (2 < value.Length) && value[ value.Length - 1 ].IsVowelRu() )
            {
                var i = value[ value.Length - 2 ].IsVowelRu() ? 2 : 1;

                value = value.Remove( value.Length - i ) + '*';
            }

            return (value);
        }
        private static bool IsVowelRu( this char ch )
        {
            return (VOWELS_RU.Contains( ch ));
        }

        public static bool ContainsSignOfPunctuation( this string value )
        {
            return (value.IndexOfAny( SIGN_OF_PUNCTUATION ) != -1);
        }
        public static bool ContainsLetters( this string value )
        {
            for ( int i = 0; i < value.Length; i++ )
            {
                if ( char.IsLetter( value, i ) )
                    return (true);
            }
            return (false);
        }
        public static string TrimEndSignOfPunctuation(this string value)
        {
            return (value.TrimEnd( SIGN_OF_PUNCTUATION ));
        }

        #region [.commented. SearchForSubsequence -> IEnumerable<>.]
        /*
        private static int IndexOf( this IEnumerable< string > sequence, int statrIndex, ref string value )
        {
            foreach ( var _ in sequence.Skip( statrIndex ) )
            {
                if ( _ == value )
                    return (statrIndex);
                statrIndex++;
            }
            return (-1);
        }
        public static bool SearchForSubsequence( this IEnumerable< string > sequence, IEnumerable< string > subsequence )
        {
            var startValue       = subsequence.ElementAt( 0 );
            int sequenceCount    = sequence   .Count();
            int subsequenceCount = subsequence.Count();
            for ( int statrIndex = -1; statrIndex < sequenceCount; )
            {
                statrIndex = sequence.IndexOf( statrIndex + 1, ref startValue );
                if ( statrIndex == -1 )
                    break;
                
                var bingo = sequence.Skip( statrIndex ).Take( subsequenceCount ).SequenceEqual( subsequence );
                if (bingo)
                    return (true);
            }
            return (false);
        }
        */        
        #endregion
        private static int IndexOf( this string[] sequence, int statrIndex, string value )
        {
            for ( int len = sequence.Length; ; )
            {
                if ( value == sequence[ statrIndex ] )
                    return (statrIndex);

                statrIndex++;
                if (len <= statrIndex)
                    break;
            }
            return (-1);
        }
        public static bool SearchForSubsequence( this string[] sequence, string[] subsequence )
        {
            int sequenceLen = sequence.Length;
            if (sequenceLen == 0)
                return (false);

            int subsequenceLen = subsequence.Length;
            var startValue = subsequence[ 0 ];
            for ( int startIndex = -1; startIndex < sequenceLen; )
            {
                startIndex = sequence.IndexOf( startIndex + 1, startValue );
                if (startIndex == -1)
                    break;


                if (sequenceLen < startIndex + subsequenceLen)
                    break;

                var bingo = true;
                for ( int i = 0; i < subsequenceLen; i++ )
                {
                    if ( sequence[ i + startIndex ] != subsequence[ i ] )
                    {
                        bingo = false;
                        break;
                    }
                }
                if (bingo)
                    return (true);
            }
            return (false);
        }

        public static bool IsEmptyOrNull< T >( this List< T > sequence )
        {
            return (sequence.IsNull() || !sequence.Any());
        }
        public static bool IsEmptyOrNull< T >( this HashSet< T > sequence )
        {
            return (sequence.IsNull() || !sequence.Any());
        }

        public static bool ContainsAny< T >( this HashSet< T > hs, IEnumerable< T > seq )
        {
            foreach ( var it in seq )
            {
                if ( hs.Contains( it ) )
                    return  (true);
            }
            return (false);
        }
    }
}
