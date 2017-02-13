using System;
using System.Runtime.Serialization;
using System.Xml.Linq;

using TextMining.Core;
using Lingvistics;

namespace TonalityMarking
{
    /// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public class TonalityMarkingObject
    {
        public TonalityMarkingObject( XElement element, string name, double? positive, double? negative )
        {
            element.ThrowIfNull("element");
            name   .ThrowIfEmptyOrNull("name");

            Element  = element;
            Name     = EntityNormalizer.Normalize( name );
            Positive = positive;
            Negative = negative;
        }
        public TonalityMarkingObject( XElement homogeneousParentElement, 
                                      XElement element, string name, double? positive, double? negative )
            : this( element, name, positive, negative )
        {
            homogeneousParentElement.ThrowIfNull("homogeneousParentElement");

            HomogeneousElement = homogeneousParentElement;
        }

        [DataMember]
        public string Name
        {
            get;
            private set;
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

        public bool IsPositive
        {
            get { return (Positive.HasValue); }
        }
        public bool IsNegative
        {
            get { return (Negative.HasValue); }
        }

        public XElement Element
        {
            get;
            private set;
        }
        public XElement HomogeneousElement
        {
            get;
            private set;
        }
        public XElement GetHomogeneousElementOrElement()
        {
            return (HomogeneousElement ?? Element);
        }

        public override string ToString()
        {
            return (Name + ", Positive: " + Positive + ", Negative: " + Negative);
        }
    }
}
