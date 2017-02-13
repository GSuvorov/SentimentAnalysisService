using System;

namespace Linguistics.Core.TonalityMarking
{
    /*
    /// <summary>
    /// 
    /// </summary>
    public struct TonalityTuple
    {
        public TonalityType SNT
        {
            get;
            set;
        }
        public int          FRT
        {
            get;
            set;
        }
    }

    /// <summary>
    /// 
    /// </summary>
    public struct TonalityInfo
    {
        public TonalityTuple? Current
        {
            get;
            set;
        }

        public TonalityTuple? Final
        {
            get;
            set;
        }

        public bool IsInquiry
        {
            get;
            set;
        }
    }
    */

    /// <summary>
    /// 
    /// </summary>
    [Serializable]
    public abstract class TonalityInfoBase
    {
        public TonalityType SNT
        {
            get;
            set;
        }
        public int FRT
        {
            get;
            set;
        }

        public TonalityType SNTFINAL
        {
            get;
            set;
        }
        public int FRTFINAL
        {
            get;
            set;
        }

        public void Set( TonalityType snt, int frt )
        {
            SNT = snt;
            FRT = frt;
        }
        public void Set( TonalityType snt )
        {
            SNT = snt;
        }
        public void SetFINAL( TonalityType sntfinal, int frtfinal )
        {
            SNTFINAL = sntfinal;
            FRTFINAL = frtfinal;
        }
        public void SetFINAL( TonalityType sntfinal )
        {
            SNTFINAL = sntfinal;
        }

        public override string ToString()
        {
            return ("SNT: " + SNT + ", SNTFINAL: " + SNTFINAL);
        }
    }

    /// <summary>
    /// 
    /// </summary>
    [Serializable]
    public class TonalityInfoEntity : TonalityInfoBase
    {
        [Flags]
        private enum InquiryEnum : byte
        {
            __UNDEFINE__     = 0x0,

            IsInquiry        = 0x1,
            IsInquiryInitial = 0x3,
        }

        private InquiryEnum _InquiryEnum;

        public bool IsInquiry
        {
            get { return ((_InquiryEnum & InquiryEnum.IsInquiry) == InquiryEnum.IsInquiry); }
            set 
            {  
                if ( value )
                    _InquiryEnum |= InquiryEnum.IsInquiry;
                else
                    _InquiryEnum = InquiryEnum.__UNDEFINE__;
            }
        }
        public bool IsInquiryInitial
        {
            get { return ((_InquiryEnum & InquiryEnum.IsInquiryInitial) == InquiryEnum.IsInquiryInitial); }
            set 
            {  
                if ( value )
                    _InquiryEnum |= InquiryEnum.IsInquiryInitial;
                else
                    _InquiryEnum = InquiryEnum.__UNDEFINE__;
            }
        }
    }

    /// <summary>
    /// 
    /// </summary>
    [Serializable]
    public class TonalityInfoSubSentence : TonalityInfoBase
    {
        public Entity Inquiry
        {
            get;
            set;
        }
    }
}




