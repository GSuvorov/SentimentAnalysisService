using System;
using System.Collections.Generic;
using System.Runtime.Serialization;

using TextMining.Core;

namespace TonalityMarking
{
    /// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public class TonalityMarkingInputParams : InputParamsBase
    {
        public TonalityMarkingInputParams( string inputText, InputTextFormat inputTextFormat = InputTextFormat.PlainText )
            : base( inputText, inputTextFormat )
        {
            ObjectAllocateMethod = ObjectAllocateMethod.FirstVerbEntityWithRoleObj;
        }
    }


#if WITH_OM_TM
    /// <summary>
    /// 
    /// </summary>
    public sealed class TonalityMarkingInputParams4InProcess
    {
        private List< string > _InquiriesSynonyms;

        public TonalityMarkingInputParams4InProcess()
        {
        }
        
        public LanguageType   LanguageType { get; set; }
        public ThemeType      ThemeType { get; set; }        
        public List< string > InquiriesSynonyms
        {
            get { return (_InquiriesSynonyms); }
            set
            {
                _InquiriesSynonyms = (value != null) ? value.CheckInquiriesSynonymsAndPrepare() : null;
            }
        }
    }
#endif
}
