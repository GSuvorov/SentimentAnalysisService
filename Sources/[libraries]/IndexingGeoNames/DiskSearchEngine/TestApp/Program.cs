using System;
using System.Collections.Generic;
using System.Configuration;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;

using DiskSearchEngine.Hashtable;

namespace TestApp
{
    /// <summary>
    /// 
    /// </summary>
    internal static class Program
    {
        #region [.geo_names.]
        private static readonly string[] geo_names = new[]
            {
                "москва",
                "хуево-кукуево",
                "кукуево",
                "кызыл-орда",
                "кызылорда",
                "бухловка",
                "козловка",
                "урюпинск",
                "moscow",
                "петербург",
                "санкт петербург",
                "с. петербург",
                "с.петербург",
                "с петербург",
            };
        #endregion

        #region [.config class.]
        private static class config
        {
            public static string dataFileFullName
            {
                get { return (ConfigurationManager.AppSettings[ "dataFileFullName" ]); }
            }
            public static Encoding dataFileEncoding
            {
                get { return (Encoding.GetEncoding( ConfigurationManager.AppSettings[ "dataFileEncoding" ] )); }
            }
            public static bool test_GetIndexHeaderByDataFile
            {
                get { return (bool.Parse(ConfigurationManager.AppSettings["test_GetIndexHeaderByDataFile"])); }
            }
            public static bool test_Search_small
            {
                get { return (bool.Parse(ConfigurationManager.AppSettings["test_Search_small"])); }
            }
            public static bool test_Search_big
            {
                get { return (bool.Parse(ConfigurationManager.AppSettings["test_Search_big"])); }
            }
            public static bool test_BuildIndex32OnDisk
            {
                get { return (bool.Parse(ConfigurationManager.AppSettings["test_BuildIndex32OnDisk"])); }
            }
            public static bool test_BuildIndex64
            {
                get { return (bool.Parse(ConfigurationManager.AppSettings["test_BuildIndex64"])); }
            }
            public static bool test_BuildIndex32
            {
                get { return (bool.Parse(ConfigurationManager.AppSettings["test_BuildIndex32"])); }
            }
            public static bool test_BuildIndexAutomatic
            {
                get { return (bool.Parse(ConfigurationManager.AppSettings["test_BuildIndexAutomatic"])); }
            }
        }
        #endregion

        static void create_datafile_in_need_encoding( string dataFileFullName, Encoding dataFileEncoding ) //, Encoding newDataFileEncoding )
        {
            using ( var sw = new StreamWriter( dataFileFullName, false, dataFileEncoding ) )
            {
                //sw.Write( new string('w', 1023) + "ф" + '!' + '\n' );
                sw.Write( "XY\r\n" );
                sw.Write( "[москва]".Circulate( 200 )+ '!' + '\n' );
                sw.Write( "[санкт петербург]".Circulate( 100 ) + '!'  + Environment.NewLine );

                sw.Write( "москва" + '\n' );
                sw.Write( "кукуево" + "\r\n" );
                sw.Write( "санкт петербург" + Environment.NewLine );
                sw.Write( "moscow" + Environment.NewLine + '\r' + "\r\n" + '\n' + '\n' );
                sw.Write( "урюпинск" + "\r\n" );
                sw.Write( "new " + '\r' );
                sw.Write( "york" + "\r\n" );
                sw.Write( "new york" + "\r\n" );
                sw.Write( "с. петербург" + "\r\n" );
            }
        }        
        static void toUtf8( string dataFileFullName, Encoding dataFileEncoding )
        {
            using ( var sr = new StreamReader( @"E:\GeoDB\data\GeoNames.data", dataFileEncoding ) )
            using ( var sw = new StreamWriter( @"E:\GeoDB\data\GeoNames.utf8.data", false, Encoding.UTF8 ) )
            {
                while ( !sr.EndOfStream )
                    sw.Write( sr.ReadLine() + '\n' );
            }
        }
        static void test_2()
        {
            var dataFileFullName = @"E:\GeoDB\DiskSearchEngine\data\fusking.rotter.data";
            var dataFileEncoding = Encoding.UTF7; //Encoding.Unicode; //Encoding.UTF8; //

            create_datafile_in_need_encoding( dataFileFullName, dataFileEncoding );


            //var indexHeader = HashtableIndexer.GetIndexHeaderByDataFile( dataFileFullName );

            var allRecordCountInDataFile = HashtableEngineHelper.CalulateAllRecordCountInDataFile( dataFileFullName, dataFileEncoding );
            HashtableIndexer.BuildIndex( dataFileFullName, dataFileEncoding, allRecordCountInDataFile.NearestPrimeNumber(), HashtableEngineHelper.NormlizeText );

            using ( var searcher = new HashtableSearcher( dataFileFullName, HashtableEngineHelper.NormlizeText ) )
            {               
                var max_len = geo_names.Max( _ => _.Length );

                searcher.IsExists1( "кызыл-орда", max_len );
                foreach ( var geo_name in geo_names )
                {
                    searcher.IsExists1( geo_name, max_len );
                }
            }
        }


