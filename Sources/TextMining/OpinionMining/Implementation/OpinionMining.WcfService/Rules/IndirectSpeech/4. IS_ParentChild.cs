using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

namespace OpinionMining
{
    using OpinionMining.DirectIndirectSpeechExtensions;
    using TextMining.Core;

    internal abstract class IS_ParentChildBase : RuleBase
    {
        public IS_ParentChildBase( string id = null ) : base( id )
        {
        }

        internal class SubsentPair
        {
            public SubsentPair( XElement parent, XElement child )
            {
                parent.ThrowIfNotSubsent();
                child.ThrowIfNotSubsent();

                PARENT = parent;
                CHILD = child;
            }
            public XElement PARENT
            {
                get;
                private set;
            }
            public XElement CHILD
            {
                get;
                private set;
            }

            public SubsentPair SwapSubsents()
            {
                return (new SubsentPair( CHILD, PARENT ));
            }
            public bool IsNotHasSubject()
            {
                return (PARENT.IsSubsentNotHasSubject() && CHILD.IsSubsentNotHasSubject());
            }            
        }

        public static IEnumerable< SubsentPair > GetSubsentPairs( XElement sent )
        {
            sent.ThrowIfNotSent();

            var tuples = from parent in sent.DescendantsSubsentence()
                         where parent.AttributeTypeAreEqual( SubsentenceType.Subordinate,
                                                             SubsentenceType.Default, 
                                                             SubsentenceType.Introductory )

                         from child in parent.ElementsSubsentence()
                         where child.AttributeTypeAreEqual( SubsentenceType.Subordinate,
                                                            SubsentenceType.Default, 
                                                            SubsentenceType.Introductory )

                         where ( parent.IsSubsentNotHasSubject() && 
                                 child .IsSubsentNotHasSubject() )

                         select new SubsentPair( parent, child );
            return (tuples);
        }

        protected abstract bool IsParentMatchCondition( SubsentPair pair, Language language, out IEnumerable< SubjectData > subjectData );
        protected abstract bool IsChildMatchCondition( SubsentPair pair, Language language );

        public virtual SubjectObjectsTuple Process( SubsentPair pair, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
        #region [.condition.]
            var subjects = default(IEnumerable< SubjectData >);
            if ( !IsChildMatchCondition( pair, language ) ||
                 !IsParentMatchCondition( pair, language, out subjects ) 
                 )
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
            var subsents = pair.CHILD.AfterAndSelfSubsents().ToArray();
            var objects = default(XElement[]);
            //if-RightExtremeElement-rights-then-pair.CHILD
            if ( subjects.Last().RightExtremeElement.IsAfter( pair.CHILD ) )
            {
                objects = pair.CHILD.AfterSubsents().TryAllocateObjectsInSubsents( objectAllocateMethod, id ).ToArray();
            }
            else                
            {
                objects = subsents.TryAllocateObjectsInSubsents( objectAllocateMethod, id ).ToArray();
            }

            //mark begin-end IndirectSpeech-subsents
            subsents.SetAttributeINDIRECTSPEECH_BEGINEND_4Subsents( directAndIndirectSpeechGlobalNumber );

            //add 2 result
            return (new SubjectObjectsTuple( subjects, objects, Id ));
        #endregion
        }
    }
    //===================================================//

    internal class IS_ParentChild_01 : IS_ParentChildBase
    {
        public IS_ParentChild_01() : base()
        {
        }

        protected override bool IsParentMatchCondition( SubsentPair pair, Language language, out IEnumerable< SubjectData > subjectData )
        {
            throw new InvalidOperationException();
        }
        protected override bool IsChildMatchCondition( SubsentPair pair, Language language )
        {
            throw new InvalidOperationException();
        }

        private bool IsParentMatchCondition( SubsentPair pair, Language language )
        {
            return (inderectspeech2.IsConditionPrecede( pair.CHILD ));
        }
        private bool IsChildMatchCondition( SubsentPair pair, Language language, out IEnumerable< SubjectData > subjectData )
        {
            subjectData = null;

            var subsent    = pair.CHILD;
            var resultList = default(List< SubjectData >);

            var sd = pattern1.GetSubjectData( subsent, language );

            sd.Add2List( ref resultList );

            if ( resultList.AnyEx() )
            {
                resultList = resultList.MakeDistinct();

                subjectData = resultList;
                return (true);
            }

            return (false);
        }        

        public override SubjectObjectsTuple Process( SubsentPair pair, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
        #region [.condition.]
            var subjects = default(IEnumerable< SubjectData >);
            if ( !IsParentMatchCondition( pair, language ) || 
                 !IsChildMatchCondition( pair, language, out subjects ) )
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
            var subsents = pair.CHILD.BeforeSubsents().ToArray();
            var objects  = subsents.TryAllocateObjectsInSubsents( objectAllocateMethod, id, pair.CHILD );

            //mark begin-end IndirectSpeech-subsents
            subsents.SetAttributeINDIRECTSPEECH_BEGINEND_4Subsents( directAndIndirectSpeechGlobalNumber, pair.CHILD );

            //add 2 result
            return (new SubjectObjectsTuple( subjects, objects, Id ));
        #endregion
        }
    }

    internal class IS_ParentChild_02 : IS_ParentChildBase
    {
        public IS_ParentChild_02() : base()
        {
        }

