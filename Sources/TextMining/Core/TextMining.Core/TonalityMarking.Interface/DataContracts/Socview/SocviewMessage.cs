using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.Xml.Linq;

using Linguistics.Coreference;

namespace TonalityMarking
{
    /// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public class SocviewMessage
    {
        [DataMember]
        public string Id
        {
            get;
            set;
        }

        [DataMember]
        public string PlainText
        {
            get;
            set;
        }
       

        [DataMember]
        public SocviewMessage[] Children
        {
            get;
            set;
        }

        [DataMember]
        public SocviewSent[] Sents
        {
            get;
            set;
        }


        public bool IsProcessed
        {
            get { return (Sents != null && Sents.Length != 0); }
        }
        public List< TonalityMarkingObject > Objects
        {
            get;
            set;
        }
        public ICoreferenceInfo CoreferenceInfo
        {
            get;
            set;
        }

        public int CalcSumNegFRT()
        {
            var result = Sents.Sum( sent => sent.SumNegFRT );

            foreach ( var child in Children )
            {
                result += child.CalcSumNegFRT();
            }

            return (result);
        }
        public int CalcSumNeg()
        {
            var result = Sents.Sum( sent => sent.SumNeg );

            foreach ( var child in Children )
            {
                result += child.CalcSumNeg();
            }

            return (result);
        }
        public int CalcSumPosFRT()
        {
            var result = Sents.Sum( sent => sent.SumPosFRT );

            foreach ( var child in Children )
            {
                result += child.CalcSumPosFRT();
            }

            return (result);
        }
        public int CalcSumPos()
        {
            var result = Sents.Sum( sent => sent.SumPos );

            foreach ( var child in Children )
            {
                result += child.CalcSumPos();
            }

            return (result);
        }

        public override string ToString()
        {
            return ("Id: " + Id + ", PlainText: " + PlainText);
        }
    }

    /// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public class SocviewSent
    {
        [DataMember]
        public int FRT 
        {
            get;
            set;
        }

        [DataMember]
        public string Offsets
        {
            get;
            set;
        }


        public XElement Element
        {
            get;
            set;
        }

        public int SumNegFRT
        {
            get;
            set;
        }
        public int SumNeg
        {
            get;
            set;
        }
        public int SumPosFRT
        {
            get;
            set;
        }
        public int SumPos
        {
            get;
            set;
        }

        /*[DataMember]
        public int Number
        {
            get;
            private set;
        }*/
    }
}
