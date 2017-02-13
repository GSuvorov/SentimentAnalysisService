using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Runtime.Serialization;

using OpinionMining;

namespace TextMining.Core
{
    /// <summary>
    /// 
    /// </summary>
    [DataContract]
    [Serializable]
    public abstract class OutputResultTupleBase< T > : OutputResultBase
        where T : OpinionMiningTuple
    {
        public OutputResultTupleBase
            (
            IEnumerable< T >      tuples,
            IEnumerable< string > operateRulesNames = null
            )
            : base( operateRulesNames )
        {
            tuples.ThrowIfNull("tuples");

            #region commented
		    /*var groupBySubject  = from tuple in opinionMiningTuples
                                    group tuple by tuple.Subject into group_by_subject
                                  select group_by_subject;
            var distinctedTupes = from g in groupBySubject
                                  select new OpinionMiningTuple
                                  ( 
                                      g.Key, 
                                      g.SelectMany( _ => _.Objects ).Distinct().ToArray() 
                                  );
            OpinionMiningTuples = distinctedTupes.ToList().AsReadOnly();*/ 
	        #endregion

            Tuples = tuples.ToList().AsReadOnly();

            #region commented
            /*OpinionMiningTuples = (from t in opinionMiningTuples
                                   orderby t.Subject.ToString(), t.Object.ToString()
                                   select t
                                  )
                                  .ToList()
                                  .AsReadOnly();*/
            #endregion
        }

        [DataMember]
        public ReadOnlyCollection< T > Tuples
        {
            get;
            private set;
        }

        public string OutputXml
        {
            get
            {
                var sents = from t in Tuples
                            select t.GetSentence();
                
                return (SentenceText.ToXDocument( sents ).ToString());
            }
        }

        public ReadOnlyCollection< T > AuthorSubjectOpinionMiningTuples
        {
            get
            {
                return (Tuples.Where( omt => omt.Subject.IsAuthor ).ToList().AsReadOnly());
            }
        }
        public ReadOnlyCollection< T > OtherSubjectOpinionMiningTuples
        {
            get
            {
                return (Tuples.Where( omt => !omt.Subject.IsAuthor ).ToList().AsReadOnly());
            }
        }
    }
}
