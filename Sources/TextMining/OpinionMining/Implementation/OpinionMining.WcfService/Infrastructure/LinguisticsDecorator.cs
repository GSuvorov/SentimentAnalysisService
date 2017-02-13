using System;
using System.Xml.Linq;

using Lingvistics.Client;
using TextMining.Core;
using TextMining.Service;

namespace OpinionMining
{
    internal static class LinguisticsDecorator
    {
        private static readonly ILingvisticsServer _LingvisticServer;

        static LinguisticsDecorator()
        {
            _LingvisticServer = LingvisticsServer.Create( Config.LinguisticsUrl );
        }

        public static XDocument ProcessText( InputParamsBase inputParams )
        {           
            var lingvisticResult = _LingvisticServer.ProcessText( inputParams.InputText, false, DateTime.Now, LingvisticsResultOptions.RDF );

            System.Diagnostics.Debug.Assert( lingvisticResult != null );
            System.Diagnostics.Debug.Assert( !string.IsNullOrEmpty( lingvisticResult.RDF ) );

            var xdoc = Algorithms.PreprocessRdf( lingvisticResult.RDF );

            return (xdoc);
        }
    }     
}