        protected override bool IsParentMatchCondition( SubsentPair pair, Language language, out IEnumerable< SubjectData > subjectData )
        {
            subjectData = null;

            var resultList = default(List< SubjectData >);
            var sd         = default(SubjectData);

            sd = pattern1.GetSubjectData( pair.PARENT, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern2.GetSubjectData( pair.PARENT, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern6.GetSubjectData( pair.PARENT, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern3_pointview.Instance.GetSubjectData( pair.PARENT, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern4_opinion  .Instance.GetSubjectData( pair.PARENT, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern5_onglance .Instance.GetSubjectData( pair.PARENT, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;

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
        protected override bool IsChildMatchCondition( SubsentPair pair, Language language )
        {
            return (inderectspeech2.IsConditionPrecede( pair.CHILD ) &&
                    inderectspeech.IsCondition( pair.CHILD ));
        }        
    }

    internal class IS_ParentChild_03 : IS_ParentChildBase
    {
        public IS_ParentChild_03() : base()
        {
        }

        protected override bool IsParentMatchCondition( SubsentPair pair, Language language, out IEnumerable< SubjectData > subjectData )
        {
            subjectData = null;

            var resultList = default(List< SubjectData >);
            var sd         = default(SubjectData);
             
            sd = pattern7 .GetSubjectData( pair.PARENT, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern8 .GetSubjectData( pair.PARENT, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern9 .GetSubjectData( pair.PARENT, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern10.GetSubjectData( pair.PARENT, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern11.GetSubjectData( pair.PARENT, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern18.GetSubjectData( pair.PARENT, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern19.GetSubjectData( pair.PARENT, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern20.GetSubjectData( pair.PARENT, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern21.GetSubjectData( pair.PARENT, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern22.GetSubjectData( pair.PARENT, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern23.GetSubjectData( pair.PARENT, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;
            sd = pattern24.GetSubjectData( pair.PARENT, language, pair.CHILD ); if ( sd.IsNotNull() ) goto NEXT;

        NEXT:
            sd.Add2List( ref resultList );

            if ( resultList.AnyEx() )
            {
                /*
                Это касается шаблонов:
                    IS_ParentChild_03
                    IS_ParentChild_04
                    IS_ParentChild_05
                    IS_ParentChild_06

                Если в этих шаблонах во втором сабсенте (дочернем, типа subordinate) есть cet@link=N, 
                который показывает куда-то вне этого же предложения с link, (в [::inderectspeech3,4,5,6::]) 
                то эти пары предложений не обрабатывать.
                */
                if ( !HasCetWithAttributeLinkPoint2Out( pair.CHILD ) )
                {
                    resultList = resultList.MakeDistinct();

                    subjectData = resultList;
                    return (true);
                }
            }

            return (false);
        }
        protected override bool IsChildMatchCondition( SubsentPair pair, Language language )
        {
            return (inderectspeech2.IsConditionPrecede( pair.CHILD ) &&
                    inderectspeech3.IsCondition( pair.CHILD, language ));
        }

        private bool HasCetWithAttributeLinkPoint2Out( XElement subsent )
        {subsent.ThrowIfNotSubsent();

            var cetLinks = from c in subsent.DescendantsAndSelfCet()
                           let link = c.GetAttributeLINK()
                           where link.HasValue
                           select link;
            if ( cetLinks.Any() )
            {
                foreach ( var cl in cetLinks )
                {
                    var e = subsent.FindEntityById_4Subsent( cl.Value );
                    if ( e.IsNotNull() && e.Ancestors().Contains( subsent ) )
                    {
                        return (false);
                    }
                }
                return (true);
            }
            return (false);
        }
    }

    internal class IS_ParentChild_04 : IS_ParentChild_03
    {
        public IS_ParentChild_04() : base()
        {
        }

        protected override bool IsChildMatchCondition( SubsentPair pair, Language language )
        {
            return (inderectspeech2.IsConditionPrecede( pair.CHILD ) &&
                    inderectspeech4.IsCondition( pair.CHILD, language ));
        }        
    }

    internal class IS_ParentChild_05 : IS_ParentChild_03
    {
        public IS_ParentChild_05() : base()
        {
        }

        protected override bool IsChildMatchCondition( SubsentPair pair, Language language )
        {
            return (inderectspeech2.IsConditionPrecede( pair.CHILD ) &&
                    inderectspeech5.IsCondition( pair.CHILD, language ));
        }        
    }

    internal class IS_ParentChild_06 : IS_ParentChild_03
    {
        public IS_ParentChild_06() : base()
        {
        }

        protected override bool IsChildMatchCondition( SubsentPair pair, Language language )
        {
            return (inderectspeech2.IsConditionPrecede( pair.CHILD ) &&
                    inderectspeech6.IsCondition( pair.CHILD, language ));
        }        
    }
    //===================================================//

    internal class IS_ParentChildGroup : IS_GroupBase< IS_ParentChildBase, IS_ParentChildBase.SubsentPair >
    {
        public static IS_ParentChildBase[] Get()
        {
            return
            (
                new IS_ParentChildBase[]
                { 
                    new IS_ParentChild_01(),
                    new IS_ParentChild_02(),
                    new IS_ParentChild_03(),
                    new IS_ParentChild_04(),
                    new IS_ParentChild_05(),
                    new IS_ParentChild_06(),
                }
            );
        }

        public IS_ParentChildGroup() : base( Get() )
        {
        }

        protected override IS_ParentChildBase.SubsentPair[] GetEssence4Processing(XElement sent)
        {
            return (IS_ParentChildBase.GetSubsentPairs( sent ).ToArray());
        }
        protected override SubjectObjectsTuple ProcessEssence(IS_ParentChildBase pattern, IS_ParentChildBase.SubsentPair essence, Language language
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
