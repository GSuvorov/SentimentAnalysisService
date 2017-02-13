using System;
using System.Collections.Generic;
using System.Runtime.Serialization;

namespace TextMining.Core
{
    /// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public abstract class InputParamsBase
    {
        public InputParamsBase( string inputText, InputTextFormat inputTextFormat = InputTextFormat.PlainText )
        {
            InputText            = inputText;
            InputTextFormat      = inputTextFormat;
            ObjectAllocateMethod = ObjectAllocateMethod.PhysicalOrJuridicalPersons;
        }

        [DataMember]
        public string InputText
        {
            get { return (_InputText); }
            set
            {
                value.ThrowIfEmptyOrNull("InputText");

                _InputText = value;
            }
        }
        private string _InputText;

        [DataMember]
        public InputTextFormat InputTextFormat
        {
            get;
            set;
        }
        
        [DataMember]
        public virtual List< string > InquiriesSynonyms
        {
            get { return (_InquiriesSynonyms); }
            set
            {
                _InquiriesSynonyms = (value != null) ? value.CheckInquiriesSynonymsAndPrepare() : null;
            }
        }
        private List< string > _InquiriesSynonyms;

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
        public ObjectAllocateMethod ObjectAllocateMethod
        {
            get;
            set;
        }
    }
}