        static void test_GetIndexHeaderByDataFile( string dataFileFullName )
        {
            //test get index info
            try
            {
                var indexHeader = HashtableIndexer.GetIndexHeaderByDataFile( dataFileFullName );

                Console.WriteLine( indexHeader.ToString() );
            }
            catch ( Exception e )
            {
                Console.WriteLine("HashtableIndexer.GetIndexHeaderByDataFile( '" + dataFileFullName  + "') => " + e.Message + Environment.NewLine );
            }
        }
        static void test_Search_small( string dataFileFullName )
        {
            using ( var searcher = new HashtableSearcher( dataFileFullName, HashtableEngineHelper.NormlizeText ) )
            {               
                var max_len = geo_names.Max( _ => _.Length );

                searcher.IsExists1( "кызыл-орда", max_len );
                foreach ( var geo_name in geo_names )
                {
                    searcher.IsExists1( geo_name, max_len );
                }
            }
        }
        static void test_Search_big  ( string dataFileFullName, Encoding dataFileEncoding )
        {
            Console.WriteLine("Begin fetch test data for search...");
            var testList = new List< string >( 4000000 );
            using ( var sr = new StreamReader( dataFileFullName, dataFileEncoding ) )
            {
                while ( !sr.EndOfStream )
                {
                    var text = sr.ReadLine();
                    if ( !string.IsNullOrEmpty( text ) )
                        testList.Add( text );
                }
            }
            Console.WriteLine("End fetch test data, all count: " + testList.Count + '.' + Environment.NewLine);


            Console.WriteLine("Begin full search...");
            using ( var searcher = new HashtableSearcher( dataFileFullName, HashtableEngineHelper.NormlizeText ) )
            {
                var sw = Stopwatch.StartNew();
                var i  = 0;
                foreach ( var text in testList )
                {
                    //searcher.IsExists2( text );

                    i++;
                    var exists = searcher.IsExists( text );
                    if ( !exists )
                    {
                        Extensions.Write2Console( '\'' + text + "' => searcher.IsExists: " + exists + ", row number: " + i, ConsoleColor.Yellow );
                    }
                }
                sw.Stop();
                Console.WriteLine("End full search, elapsed: " + sw.Elapsed + ", all count: " + testList.Count + '.' + Environment.NewLine);
            }

            #region [.commented. disk read.]
            /*Console.WriteLine("Begin full search...");
            using ( var searcher = new HashtableSearcher( dataFileFullName ) )
            {
                var sw = Stopwatch.StartNew();
                var i = 0;
                using ( var sr = new StreamReader( dataFileFullName, dataFileEncoding ) )
                {
                    while ( !sr.EndOfStream )
                    {
                        var geo_name = sr.ReadLine();
                        if ( !string.IsNullOrEmpty( geo_name ) )
                        {
                            searcher.IsExistsEx( geo_name );
                            i++;
                        }
                    }
                }
                sw.Stop();

                Console.WriteLine( "End full search, elapsed: " + sw.Elapsed + ", all count: " + i );
            }*/
            #endregion
        }
        static void test_BuildIndex64( string dataFileFullName, Encoding dataFileEncoding )
        {
            Console.WriteLine( "Begin calulate record count in data file..." );
            var allRecordCountInDataFile = HashtableEngineHelper.CalulateAllRecordCountInDataFile( dataFileFullName, dataFileEncoding );
            Console.WriteLine( "End calulate record count in data file: " + allRecordCountInDataFile + Environment.NewLine );


            //test Indexing
            Console.WriteLine( "Begin build int64-index...");
            var sw = Stopwatch.StartNew();

            HashtableIndexer.BuildIndex( dataFileFullName, dataFileEncoding, allRecordCountInDataFile
                //.RoundBeforeGreaterBorder()
                .NearestPrimeNumber(), HashtableEngineHelper.NormlizeText );
            //HashtableIndexer.BuildIndex( dataFileFullName, dataFileEncoding, 3000000 );

            Console.WriteLine( "End build int64-index, elapsed: " + sw.Elapsed + Environment.NewLine );
        }
        static void test_BuildIndex32( string dataFileFullName, Encoding dataFileEncoding )
        {
            Console.WriteLine( "Begin calulate record count in data file..." );
            var allRecordCountInDataFile = HashtableEngineHelper.CalulateAllRecordCountInDataFile( dataFileFullName, dataFileEncoding );
            Console.WriteLine( "End calulate record count in data file: " + allRecordCountInDataFile + Environment.NewLine );


           //test Int32-Indexing
            Console.WriteLine( "Begin build int32-index...");
            Console.WriteLine( "Hash-table size: " + allRecordCountInDataFile
                //.RoundBeforeGreaterBorder()
                .NearestPrimeNumber() );
            var sw = Stopwatch.StartNew();
            
            HashtableIndexer.BuildIndexInt32( dataFileFullName, dataFileEncoding, allRecordCountInDataFile
                //.RoundBeforeGreaterBorder()
                .NearestPrimeNumber(), HashtableEngineHelper.NormlizeText );
            //HashtableIndexer.BuildIndexInt32( dataFileFullName, dataFileEncoding, 3000000 );

            Console.WriteLine( "End build int32-index, elapsed: " + sw.Elapsed + Environment.NewLine );
        }
        static void test_BuildIndexAutomatic( string dataFileFullName, Encoding dataFileEncoding )
        {
            /*
            Console.WriteLine( "Begin calulate record count in data file..." );
            var allRecordCountInDataFile = HashtableEngineHelper.CalulateAllRecordCountInDataFile( dataFileFullName, dataFileEncoding );
            Console.WriteLine( "End calulate record count in data file: " + allRecordCountInDataFile + Environment.NewLine );

            //test automatic-Indexing
            Console.WriteLine( "Begin build automatic-index...");
            Console.WriteLine( "Hash-table size: " + allRecordCountInDataFile.NearestPrimeNumber() );
            var sw = Stopwatch.StartNew();
            
            HashtableIndexer.BuildIndexAutomatic( dataFileFullName, dataFileEncoding, allRecordCountInDataFile.NearestPrimeNumber() );

            Console.WriteLine( "End build automatic-index, elapsed: " + sw.Elapsed + Environment.NewLine );
            */

            Console.WriteLine( "Begin build automatic-index...");
            var sw = Stopwatch.StartNew();
            HashtableIndexer.BuildIndexAutomatic( dataFileFullName, dataFileEncoding, HashtableEngineHelper.NormlizeText );

            Console.WriteLine( "End build automatic-index, elapsed: " + sw.Elapsed + Environment.NewLine );
        }
        static void test_BuildIndex32OnDisk( string dataFileFullName, Encoding dataFileEncoding )
        {
            Console.WriteLine( "Begin calulate record count in data file..." );
            var allRecordCountInDataFile = HashtableEngineHelper.CalulateAllRecordCountInDataFile( dataFileFullName, dataFileEncoding );
            Console.WriteLine( "End calulate record count in data file: " + allRecordCountInDataFile + Environment.NewLine );


            //test Indexing
            Console.WriteLine( "Begin build int32-index on disk...");
            var sw = Stopwatch.StartNew();

            HashtableIndexer.BuildIndexInt32OnDisk( dataFileFullName, dataFileEncoding, allRecordCountInDataFile.NearestPrimeNumber(), HashtableEngineHelper.NormlizeText );

            Console.WriteLine( "End build int32-index on disk, elapsed: " + sw.Elapsed + Environment.NewLine );
        }

