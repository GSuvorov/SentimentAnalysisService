using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

namespace OpinionMining
{
    using OpinionMining.DirectIndirectSpeechExtensions;
    using TextMining.Core;

    internal abstract class DS_ThreeSubsentBase : RuleBase
    {
        public DS_ThreeSubsentBase( string id = null ) : base( id )
        {
        }

        internal class SubsentTuple
        {
            public SubsentTuple( XElement subsent1, XElement subsent2, XElement subsent3 )
            {
                subsent1.ThrowIfNotSubsent();
                subsent2.ThrowIfNotSubsent();
                subsent3.ThrowIfNotSubsent();

                SUBSENT1 = subsent1;
                SUBSENT2 = subsent2;
                SUBSENT3 = subsent3;
            }
            public XElement SUBSENT1
            {
                get;
                private set;
            }
            public XElement SUBSENT2
            {
                get;
                private set;
            }
            public XElement SUBSENT3
            {
                get;
                private set;
            }
            public bool IsNotHasSubject()
            {
                return (SUBSENT1.IsSubsentNotHasSubject() && SUBSENT2.IsSubsentNotHasSubject() && SUBSENT3.IsSubsentNotHasSubject());
            }
        }

        public static IEnumerable< SubsentTuple > GetSubsentTuples( XElement sent )
        {
            sent.ThrowIfNotSent();

            var tuples = from subsent1 in sent.DescendantsSubsentence()
                         where subsent1.AttributeTypeAreEqual( SubsentenceType.DirectSpeech )

                         let subsent2 = subsent1.ElementsAfterSelfExceptT().FirstOrDefault()
                         where ( subsent2.IsNotNull() && 
                                 subsent2.IsElementSUBSENT() && 
                                 subsent2.AttributeTypeAreEqual( SubsentenceType.Subordinate,
                                                                 SubsentenceType.Default, 
                                                                 SubsentenceType.Introductory ) )

                         let subsent3 = subsent2.ElementsAfterSelfExceptT().FirstOrDefault()
                         where ( subsent3.IsNotNull() &&
                                 subsent3.IsElementSUBSENT() && 
                                 subsent3.AttributeTypeAreEqual( SubsentenceType.DirectSpeech ) )

                         where ( subsent1.IsSubsentNotHasSubject()  && 
                                 subsent2.IsSubsentNotHasSubject()  && 
                                 subsent3.IsSubsentNotHasSubject() )

                         select new SubsentTuple( subsent1, subsent2, subsent3 );
            return (tuples);
        }

        protected abstract bool IsSubsent2MatchCondition( SubsentTuple tuple, Language language, out IEnumerable< SubjectData > subjectData );

        public virtual SubjectObjectsTuple Process( SubsentTuple tuple, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
        #region [.condition.]
            var subjects = default(IEnumerable< SubjectData >);
            if ( !IsSubsent2MatchCondition( tuple, language, out subjects ) )
            {
                return (null); 
            }
        #endregion

        #region [.match condition.]
            MatchActionDebugInfoOutput( this.Id );

            //growup global IndirectSpeech-subsent number
            directAndIndirectSpeechGlobalNumber++;

            //set 'ISSUBJECT' attribute for subject-entity
            subjects.SetAttributeISSUBJECTforAllEntity( directAndIndirectSpeechGlobalNumber );

            //set 'ISOBJECT' attribute for all object-entity
            var id = subjects.GetAttributeIDForce();
            //allocate objects
            var objects = tuple.SUBSENT1.ElementsExceptT().TryAllocateObjectsInUnknownElements( objectAllocateMethod, id )
                         .Concat
                         (
                          tuple.SUBSENT3.ElementsExceptT().TryAllocateObjectsInUnknownElements( objectAllocateMethod, id )
                         )
                         .ToArray();

            //mark begin-end DirectSpeech-subsents
            tuple.SUBSENT1.Elements().SetAttributeDIRECTSPEECH_BEGINEND( directAndIndirectSpeechGlobalNumber );
            tuple.SUBSENT3.Elements().SetAttributeDIRECTSPEECH_BEGINEND( directAndIndirectSpeechGlobalNumber );

            //add 2 result
            return (new SubjectObjectsTuple( subjects, objects, Id ));
        #endregion
        }
    }
    //===================================================//

    internal class DS_ThreeSubsent_01 : DS_ThreeSubsentBase
    {
        public DS_ThreeSubsent_01(): base()
        {
        }

        protected override bool IsSubsent2MatchCondition( SubsentTuple tuple, Language language, out IEnumerable< SubjectData > subjectData )
        {            
            subjectData = null;

            var resultList = default(List< SubjectData >);
            var subsent    = tuple.SUBSENT2;

            var subjectData1  = pattern12.GetSubjectData( subsent, language );
            var subjectData2  = pattern13.GetSubjectData( subsent, language );
            var subjectData3  = pattern15.GetSubjectData( subsent, language );
            var subjectData4  = pattern16.GetSubjectData( subsent, language );
            var subjectData5  = pattern17.GetSubjectData( subsent, language );
            var subjectData6  = pattern6 .GetSubjectData( subsent, language );
            var subjectData7  = pattern18.GetSubjectData( subsent, language );
            var subjectData8  = pattern19.GetSubjectData( subsent, language );
            var subjectData9  = pattern20.GetSubjectData( subsent, language );
            var subjectData10 = pattern21.GetSubjectData( subsent, language );
            var subjectData11 = pattern22.GetSubjectData( subsent, language );
            var subjectData12 = pattern23.GetSubjectData( subsent, language );
            var subjectData13 = pattern24.GetSubjectData( subsent, language );

            subjectData1 .Add2List( ref resultList );
            subjectData2 .Add2List( ref resultList );
            subjectData3 .Add2List( ref resultList );
            subjectData4 .Add2List( ref resultList );
            subjectData5 .Add2List( ref resultList );
            subjectData6 .Add2List( ref resultList );
            subjectData7 .Add2List( ref resultList );
            subjectData8 .Add2List( ref resultList );
            subjectData9 .Add2List( ref resultList );
            subjectData10.Add2List( ref resultList );
            subjectData11.Add2List( ref resultList );
            subjectData12.Add2List( ref resultList );
            subjectData13.Add2List( ref resultList );

            if ( resultList.AnyEx() )
            {
                resultList = resultList.MakeDistinct();

                subjectData = resultList;
                return (true);
            }

            return (false);
        }
    }
    //===================================================//

    internal class DS_ThreeSubsentGroup : DS_GroupBase< DS_ThreeSubsentBase, DS_ThreeSubsentBase.SubsentTuple >
    {
        private static DS_ThreeSubsentBase[] Get()
        {
            return
            (
                new[]
                { 
                    new DS_ThreeSubsent_01(),
                }
            );
        }

        public DS_ThreeSubsentGroup() : base( Get() )
        {
        }

        protected override DS_ThreeSubsentBase.SubsentTuple[] GetEssence4Processing( XElement sent )
        {
            return (DS_ThreeSubsentBase.GetSubsentTuples( sent ).ToArray());
        }
        protected override SubjectObjectsTuple ProcessEssence( DS_ThreeSubsentBase pattern, DS_ThreeSubsentBase.SubsentTuple essence, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
            if ( essence.IsNotHasSubject() )
            {
                return (pattern.Process( essence, language, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod ));
            }
            return (null);
        }
    }
}
