using System;
using System.Collections.Generic;
using System.Runtime.Serialization;

namespace TonalityMarking
{
    /*
    /// <summary>
    /// 
    /// </summary>
    public enum OutputSocviewFormat
    {
        Offsets, //converted from linguistic-xml to offsets-map
        LinguisticXml, //processed linguistic-xml
    }

    /// <summary>
    /// processing Socview-post's mode
    /// </summary>
    public enum ProcessingSocviewMode
    {
        Parallel,      //processing post's parallel
        Consecutively, //processing post's consecutively
    }
    */

    /// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public class SocviewProcessingData
    {
        /*
        [DataMember]
        public OutputSocviewFormat OutputSocviewFormat
        {
            get;
            set;
        }

        [DataMember]
        public ProcessingSocviewMode ProcessingSocviewMode
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
        */


        [DataMember]
        public SocviewMessage SocviewPost
        {
            get;
            set;
        }

        [DataMember]
        public int SumNegFRT
        {
            get;
            set;
        }
        [DataMember]
        public int SumNeg
        {
            get;
            set;
        }
        [DataMember]
        public int SumPosFRT
        {
            get;
            set;
        }
        [DataMember]
        public int SumPos
        {
            get;
            set;
        }


        public IEnumerable< SocviewMessage > GetUnprocessedSocviewMessages()
        {
            return GetUnprocessedSocviewMessages( SocviewPost );
        }
        private IEnumerable< SocviewMessage > GetUnprocessedSocviewMessages( SocviewMessage parent )
        {
            if ( !parent.IsProcessed )
            {
                yield return parent;
            }

            foreach ( var child in parent.Children )
            {
                foreach ( var subchild in GetUnprocessedSocviewMessages( child ) )
                {
                    yield return subchild;
                }
            }
        }
    }
}
