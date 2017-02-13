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
    public class TonalityMarkingInputParams4Blogs
    {
        public TonalityMarkingInputParams4Blogs( XElement inputXml, IEnumerable< string > inquiriesSynonyms )
        {
            Init( inputXml, inquiriesSynonyms );
        }
        public TonalityMarkingInputParams4Blogs( string inputXml, IEnumerable< string > inquiriesSynonyms )
        {
            inputXml.ThrowIfEmptyOrNull("inputText");

            Init( inputXml.ToXDocument().Root, inquiriesSynonyms );
        }

        private void Init( XElement inputXml, IEnumerable< string > inquiriesSynonyms )
        {
            inputXml         .ThrowIfNull("inputText");
            inquiriesSynonyms.ThrowIfNull("inquiriesSynonyms");

            InputXml          = inputXml;
            InquiriesSynonyms = inquiriesSynonyms.ToList().CheckInquiriesSynonymsAndPrepare().AsReadOnly();
            if ( !InquiriesSynonyms.Any() ) throw (new ArgumentException("!InquiriesSynonyms.Any()"));
        }

        [DataMember]
        public XElement InputXml
        {
            get;
            private set;
        }
        
        [DataMember]
        public ReadOnlyCollection< string > InquiriesSynonyms
        {
            get;
            private set;
        }

        [DataMember]
        public OutputBlogsFormat OutputBlogsFormat
        {
            get;
            set;
        }


        [DataMember]
        public LanguageType LanguageType
        {
            get;
            set;
        }

        [DataMember]
        public ThemeType ThemeType
        {
            get;
            set;
        }

        [DataMember]
        public ProcessingBlogsMode ProcessingBlogsMode
        {
            get;
            set;
        }        
    }
}
