using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Web.Hosting;
using System.Xml;
using System.Xml.Linq;
using System.Xml.XPath;

using TextMining.Core;
using TextMining.Service;
using TonalityMarking;
using OpinionMining;

namespace Digest
{
    /// <summary>
    /// 
    /// </summary>
    internal sealed class ConfigImpl : ConfigBase
    {
        public override string ConfigSectionName
        {
            get { return (ConfigBase.DIGEST_SECTION_NAME); }
        }

        public XElement GetLinguisticsNode()
        {
            return (GetXElementByXPath( ConfigBase.LINGUISTICS_XPATH  ));
        }
        #region commented. CompactOutputXmlFormatConverter
        /*
        public XElement GetCompactOutputXmlFormatNode()
        {
            return (GetXElementByXPath( ConfigBase.COMPACTOUTPUTXMLFORMAT_XPATH  )); 
        }
        */
        #endregion

        public XElement GetOpinionMiningSection()
        {
            return (GetXElementByXPath( ConfigBase.OPINION_MINING_SECTION_NAME ));
        }
        public XElement GetTonalityMarkingSection()
        {
            return (GetXElementByXPath( ConfigBase.TONALITY_MARKING_SECTION_NAME ));
        }

        private XElement GetXElementByXPath( string url )
        {
            var linguistics  = ConfigSection.Root.GetSingleNode( url );

            if ( linguistics.IsNull() )
            {
                throw (new ConfigException( url.InSingleQuote() + " section is empty" ));
            }

            return (linguistics);
        }   
    }

    /// <summary>
    /// 
    /// </summary>
    internal static class Config
    {
        private static readonly ConfigImpl _ConfigImpl = new ConfigImpl();

        #region commented. CompactOutputXmlFormatConverter
        /*
        internal static CompactOutputXmlFormatConverter CompactOutputXmlFormatConverter
        {
            get { return (_ConfigImpl.CompactOutputXmlFormatConverter); }
        }
        */
        #endregion
        //---------------------------------------------------------------------------//

        internal static void ExecuteConfigure()
        {
            lock ( _ConfigImpl.SyncRoot )
            {
                //.Get Digest config-section
                var linguistics = _ConfigImpl.GetLinguisticsNode();
                //---var compactOutputXmlFormat = _ConfigImpl.GetCompactOutputXmlFormatNode();

                //.Set Opinion-mining config-section
                var opinionMiningSection = _ConfigImpl.GetOpinionMiningSection();
                opinionMiningSection.Add( linguistics );
                //---opinionMiningSection.Add( compactOutputXmlFormat );
                OpinionMiningWcfService.SetConfigSection( opinionMiningSection.ToXDocument() );

                //.Set Tonality-marking config-section
                var tonalityMarkingSection = _ConfigImpl.GetTonalityMarkingSection();
                tonalityMarkingSection.Add( linguistics );
                //---tonalityMarkingSection.Add( compactOutputXmlFormat );
                TonalityMarkingWcfService.SetConfigSection( tonalityMarkingSection.ToXDocument() );
            }
        }
    }
}