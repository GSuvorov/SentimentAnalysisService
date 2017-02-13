using System;
using System.Text;

namespace DiskSearchEngine.Hashtable
{
    /// <summary>
    /// Helper for indexing public class
    /// </summary>
    public static class HashtableEngineHelper
    {
        public static uint CalulateAllRecordCountInDataFile( string dataFileFullName, Encoding dataFileEncoding )
        {
            if ( dataFileFullName.IsEmptyOrNull() )
                throw (new ArgumentNullException("dataFileFullname"));
            if ( dataFileEncoding == null )
                throw (new ArgumentNullException("dataFileEncoding"));


            uint dataRecordCount = 0;
            using ( var dataFileTextLineReader = new TextLineReader( dataFileFullName, dataFileEncoding ) )
            {
		        while ( !dataFileTextLineReader.EndOfStream )
                {
                    //[ReadLine4Searching()] used for speeding, no need here truth file-pointer position
			        if ( !dataFileTextLineReader.ReadLine4Searching().IsEmptyOrNull() )	
                        dataRecordCount++;                  
                }
            }
            return (dataRecordCount);
        }
        public static uint RoundBeforeGreaterBorder( this uint value )
        {
            var text = value.ToString();
            if ( text.Length == 1 )
                return (10);
            if ( text.TrimEnd( '0' ).Length == 1 )
                return (value);

            var i = text.Length;
            var f = value / Math.Pow( 10, i );

            return (Convert.ToUInt32( Convert.ToUInt32( f * 10 + 0.5  ) *  Math.Pow( 10, i - 1 ) ));

            #region [.commented.]
            /*var f = value / 10.0;
            var i = 0;
            do
            {
                f = f / 10.0;
                i++;
            }
            while( 1 < f );
            return (Convert.ToUInt32( Convert.ToUInt32( f * 10 + 0.5  ) *  Math.Pow( 10, i ) ));*/
            #endregion
        }
        public static uint NearestPrimeNumber( this uint num ) 
        {
            int cnt = (int)Math.Sqrt( num ) + 2;    // pi((sqrt(x)+1)^2) - pi(x) > 1 for all x > 4
            int pi_primes = 0;  
            int[] sieve = new int[ cnt ];//?! - 1 ];
            bool[] diff = new bool[ cnt ];
            bool is_prime = false;
            uint nearest = 0;

            //memset (sieve, 1, sizeof(int) * cnt-1);
            for (int i = 0; i < sieve.Length; i++ ) sieve[ i ] = 1;

            for (int i = 2; i < cnt; i++) 
            {
                if (sieve[ i ] != 0) {
                    for (int j = 2*i; j < cnt; j+=i) 
                        sieve[j] = 0;
                    sieve[pi_primes++] = i;
                }
            }

            while ( !is_prime )
            {
                /* Move to the next number of least difference*/
                num += nearest;
                nearest = 1; 
                is_prime = true;
                //memset (diff, false, sizeof(bool) * cnt);
                for (int i = 0; i < diff.Length; i++) diff [i ] = false;
                for (int i = 0; i < pi_primes; i++) {
                    if (num%sieve[i] == 0) is_prime = false;
                    else diff[sieve[i] - num%sieve[i]] = true;
                }

                if (is_prime) break;
                while (diff[nearest]) nearest++;
            }

            return num;
        }
        public static int  NearestPrimeNumber( this int num ) 
        {
            int cnt = (int)Math.Sqrt( num ) + 2;    // pi((sqrt(x)+1)^2) - pi(x) > 1 for all x > 4
            int pi_primes = 0;  
            int[] sieve = new int[ cnt ];//?! - 1 ];
            bool[] diff = new bool[ cnt ];
            bool is_prime = false;
            int nearest = 0;

            //memset (sieve, 1, sizeof(int) * cnt-1);
            for (int i = 0; i < sieve.Length; i++ ) sieve[ i ] = 1;

            for (int i = 2; i < cnt; i++) 
            {
                if (sieve[ i ] != 0) {
                    for (int j = 2*i; j < cnt; j+=i) 
                        sieve[j] = 0;
                    sieve[pi_primes++] = i;
                }
            }

            while ( !is_prime )
            {
                /* Move to the next number of least difference*/
                num += nearest;
                nearest = 1; 
                is_prime = true;
                //memset (diff, false, sizeof(bool) * cnt);
                for (int i = 0; i < diff.Length; i++) diff [i ] = false;
                for (int i = 0; i < pi_primes; i++) {
                    if (num%sieve[i] == 0) is_prime = false;
                    else diff[sieve[i] - num%sieve[i]] = true;
                }

                if (is_prime) break;
                while (diff[nearest]) nearest++;
            }

            return num;
        }
        /// <summary>
        /// Normlize text for indexing and search
        /// </summary>
        /// <param name="value">text for indexing or search</param>
        /// <returns>Normlized text</returns>
        /*public static string NormlizeText( string text )
        {
            return (NormlizeText( ref text ));
        }
        public static string HashtableEngineNormlizeText( this string text )
        {
            return (NormlizeText( ref text ));
        }*/
        public static string NormlizeText( string text )
        {
            //if ( string.IsNullOrEmpty( geoName ) )
            //    throw (new ArgumentNullException());

            //return (value.Trim('\r', '\n', '|').ToUpper());

            
            /*var n = from _ch in text
                    where ( char.IsLetterOrDigit( _ch ) )
                    select char.ToUpper( _ch );

            return (new string( n.ToArray() ));*/


            var sb = new StringBuilder( text.Length );
            foreach ( var ch in text )
            {
                if ( char.IsLetterOrDigit( ch ) )
                {
                    sb.Append( char.ToUpper( ch ) );
                }
            }
            return (sb.ToString());
        }
    }
}
