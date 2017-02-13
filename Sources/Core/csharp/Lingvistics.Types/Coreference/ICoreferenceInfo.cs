using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Linguistics.Coreference
{
	/// <summary>
    /// connected with coreference resolution
	/// </summary>
    public interface ICoreferenceInfo
	{
        /// <summary>
        /// try getting master coreference-synset name & XElement
        /// </summary>
        Tuple< string, XElement > TryGetMaster( XElement entity );

        /// <summary>
        /// try getting master coreference-synset name
        /// </summary>
        string TryGetMasterName( XElement entity );
	}
}
