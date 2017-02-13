using System;
using System.Configuration;
using System.Xml;

using TextMining.Core;

namespace Digest
{
    /// <summary>
    /// 
    /// </summary>
    internal class DigestSectionHandler : IConfigurationSectionHandler
    {
        #region [.IConfigurationSectionHandler.]
        object IConfigurationSectionHandler.Create( object parent, object configContext, XmlNode section )
        {
            if ( object.Equals( section, null ) )
            {
                throw (new NullReferenceException( "Configuration error: 'Digest' section is not found" ));
            }

            return (section.ToXDocument());
        }
        #endregion
    }
}