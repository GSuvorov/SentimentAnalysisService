using System;
using System.Linq;
using System.ServiceProcess;
using System.Text.RegularExpressions;

namespace Lingvistics
{
    /// <summary>
    /// 
    /// </summary>
    internal static class Program
	{
		/// <summary>
		/// 
		/// </summary>
		private static void Main()
		{
            var rgx = new Regex( @"(?<P>[^\s\""]+)|(\""(?<P>.*?)\"")", RegexOptions.Singleline );
            string[] argv = rgx.Matches( Environment.CommandLine )
                               .Cast< Match >()
                               .Select( m => m.Groups[ "P" ].Value )
                               .ToArray();

            if ( (1 < argv.Length) && (string.Compare( argv[ 1 ], "console", true ) == 0) )
			{
				var lingvisticServer = new LingvisticsServer();
				lingvisticServer.Start();
                //Thread.Sleep( Timeout.Infinite );
                Console.WriteLine("[......push Enter for exit......]");
                Console.ReadLine();
                lingvisticServer.Stop();
                Console.WriteLine( "[......exit in progress......]" );
			}
			else
			{
				var servicesToRun = new[] { new LingvisticsServer() };
                ServiceBase.Run( servicesToRun );
			}
		}
	}
}
