using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.ServiceModel;
using System.Text;
using System.Threading;
using System.Xml;

using OpinionMining;

namespace OpinionMining.ConsoleServiceHost
{
    class Program
    {
        static void Main( string[] args )
        {
            AppDomain.CurrentDomain.UnhandledException += new UnhandledExceptionEventHandler( CurrentDomain_UnhandledException );

            using ( var serviceHost = new ServiceHost( typeof(OpinionMiningWcfService) ) )
            {
                serviceHost.Open();

                Console.WriteLine( "\r\n\t[.....wait for input connection......]" );
                Console.ReadLine();
            }
        }

        static readonly object _SyncRoot = new object();
        static void CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            lock ( _SyncRoot )
            {
                var fc = Console.ForegroundColor; Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine( e.ExceptionObject.ToString() );
                Console.ForegroundColor = fc;
            }
        }
    }
}
