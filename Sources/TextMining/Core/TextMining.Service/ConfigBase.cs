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

namespace TextMining.Service
{
    public abstract class ConfigBase
    {
        public const string DIGEST_SECTION_NAME                = "Digest";
        public const string OPINION_MINING_SECTION_NAME        = "OpinionMining";
        public const string TONALITY_MARKING_SECTION_NAME      = "TonalityMarking";

        protected const string LINGUISTICS_XPATH               = "Linguistics";        
        protected const string LINGUISTICS_URL_XPATH           = LINGUISTICS_XPATH + "/Url";
        #region commented. CompactOutputXmlFormatConverter
        //protected const string COMPACTOUTPUTXMLFORMAT_XPATH      = "CompactOutputXmlFormat";
        //protected const string COMPACTOUTPUTXMLFORMAT_XSLT_XPATH = COMPACTOUTPUTXMLFORMAT_XPATH + "/Xslt";
        //protected const string XSLT_FILE                         = "XsltFile";
        #endregion
        protected const string BASE_PATH                       = "BasePath";
        protected const string CURRENT_NODE_XPATH              = ".";
        protected const string APP_DATA                        = "~/App_Data";

        private readonly object _SyncRoot = new object();
        public object SyncRoot
        {
            get { return (_SyncRoot); }
        }

        private XDocument _ConfigSection;
        public XDocument ConfigSection
        {
            get
            {
                if ( _ConfigSection == null )
                {
                    lock ( _SyncRoot )
                    {
                        if ( _ConfigSection == null )
                        {
                            _ConfigSection = (XDocument) ConfigurationManager.GetSection( ConfigSectionName );
                        }
                    }
                }
                return (_ConfigSection);
            }
        }
        public void SetConfigSection( XDocument configSection )
        {
            configSection.ThrowIfNull("section");
            lock ( _SyncRoot )
            {
                _ConfigSection = configSection;
            }
        }

        public abstract string ConfigSectionName
        {
            get;
        }

        public static string AppDataPath
        {
            get { return (HostingEnvironment.MapPath( APP_DATA ) ?? Environment.CurrentDirectory); }
        }

        public string LinguisticsUrl
        {
            get 
            {
                var url = ConfigSection.Root.GetSingleNode( LINGUISTICS_URL_XPATH ).Value.Trim();
                if ( url.IsEmptyOrNull() )
                    throw (new ConfigException( LINGUISTICS_URL_XPATH.InSingleQuote() + " section is empty"));

                return (url);
            }
        }
        //---------------------------------------------------------------------------//
        #region commented. CompactOutputXmlFormatConverter
        /*
        private CompactOutputXmlFormatConverter _CompactOutputXmlFormatConverter;
        public CompactOutputXmlFormatConverter CompactOutputXmlFormatConverter
        {
            get
            {
                if ( _CompactOutputXmlFormatConverter == null )
                {
                    lock ( _SyncRoot )
                    {
                        if ( _CompactOutputXmlFormatConverter == null )
                        {
                            _CompactOutputXmlFormatConverter = CreateCompactOutputXmlFormatConverter( ConfigSection.Root );
                        }
                    }
                }
                return (_CompactOutputXmlFormatConverter);
            }
        }
        private static CompactOutputXmlFormatConverter CreateCompactOutputXmlFormatConverter( XElement configSectionNode )
        {
            configSectionNode.ThrowIfNull("configSectionNode");

            var xsltNode = configSectionNode.GetSingleNode( COMPACTOUTPUTXMLFORMAT_XSLT_XPATH );

            var basePathNode = xsltNode.Attribute( BASE_PATH );
            var basePath     = (basePathNode != null) ? basePathNode.Value : null;

            var xsltFiles = from xsltFileNode in xsltNode.GetMultiNodes( XSLT_FILE )
                            let xsltFile = GetFilename( xsltFileNode, CURRENT_NODE_XPATH, basePath )
                            select xsltFile;

            return (new CompactOutputXmlFormatConverter( xsltFiles ));
        }
        */
        #endregion
        //---------------------------------------------------------------------------//

        public static string GetFilename( XElement node, string fileNameXpath, string basePath )
        {
            var fileName = fileNameXpath.StartsWith( "@" )
                         ? GetAttributeNode( node, fileNameXpath ).Value
                         : node.GetSingleNode( fileNameXpath ).Value;      

            if ( !basePath.IsEmptyOrNull() )
            {
                fileName = Path.Combine( basePath, fileName );
            }
            if ( !File.Exists( fileName ) )
            {
                fileName = Path.Combine( AppDataPath, fileName );
                if ( !File.Exists( fileName ) )
                    throw (new ConfigException( fileName.InSingleQuote() + " file is not found"));
            }
            return (fileName);
        }       

        public static XAttribute GetAttributeNode( XElement element, string attributeName )
        {
            var a = element.Attribute( attributeName.StartsWith("@") ? attributeName.Substring( 1 ) : attributeName );
            if ( a == null )
                throw (new InvalidOperationException( (attributeName.StartsWith("@") ? attributeName.Substring( 1 ) : attributeName).InSingleQuote() + " is not found in " + element.FullXPath().InSingleQuote()) );

            return (a);
        }
    }
}