using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Runtime.Serialization;

using TextMining.Core;

namespace TonalityMarking
{
    /// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public class TonalityMarkingTuple
    {
        private const string AUTHOR = "Author";
        public TonalityMarkingTuple
            ( 
            string                         subject,
            double?                        positive,
            double?                        negative,
            IList< TonalityMarkingObject > objects
            )
        {
            subject.ThrowIfEmptyOrNull("subject");
            objects.ThrowIfNull("objects");

            Subject  = subject;
            Positive = positive;
            Negative = negative;
            Objects  = objects.ToList().AsReadOnly();
        }

        [DataMember]
        public string Subject
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

        [DataMember]
        public ReadOnlyCollection< TonalityMarkingObject > Objects
        {
            get;
            private set;
        }

        public static TonalityMarkingTuple CreateSubjectAuthor( double? positive, double? negative, List< TonalityMarkingObject > objects )
        {
            return (new TonalityMarkingTuple( AUTHOR, positive, negative, objects ));
        }
    }
}