        static void Main( string[] args )
        {
            AppDomain.CurrentDomain.UnhandledException += (sender, e) =>
                Extensions.Write2Console( "AppDomain::UnhandledException => " + e.ExceptionObject.ToString(), ConsoleColor.Red );

            //toUtf8( "XZ", Encoding.Unicode );

            //=====================//
            Console.WriteLine( "Data file:          '" + config.dataFileFullName + '\'' );
            Console.WriteLine( "Data file encoding: '" + config.dataFileEncoding.BodyName + '\'' + Environment.NewLine );


            if ( config.test_BuildIndex32OnDisk )
                test_BuildIndex32OnDisk( config.dataFileFullName, config.dataFileEncoding );


            if ( config.test_BuildIndex64 )
                test_BuildIndex64( config.dataFileFullName, config.dataFileEncoding );

            if ( config.test_BuildIndex32 )
                test_BuildIndex32( config.dataFileFullName, config.dataFileEncoding );

            if ( config.test_BuildIndexAutomatic )
                test_BuildIndexAutomatic( config.dataFileFullName, config.dataFileEncoding );

            if ( config.test_GetIndexHeaderByDataFile )
                test_GetIndexHeaderByDataFile( config.dataFileFullName );

            if ( config.test_Search_small )
                test_Search_small( config.dataFileFullName );

            if ( config.test_Search_big )
                test_Search_big( config.dataFileFullName, config.dataFileEncoding );

            //=====================//
            Console.WriteLine("\r\n\r\n\r\n\t{.....push fusking button.finita.fusking rotter....}");
            Console.ReadLine();
        }
    }

