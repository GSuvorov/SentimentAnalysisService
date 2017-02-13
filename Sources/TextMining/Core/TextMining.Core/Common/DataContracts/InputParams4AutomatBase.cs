using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Runtime.Serialization;
using System.ServiceModel;
using System.ServiceModel.Web;
using System.Text;
using System.Xml.Linq;

namespace TextMining.Core
{
    /*/// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public abstract class InputParams4AutomatBase : InputParamsBase
    {
        public InputParams4AutomatBase
            ( 
            string         inputText, 
            List< string > inquiriesSynonyms, 
            List< int >    offsets
            )
            : base( inputText, InputTextFormat.PlainText )
        {
            InquiriesSynonyms = inquiriesSynonyms;
            Offsets           = offsets;
        }

        private List< int > _Offsets;
        [DataMember]
        public List< int > Offsets
        {
            get { return (_Offsets); }
            set
            {
                //value.ThrowIfNull("offsets");
                //if ( !value.Any() ) throw (new ArgumentNullException("offsets"));

                _Offsets = value;
            }
        }

        private List< string > _Coreferences;
        [DataMember]
        public List< string > Coreferences
        {
            get { return (_Coreferences); }
            set
            {
                //_Coreferences = value ?? new List< string >();

                _Coreferences = value;
            }
        }

        /*public override List< string > InquiriesSynonyms
        {
            get
            {
                return (base.InquiriesSynonyms);
            }
            set
            {
                value.ThrowIfNull("inquiriesSynonyms");

                base.InquiriesSynonyms = value.CheckInquiriesSynonymsAndPrepare();
            }
        }*/

        /*[DataMember]
        public AutomatInquiryType InquiryType
        {
            get;
            set;
        }*/

        [DataMember]
        public LingvisticClient.TonInquiryType InquiryType
        {
            get;
            set;
        }
    }*/
}
