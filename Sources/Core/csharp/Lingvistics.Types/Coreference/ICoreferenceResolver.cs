using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;

namespace Linguistics.Coreference
{
    /// <summary>
    /// 
    /// </summary>
    [Flags]
    public enum CoreferenceGroupType
    {
        All = 0x0,

        PersonNameGroup   = 0x1,
        PersonInfoGroup   = 1 << 1,
        PersonNoInfoGroup = 1 << 2,
        OrgGroup          = 1 << 3,
        PlaceGroup        = 1 << 4,
        ProductGroup      = 1 << 5,
        EventGroup        = 1 << 6,
        OrgAbbrGroup      = 1 << 7,
    }

	/// <summary>
    /// connected with coreference resolution
	/// </summary>
    public interface ICoreferenceResolver
    {
        ICoreferenceInfo Process( string   rdf, int rdfPartNumber = 0, CoreferenceGroupType groupType = CoreferenceGroupType.All );
        ICoreferenceInfo Process( XElement rdf, int rdfPartNumber = 0, CoreferenceGroupType groupType = CoreferenceGroupType.All );

        ICoreferenceInfo ReadFromRdf( XElement rdf, int rdfPartNumber = 0, bool processIfEmpty = true );
    }
}
