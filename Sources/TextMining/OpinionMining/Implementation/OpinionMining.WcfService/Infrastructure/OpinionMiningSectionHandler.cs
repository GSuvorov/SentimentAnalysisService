using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Xml;

using TextMining.Core;

namespace OpinionMining
{
    internal class OpinionMiningSectionHandler : IConfigurationSectionHandler
    {
    #region [.IConfigurationSectionHandler.]
        object IConfigurationSectionHandler.Create( object parent, object configContext, XmlNode section )
        {
            if ( object.Equals( section, null ) )
                throw (new NullReferenceException("Configuration error: 'OpinionMining' section is not found"));

            return (section.ToXDocument());
        }
    #endregion
    }
}