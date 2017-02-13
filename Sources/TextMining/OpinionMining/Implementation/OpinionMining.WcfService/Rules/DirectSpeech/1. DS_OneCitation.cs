using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

namespace OpinionMining
{
    using OpinionMining.DirectIndirectSpeechExtensions;
    using TextMining.Core;

    internal abstract class DS_OneCitationBase : RuleBase
    {
        public DS_OneCitationBase( string id = null ) : base( id )
        {
        }

        public static IEnumerable< XElement > GetSubsents( XElement sent )
        {
            sent.ThrowIfNotSent();

            var subsents   = from subsent in sent.DescendantsSubsentence()
                             where subsent.AttributeTypeAreEqual( SubsentenceType.Subordinate,
                                                                  SubsentenceType.Default, 
                                                                  SubsentenceType.Introductory )

                             where subsent.IsSubsentNotHasObjectAndSubject() //.IsSubsentNoThereWasInRules()

                             select subsent;
            return (subsents);
        }

        protected List< SubjectData > ExecPatterns( XElement subsent, Language language )
        {
            var resultList = default(List< SubjectData >);

            var subjectData1  = pattern12.GetSubjectData4Citation( subsent, language );
            var subjectData2  = pattern13.GetSubjectData4Citation( subsent, language );
            var subjectData4  = pattern15.GetSubjectData4Citation( subsent, language );
            var subjectData5  = pattern16.GetSubjectData4Citation( subsent, language );
            var subjectData6  = pattern17.GetSubjectData4Citation( subsent, language );
            var subjectData7  = pattern6 .GetSubjectData4Citation( subsent, language );
            var subjectData8  = pattern2 .GetSubjectData4Citation( subsent, language );
            var subjectData9  = pattern5_onglance .Instance.GetSubjectData4Citation( subsent           );
            var subjectData10 = pattern4_opinion  .Instance.GetSubjectData4Citation( subsent, language );
            var subjectData11 = pattern3_pointview.Instance.GetSubjectData4Citation( subsent           );
            var subjectData12 = pattern18.GetSubjectData4Citation( subsent, language );
            var subjectData13 = pattern19.GetSubjectData4Citation( subsent, language );
            var subjectData14 = pattern20.GetSubjectData4Citation( subsent, language );
            var subjectData15 = pattern21.GetSubjectData4Citation( subsent, language );
            var subjectData16 = pattern22.GetSubjectData4Citation( subsent, language );
            var subjectData17 = pattern23.GetSubjectData4Citation( subsent, language );
            var subjectData18 = pattern24.GetSubjectData4Citation( subsent, language );

            subjectData1 .Add2List( ref resultList );
            subjectData2 .Add2List( ref resultList );
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
            subjectData14.Add2List( ref resultList );
            subjectData15.Add2List( ref resultList );
            subjectData16.Add2List( ref resultList );
            subjectData17.Add2List( ref resultList );
            subjectData18.Add2List( ref resultList );

            if ( resultList.AnyEx() )
            {
                resultList = resultList.MakeDistinct();

                return (resultList);
            }

            return (null);
        }
        protected abstract bool IsSubsentMatchCondition( XElement subsent, Language language, out IEnumerable< SubjectData > subjectData, out CitationQuotes quotePair );

        public SubjectObjectsTuple Process( XElement subsent, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {subsent.ThrowIfNotSubsent();

        #region [.condition.]
            var subjects  = default(IEnumerable< SubjectData >);
            var quotePair = default(CitationQuotes);
            if ( !IsSubsentMatchCondition( subsent, language, out subjects, out quotePair ) )
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
            var elements = quotePair.ElementsBetween();
            var objects  = elements.TryAllocateObjectsInUnknownElements( objectAllocateMethod, id );
                
            //mark begin-end DirectSpeech
            elements.SetAttributeDIRECTSPEECH_BEGINEND( directAndIndirectSpeechGlobalNumber );

            //add 2 result
            return (new SubjectObjectsTuple( subjects, objects, Id ) );
        #endregion
        }
    }
    //===================================================//

    internal class DS_OneCitation_01 : DS_OneCitationBase
    {
        public DS_OneCitation_01(): base()
        {
        }

        protected override bool IsSubsentMatchCondition( XElement subsent, Language language, out IEnumerable< SubjectData > subjectData, out CitationQuotes quotePair )
        {
            quotePair   = null;
            subjectData = null;
            
            var resultList = ExecPatterns( subsent, language );
            if ( resultList.IsNotNull() )
            {
                var rightExtremeElement = resultList.GetRightExtremeElement();
                if ( (quotePair = CitationQuotes.GetAfter( rightExtremeElement )) != null )
                {
                    subjectData = resultList;
                    return (true);
                }
            }

            return (false);
        }
    }

    internal class DS_OneCitation_02 : DS_OneCitationBase
    {
        public DS_OneCitation_02(): base()
        {
        }

        protected override bool IsSubsentMatchCondition( XElement subsent, Language language, out IEnumerable< SubjectData > subjectData, out CitationQuotes quotePair )
        {
            quotePair   = null;
            subjectData = null;
            
            var resultList = ExecPatterns( subsent, language );
            if ( resultList.IsNotNull() )
            {
                var leftExtremeElement = resultList.GetLeftExtremeElement();
                if ( (quotePair = CitationQuotes.GetBefore( leftExtremeElement )) != null )
                {
                    subjectData = resultList;
                    return (true);
                }
            }

            return (false);
        }
    }
    //===================================================//

    internal class DS_OneCitationGroup : DS_GroupBase< DS_OneCitationBase, XElement >
    {
        private static DS_OneCitationBase[] Get()
        {
            return
            (
                new DS_OneCitationBase[]
                { 
                    new DS_OneCitation_01(),
                    new DS_OneCitation_02(),
                }
            );
        }

        public DS_OneCitationGroup() : base( Get() )
        {
        }

        protected override XElement[] GetEssence4Processing( XElement sent )
        {
            return (DS_OneCitationBase.GetSubsents( sent ).ToArray());
        }
        protected override SubjectObjectsTuple ProcessEssence( DS_OneCitationBase pattern, XElement essence, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
            if ( essence.IsSubsentNotHasObjectAndSubject() )
            {
                return (pattern.Process( essence, language, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod ));
            }
            return (null);
        }
    }
}
