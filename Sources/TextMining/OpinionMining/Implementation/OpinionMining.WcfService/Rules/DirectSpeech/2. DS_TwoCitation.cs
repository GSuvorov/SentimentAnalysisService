using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

namespace OpinionMining
{
    using OpinionMining.DirectIndirectSpeechExtensions;
    using TextMining.Core;

    internal abstract class DS_TwoCitationBase : RuleBase
    {
        public DS_TwoCitationBase( string id = null ) : base( id )
        {
        }

        public static IEnumerable< XElement > GetSubsents( XElement sent )
        {
            sent.ThrowIfNotSent();

            var subsents   = from subsent in sent.DescendantsSubsentence()
                             where subsent.AttributeTypeAreEqual( SubsentenceType.Subordinate,
                                                                  SubsentenceType.Default, 
                                                                  SubsentenceType.Introductory )

                             where subsent.IsSubsentNotHasObjectAndSubject() 

                             select subsent;
            return (subsents);
        }

        protected List< SubjectData > ExecPatterns( XElement subsent, Language language )
        {
            var resultList = default(List< SubjectData >);

            var subjectData1  = pattern12.GetSubjectData4Citation( subsent, language );
            var subjectData2  = pattern6 .GetSubjectData4Citation( subsent, language );
            var subjectData3  = pattern13.GetSubjectData4Citation( subsent, language );
            var subjectData4  = pattern15.GetSubjectData4Citation( subsent, language );
            var subjectData5  = pattern16.GetSubjectData4Citation( subsent, language );
            var subjectData6  = pattern17.GetSubjectData4Citation( subsent, language );
            var subjectData7  = pattern18.GetSubjectData4Citation( subsent, language );
            var subjectData8  = pattern19.GetSubjectData4Citation( subsent, language );
            var subjectData9  = pattern20.GetSubjectData4Citation( subsent, language );
            var subjectData10 = pattern21.GetSubjectData4Citation( subsent, language );
            var subjectData11 = pattern22.GetSubjectData4Citation( subsent, language );
            var subjectData12 = pattern23.GetSubjectData4Citation( subsent, language );
            var subjectData13 = pattern24.GetSubjectData4Citation( subsent, language );

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

                return (resultList);
            }

            return (null);
        }
        protected bool IsSubsentMatchCondition( XElement subsent, Language language, out IEnumerable< SubjectData > subjectData
                                                , out CitationQuotes quotePairLeft
                                                , out CitationQuotes quotePairRight )
        {
            quotePairLeft  = null;
            quotePairRight = null;
            subjectData    = null;
            
            var resultList = ExecPatterns( subsent, language );
            if ( resultList.IsNotNull() )
            {
                var leftExtremeElement = resultList.GetLeftExtremeElement();
                if ( (quotePairLeft = CitationQuotes.GetBefore( leftExtremeElement )) == null )
                {
                    return (false);
                }

                var rightExtremeElement = resultList.GetRightExtremeElement();
                if ( (quotePairRight = CitationQuotes.GetAfter( rightExtremeElement )) == null )
                {
                    return (false);
                }

                subjectData = resultList;
                return (true);
            }

            return (false);
        }

        public SubjectObjectsTuple Process( XElement subsent, Language language
            , ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {subsent.ThrowIfNotSubsent();

        #region [.condition.]
            var subjects       = default(IEnumerable< SubjectData >);
            var quotePairLeft  = default(CitationQuotes);
            var quotePairRight = default(CitationQuotes);
            if ( !IsSubsentMatchCondition( subsent, language, out subjects, out quotePairLeft, out quotePairRight ) )
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
            var elementsLeft  = quotePairLeft .ElementsBetween();
            var elementsRight = quotePairRight.ElementsBetween();
            var objects       = elementsLeft.TryAllocateObjectsInUnknownElements( objectAllocateMethod, id )
                               .Concat
                               (
                                elementsRight.TryAllocateObjectsInUnknownElements( objectAllocateMethod, id )
                               )
                               .ToArray();
                
            //mark begin-end DirectSpeech
            elementsLeft .SetAttributeDIRECTSPEECH_BEGINEND( directAndIndirectSpeechGlobalNumber );
            elementsRight.SetAttributeDIRECTSPEECH_BEGINEND( directAndIndirectSpeechGlobalNumber );

            //add 2 result
            return (new SubjectObjectsTuple( subjects, objects, Id ) );
        #endregion
        }
    }
    //===================================================//

    internal class DS_TwoCitation_01 : DS_TwoCitationBase
    {
        public DS_TwoCitation_01(): base()
        {
        }
    }
    //===================================================//

    internal class DS_TwoCitationGroup : DS_GroupBase< DS_TwoCitationBase, XElement >
    {
        private static DS_TwoCitationBase[] Get()
        {
            return
            (
                new DS_TwoCitationBase[]
                { 
                    new DS_TwoCitation_01(),
                }
            );
        }

        public DS_TwoCitationGroup() : base( Get() )
        {
        }

        protected override XElement[] GetEssence4Processing( XElement sent )
        {
            return (DS_TwoCitationBase.GetSubsents( sent ).ToArray());
        }
        protected override SubjectObjectsTuple ProcessEssence( DS_TwoCitationBase pattern, XElement essence, Language language
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
