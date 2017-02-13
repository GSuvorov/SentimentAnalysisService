using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

namespace OpinionMining
{
    using OpinionMining.DirectIndirectSpeechExtensions;
    using TextMining.Core;

    internal abstract class DS_TwoSubsent_ExBase : RuleBase
    {
        public DS_TwoSubsent_ExBase( string id = null ) : base( id )
        {
        }

        internal class SubsentPair
        {
            public SubsentPair( XElement subsent1, XElement subsent2 )
            {
                subsent1.ThrowIfNotSubsent();
                subsent2.ThrowIfNotSubsent();

                SUBSENT1 = subsent1;
                SUBSENT2 = subsent2;
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

            public SubsentPair SwapSubsents()
            {
                return (new SubsentPair( SUBSENT2, SUBSENT1 ));
            }
            public bool IsNotHasSubject()
            {
                return (SUBSENT1.IsSubsentNotHasSubject() && SUBSENT2.IsSubsentNotHasSubject());
            }
        }

        private static bool IsComma( string text )
        {
            return (text.TrimWhiteSpaces() == ",");
        }

        public static IEnumerable< SubsentPair > GetSubsentPairs( XElement sent )
        {
            sent.ThrowIfNotSent();

            var tuples1 = from subsent1 in sent.DescendantsSubsentence()
                          where subsent1.AttributeTypeAreEqual( SubsentenceType.Subordinate,
                                                                SubsentenceType.Default, 
                                                                SubsentenceType.Introductory )

                          let subsent2 = subsent1.ElementsAfterSelfExceptT().FirstOrDefault()
                          where ( subsent2.IsNotNull() && 
                                  subsent2.IsElementSUBSENT() &&
                                  subsent2.IsSubsentEndWithU( IsComma ) )

                          let subsent3 = subsent2.ElementsAfterSelfExceptT().FirstOrDefault()
                          where ( subsent3.IsNotNull() && 
                                  subsent3.IsElementSUBSENT() && 
                                  subsent3.AttributeTypeAreEqual( SubsentenceType.DirectSpeech ) )

                          where subsent1.IsSubsentNotHasSubject()  && 
                                subsent2.IsSubsentNotHasSubject()  && 
                                subsent3.IsSubsentNotHasSubject() 

                          select new SubsentPair( subsent1, subsent3 );

            var tuples2 = from subsent1 in sent.DescendantsSubsentence()
                          where subsent1.AttributeTypeAreEqual( SubsentenceType.DirectSpeech )

                          let subsent2 = subsent1.ElementsAfterSelfExceptT().FirstOrDefault()
                          where ( subsent2.IsNotNull() && 
                                  subsent2.IsElementSUBSENT() &&
                                  subsent2.IsSubsentEndWithU( IsComma ) )

                          let subsent3 = subsent2.ElementsAfterSelfExceptT().FirstOrDefault()
                          where ( subsent3.IsNotNull() && 
                                  subsent3.IsElementSUBSENT() && 
                                  subsent3.AttributeTypeAreEqual( SubsentenceType.Subordinate,
                                                                  SubsentenceType.Default, 
                                                                  SubsentenceType.Introductory ) )

                          where ( subsent1.IsSubsentNotHasSubject()  &&
                                  subsent2.IsSubsentNotHasSubject()  && 
                                  subsent3.IsSubsentNotHasSubject() )

                          select new SubsentPair( subsent1, subsent3 );

            var tuples = tuples1.Concat( tuples2 );

            return (tuples);
        }

        protected bool SubsentsIsSwaped
        {
            get;
            set;
        }
        protected abstract bool IsSubsent1MatchCondition( SubsentPair pair, Language language, out IEnumerable< SubjectData > subjectData );

        public virtual SubjectObjectsTuple Process( SubsentPair pair, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
        #region [.condition.]
            SubsentsIsSwaped = false;

            var subjects = default(IEnumerable< SubjectData >);
            if ( !IsSubsent1MatchCondition( pair, language, out subjects ) )
            {
                pair = pair.SwapSubsents();
                SubsentsIsSwaped = true;
                if ( !IsSubsent1MatchCondition( pair, language, out subjects ) )
                {
                    return (null); 
                }
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
            var subsent = pair.SUBSENT1; // (SubsentsIsSwaped ? pair.SUBSENT1 : pair.SUBSENT2);
            var objects = subsent.ElementsExceptT().TryAllocateObjectsInUnknownElements( objectAllocateMethod, id );
                
            //mark begin-end DirectSpeech-subsents
            subsent.Elements().SetAttributeDIRECTSPEECH_BEGINEND( directAndIndirectSpeechGlobalNumber );

            //add 2 result
            return (new SubjectObjectsTuple( subjects, objects, Id ));
        #endregion
        }
    }
    //===================================================//

    internal class DS_TwoSubsent_Ex_01 : DS_TwoSubsent_ExBase
    {
        public DS_TwoSubsent_Ex_01() : base()
        {
        }

        protected override bool IsSubsent1MatchCondition( SubsentPair pair, Language language, out IEnumerable< SubjectData > subjectData )
        {
            subjectData = null;

            if ( !pair.SUBSENT1.AttributeTypeAreEqual( SubsentenceType.DirectSpeech ) )
                return (false);

            var resultList = default(List< SubjectData >);
            var subsent    = pair.SUBSENT2;

            var subjectData1  = pattern12.GetSubjectData( subsent, language );
            var subjectData2  = pattern13.GetSubjectData( subsent, language );
            var subjectData3  = pattern15.GetSubjectData( subsent, language );
            var subjectData4  = pattern16.GetSubjectData( subsent, language );
            var subjectData5  = pattern17.GetSubjectData( subsent, language );
            var subjectData6  = pattern18.GetSubjectData( subsent, language );
            var subjectData7  = pattern19.GetSubjectData( subsent, language );
            var subjectData8  = pattern20.GetSubjectData( subsent, language );
            var subjectData9  = pattern21.GetSubjectData( subsent, language );
            var subjectData10 = pattern22.GetSubjectData( subsent, language );
            var subjectData11 = pattern23.GetSubjectData( subsent, language );
            var subjectData12 = pattern24.GetSubjectData( subsent, language );

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

    internal class DS_TwoSubsent_ExGroup : DS_GroupBase< DS_TwoSubsent_ExBase, DS_TwoSubsent_ExBase.SubsentPair >
    {
        private static DS_TwoSubsent_ExBase[] Get()
        {
            return
            (
                new DS_TwoSubsent_ExBase[]
                { 
                    new DS_TwoSubsent_Ex_01(),
                }
            );
        }

        public DS_TwoSubsent_ExGroup() : base( Get() )
        {
        }

        protected override DS_TwoSubsent_ExBase.SubsentPair[] GetEssence4Processing( XElement sent )
        {
            return (DS_TwoSubsent_ExBase.GetSubsentPairs( sent ).ToArray());
        }
        protected override SubjectObjectsTuple ProcessEssence( DS_TwoSubsent_ExBase pattern, DS_TwoSubsent_ExBase.SubsentPair essence, Language language
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
