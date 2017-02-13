using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

namespace OpinionMining
{
    using OpinionMining.DirectIndirectSpeechExtensions;
    using TextMining.Core;
    using SubsentPair = OpinionMining.DS_TwoSubsentBase.SubsentPair;

    internal abstract class DS_TwoSubsentBase : RuleBase
    {
        public DS_TwoSubsentBase( string id = null ) : base( id )
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
                                  subsent2.AttributeTypeAreEqual( SubsentenceType.DirectSpeech ) )

                          where subsent1.IsSubsentNotHasSubject()  && 
                                subsent2.IsSubsentNotHasSubject()  

                          select new SubsentPair( subsent1, subsent2 );

            var tuples2 = from subsent1 in sent.DescendantsSubsentence()
                          where subsent1.AttributeTypeAreEqual( SubsentenceType.DirectSpeech )

                          let subsent2 = subsent1.ElementsAfterSelfExceptT().FirstOrDefault()
                          where ( subsent2.IsNotNull() && 
                                  subsent2.IsElementSUBSENT() && 
                                  subsent2.AttributeTypeAreEqual( SubsentenceType.Subordinate,
                                                                  SubsentenceType.Default, 
                                                                  SubsentenceType.Introductory ) )

                          where ( subsent1.IsSubsentNotHasSubject()  &&
                                  subsent2.IsSubsentNotHasSubject() )

                          select new SubsentPair( subsent2, subsent1 );

            var tuples = tuples1.Concat( tuples2 );

