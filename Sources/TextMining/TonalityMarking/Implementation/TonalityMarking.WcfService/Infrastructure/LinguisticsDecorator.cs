using System;
using System.Xml.Linq;

using Lingvistics.Client;
using TextMining.Core;
using TextMining.Service;

namespace TonalityMarking
{
    internal static class LinguisticsDecorator
    {
        private static readonly ILingvisticsServer _LingvisticServer;

        static LinguisticsDecorator()
        {
            _LingvisticServer = LingvisticsServer.Create( Config.LinguisticsUrl, 10 );
        }

        public static XDocument ProcessText( InputParamsBase inputParams )
        {
            var lingvisticResult = _LingvisticServer.ProcessText( inputParams.InputText, false, DateTime.Now, LingvisticsResultOptions.RDF );
            if ( lingvisticResult == null || lingvisticResult.RDF.IsEmptyOrNull() )
                throw (new InvalidOperationException("LingvisticServer::ProcessText => [lingvisticResult == null || lingvisticResult.RDF.IsEmptyOrNull()]"));

            var xdoc = Algorithms.PreprocessRdf( lingvisticResult.RDF );

            return (xdoc);
        }
    }    
}