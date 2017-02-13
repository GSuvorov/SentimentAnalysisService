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
    public class TonalityMarkingTuple4Blogs
    {
        public TonalityMarkingTuple4Blogs
            ( 
            string                         id,
            string                         outputResult,
            OutputBlogsFormat              outputBlogsFormat,
            double?                        positive,
            double?                        negative,
            IList< TonalityMarkingObject > objects
            )
        {
                      id.ThrowIfEmptyOrNull("id");
            outputResult.ThrowIfEmptyOrNull("outputResult");
                 objects.ThrowIfNull("objects");

            Id                = id;
            OutputResult      = outputResult;
            OutputBlogsFormat = outputBlogsFormat;
            Positive          = positive;
            Negative          = negative;
            Objects           = objects.ToList().AsReadOnly();
        }

        [DataMember]
        public string Id
        {
            get;
            private set;
        }

        [DataMember]
        public string OutputResult
        {
            get;
            private set;
        }

        [DataMember]
        public OutputBlogsFormat OutputBlogsFormat
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

        public override string ToString()
        {
            return ("(Positive: "  + ((Positive.HasValue) ? Positive.Value.ToString() : "-") + 
                    ", Negative: " + ((Negative.HasValue) ? Negative.Value.ToString() : "-") + "), " +
                    "Id: " + Id + ", " + OutputResult);
        }
    }
}