            return (tuples);
        }

        protected abstract bool IsSubsent1MatchCondition( SubsentPair pair, Language language, out IEnumerable< SubjectData > subjectData );
        protected abstract XElement GetSubsent4SearchSubject( SubsentPair pair );
        protected abstract XElement GetSubsent4SearchObject( SubsentPair pair );

        public virtual SubjectObjectsTuple Process( SubsentPair pair, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
        #region [.condition.]
            var subjects = default(IEnumerable< SubjectData >);
            if ( !IsSubsent1MatchCondition( pair, language, out subjects ) )
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
            var subsent = GetSubsent4SearchObject( pair );
            var objects = subsent.ElementsExceptT().TryAllocateObjectsInUnknownElements( objectAllocateMethod, id );
                
            //mark begin-end DirectSpeech-subsents
            subsent.Elements().SetAttributeDIRECTSPEECH_BEGINEND( directAndIndirectSpeechGlobalNumber );

            //add 2 result
            return (new SubjectObjectsTuple( subjects, objects, Id ));
        #endregion
        }
    }
    //===================================================//

    internal class DS_TwoSubsent_01 : DS_TwoSubsentBase
    {
        public DS_TwoSubsent_01() : base()
        {
        }

        protected override bool IsSubsent1MatchCondition( SubsentPair pair, Language language, out IEnumerable< SubjectData > subjectData )
        {
            subjectData = null;

            if ( !GetSubsent4SearchObject( pair ).AttributeTypeAreEqual( SubsentenceType.DirectSpeech ) )
                return (false);
            
            var subsent    = GetSubsent4SearchSubject( pair );
            var resultList = default(List< SubjectData >);
            var sd         = default(SubjectData);

            sd = pattern12.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern6 .GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern2 .GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern5_onglance.Instance.GetSubjectData( subsent           );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern4_opinion .Instance.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern13.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern15.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern16.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern17.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern18.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern19.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern20.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern21.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern22.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern23.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern24.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;

        NEXT:
            sd.Add2List( ref resultList );

            if ( resultList.AnyEx() )
            {
                resultList = resultList.MakeDistinct();

                subjectData = resultList;
                return (true);
            }

            return (false);
        }
        protected override XElement GetSubsent4SearchSubject( SubsentPair pair )
        {
            return (pair.SUBSENT1);
        }
        protected override XElement GetSubsent4SearchObject( SubsentPair pair )
        {
            return (pair.SUBSENT2);
        }
    }

    internal class DS_TwoSubsent_02 : DS_TwoSubsentBase
    {
        public DS_TwoSubsent_02() : base()
        {
        }

        protected override bool IsSubsent1MatchCondition( SubsentPair pair, Language language, out IEnumerable< SubjectData > subjectData )
        {
            subjectData = null;

            if ( !GetSubsent4SearchObject( pair ).AttributeTypeAreEqual( SubsentenceType.DirectSpeech ) )
                return (false);
            
            var subsent    = GetSubsent4SearchSubject( pair );
            var resultList = default(List<SubjectData>);
            var sd         = default(SubjectData);

            sd = pattern12.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern13.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern15.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern16.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern17.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern18.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern19.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern20.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern21.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern22.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern23.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern24.GetSubjectData( subsent, language );  if ( sd.IsNotNull() ) goto NEXT;

        NEXT:
            sd.Add2List( ref resultList );

            if ( resultList.AnyEx() )
            {
                resultList = resultList.MakeDistinct();

                subjectData = resultList;
                return (true);
            }

            return (false);
        }
        protected override XElement GetSubsent4SearchSubject( SubsentPair pair )
        {
            return (pair.SUBSENT2);
        }
        protected override XElement GetSubsent4SearchObject( SubsentPair pair )
        {
            return (pair.SUBSENT1);
        }
    }
    //===================================================//

    internal class DS_TwoSubsentGroup : DS_GroupBase< DS_TwoSubsentBase, SubsentPair >
    {
        private static DS_TwoSubsentBase[] Get()
        {
            return
            (
                new DS_TwoSubsentBase[]
                { 
                    new DS_TwoSubsent_01(),
                    new DS_TwoSubsent_02(),
                }
            );
        }

        public DS_TwoSubsentGroup() : base( Get() )
        {
        }

        protected override SubsentPair[] GetEssence4Processing( XElement sent )
        {
            return (DS_TwoSubsentBase.GetSubsentPairs( sent ).ToArray());
        }
        protected override SubjectObjectsTuple ProcessEssence( DS_TwoSubsentBase pattern, SubsentPair essence, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
            if ( essence.IsNotHasSubject() )
            {
                return (pattern.Process( essence, language, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod ));
            }
            return (null);
        }
    }
    //===================================================//
    //===================================================//

    internal class DS_TwoSubsent_03 : DS_TwoSubsent_01
    {
        public DS_TwoSubsent_03() : base()
        {
        }

        public static IEnumerable< SubsentPair > GetSubsentPairs_2( XElement sent )
        {
            sent.ThrowIfNotSent();

            var tuples1 = from _subsent in sent.DescendantsSubsentence()
                          where _subsent.AttributeTypeAreEqual( SubsentenceType.Default )

                          let elements = _subsent.Elements().Take( 2 ).ToArray()
                          where ( elements.Length == 1 )

                          let subsent1 = elements[ 0 ]
                          where ( subsent1.IsElementSUBSENT() && 
                                  subsent1.AttributeTypeAreEqual( SubsentenceType.Subordinate ) )

                          let subsent2 = _subsent.ElementsAfterSelfExceptT().FirstOrDefault()
                          where ( subsent2.IsNotNull() && 
                                  subsent2.IsElementSUBSENT() && 
                                  subsent2.AttributeTypeAreEqual( SubsentenceType.DirectSpeech ) )

                          where subsent1.IsSubsentNotHasSubject()  && 
                                subsent2.IsSubsentNotHasSubject()  

                          select new SubsentPair( subsent1, subsent2 );

            var tuples2 = from subsent1 in sent.DescendantsSubsentence()
                          where ( subsent1.AttributeTypeAreEqual( SubsentenceType.DirectSpeech ) )

                          let _subsent = subsent1.ElementsAfterSelfExceptT().FirstOrDefault()
                          where ( _subsent.IsNotNull() && 
                                  _subsent.IsElementSUBSENT() && 
                                  _subsent.AttributeTypeAreEqual( SubsentenceType.Default ) )

                          let subsent2 = TryGetSubordinateSubsent( _subsent )
                          where ( subsent2.IsNotNull() && 
                                  subsent2.IsElementSUBSENT() && 
                                  subsent2.AttributeTypeAreEqual( SubsentenceType.Subordinate ) )

                          where subsent1.IsSubsentNotHasSubject()  && 
                                subsent2.IsSubsentNotHasSubject()  

                          select new SubsentPair( subsent2, subsent1 );

            var tuples = tuples1.Concat( tuples2 );

            return (tuples);
        }
        private static XElement TryGetSubordinateSubsent( XElement defaultSubsent )
        {
            var elements = defaultSubsent.Elements().Take( 2 ).ToArray();
            if ( elements.Length == 1 )   
            {
                return (elements[ 0 ]);
            }
            if ( elements.Length == 2 )   
            {
                var e = elements[ 0 ];
                if ( e.IsElementU() && e.Value.ContainsSignOfPunctuation())
                    return (elements[ 1 ]);
            }
            return (null);
        }
    }
    //===================================================//

    internal class DS_TwoSubsentGroup_2 : DS_GroupBase< DS_TwoSubsent_03, SubsentPair >
    {
        private static DS_TwoSubsent_03[] Get()
        {
            return
            (
                new DS_TwoSubsent_03[]
                { 
                    new DS_TwoSubsent_03(),
                }
            );
        }

        public DS_TwoSubsentGroup_2() : base( Get() )
        {
        }

        protected override SubsentPair[] GetEssence4Processing( XElement sent )
        {
            return (DS_TwoSubsent_03.GetSubsentPairs_2( sent ).ToArray());
        }
        protected override SubjectObjectsTuple ProcessEssence( DS_TwoSubsent_03 pattern, SubsentPair essence, Language language
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
