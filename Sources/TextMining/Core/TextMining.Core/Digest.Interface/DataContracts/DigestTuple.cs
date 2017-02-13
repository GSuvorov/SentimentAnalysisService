using System;
using System.Runtime.Serialization;

using OpinionMining;

namespace Digest
{   
    /// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public class DigestTuple : OpinionMiningTuple
    {
        public DigestTuple( OpinionMiningTuple other4Copy, string sentenceXml )
            : base( other4Copy, sentenceXml )
        {
        }
        public DigestTuple( OpinionMiningTuple other4Copy, string sentenceXml, double? positive, double? negative )
            : base( other4Copy, sentenceXml )
        {
            Positive = positive;
            Negative = negative;
        }

        [DataMember]
        public double? Positive
        {
            get;
            private set;
        }

        [DataMember]
        public double? Negative
        {
            get;
            private set;
        }

        public override string ToString()
        {
            return ( base.ToString() + 
                    " (Positive: " + ((Positive.HasValue) ? Positive.Value.ToString() : "-") + 
                    ", Negative: " + ((Negative.HasValue) ? Negative.Value.ToString() : "-")  + ")" );
        }
    }
}
