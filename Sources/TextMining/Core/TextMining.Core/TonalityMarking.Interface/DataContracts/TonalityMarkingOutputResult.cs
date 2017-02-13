using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Runtime.Serialization;
using System.Xml.Linq;

using TextMining.Core;

namespace TonalityMarking
{
    /// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public class TonalityMarkingOutputResult : OutputResultBase
    {
        public TonalityMarkingOutputResult
            (
            XDocument                     resultXDocument,
            InquiryAllocateStatus         inquiryAllocateStatus,
            double?                       positive,
            double?                       negative,
            List< TonalityMarkingObject > objects,
            IEnumerable< string >         operateRulesNames = null
            )
            : base( operateRulesNames )
        {
            resultXDocument.ThrowIfNull("resultXDocument");
            //if ( !resultXDocument.Root.HasElements ) throw (new ArgumentNullException("Tonality output-xml is empty."));

            _OutputXDocument           = resultXDocument;
            OutputXml                  = resultXDocument.ToString();
            OutputXmlIsEmpty           = !resultXDocument.Root.HasElements;
            InquiryAllocateStatus      = inquiryAllocateStatus;
            AuthorTonalityMarkingTuple = TonalityMarkingTuple.CreateSubjectAuthor( positive, negative, objects );
            TonalityMarkingTuples      = (new[] { AuthorTonalityMarkingTuple }).ToList().AsReadOnly();
        }

        public TonalityMarkingOutputResult
            (
            string                        resultXml,
            bool                          resultXmlIsEmpty,
            InquiryAllocateStatus         inquiryAllocateStatus,
            double?                       positive,
            double?                       negative,
            List< TonalityMarkingObject > objects,
            IEnumerable< string >         operateRulesNames = null
            )
            : base( operateRulesNames )
        {
            OutputXml                  = resultXml;
            OutputXmlIsEmpty           = resultXmlIsEmpty;
            InquiryAllocateStatus      = inquiryAllocateStatus;
            AuthorTonalityMarkingTuple = TonalityMarkingTuple.CreateSubjectAuthor( positive, negative, objects );
            TonalityMarkingTuples      = (new[] { AuthorTonalityMarkingTuple }).ToList().AsReadOnly();
        }

        [DataMember]
        public string OutputXml
        {
            get;
            private set;
        }

        private XDocument _OutputXDocument;
        public XDocument GetOutputXDocument()
        {
            if ( _OutputXDocument == null ) 
            {
                _OutputXDocument = XDocument.Parse( OutputXml );
            }
            return (_OutputXDocument);
        }

        [DataMember]
        public bool OutputXmlIsEmpty
        {
            get;
            private set;
        }

        [DataMember]
        public InquiryAllocateStatus InquiryAllocateStatus
        {
            get;
            private set;
        }

        [DataMember]
        public ReadOnlyCollection< TonalityMarkingTuple > TonalityMarkingTuples
        {
            get;
            private set;
        }

        [DataMember]
        public TonalityMarkingTuple AuthorTonalityMarkingTuple
        {
            get;
            private set;
        }
    }
}