    /// <summary>
    /// 
    /// </summary>
    internal static class Extensions
    {
        public static void IsExists1( this HashtableSearcher searcher, string geoName, int max_len )
        {
            try
            {
                var exists = searcher.IsExists( geoName );

                Write2Console( '\'' + geoName + '\'' + new string(' ', Math.Max(max_len, geoName.Length) - geoName.Length) + " => HashtableSearcher.IsExists: " + exists,
                    (exists) ? ConsoleColor.White : ConsoleColor.Yellow );
            }
            catch (Exception e)
            {
                Write2Console( '\'' + geoName + '\'' + new string(' ', Math.Max(max_len, geoName.Length) - geoName.Length) + " => " + e.ToString(), ConsoleColor.Red );
            }
        }
        public static void IsExists2( this HashtableSearcher searcher, string geoName )
        {
            //try
            //{
                var exists = searcher.IsExists( geoName/*.HashtableEngineNormlizeText()*/ );
                if ( !exists )
                {
                    Write2Console( '\'' + geoName + "' => searcher.IsExists: " + exists, ConsoleColor.Yellow );
                }
            //}
            //catch (Exception e)
            //{
            //    Write2Console( '\'' + geoName + "' => " + e.ToString(), ConsoleColor.Red );
            //}
        }

        public static void Write2Console( string message, ConsoleColor consoleColor )
        {
            var _ = Console.ForegroundColor; Console.ForegroundColor = consoleColor;
            Console.WriteLine( message );
            Console.ForegroundColor = _;
        }

        public static string Circulate( this string value, int count )
        {
            StringBuilder sb = new StringBuilder();

            for ( int i = 0; i < count; i++ )
                sb.Append( value );

            return (sb.ToString());
        }
    }
}
