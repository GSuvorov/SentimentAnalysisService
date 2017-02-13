using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

using TextMining.Core;

namespace OpinionMining
{
    internal class DirectSpeech
    {
        public DirectSpeech( Language language )
        {
            Language = language;

            OneCitation         = new DS_OneCitationGroup();
            TwoCitation         = new DS_TwoCitationGroup();
            TwoSubsent          = new DS_TwoSubsentGroup();
            TwoSubsent_2        = new DS_TwoSubsentGroup_2();
            TwoSubsent_Ex       = new DS_TwoSubsent_ExGroup();
            ThreeSubsent        = new DS_ThreeSubsentGroup();
        }

        private Language Language
        {
            get;
            set;
        }

        private DS_OneCitationGroup         OneCitation
        {
            get;
            set;
        }
        private DS_TwoCitationGroup         TwoCitation
        {
            get;
            set;
        }
        private DS_TwoSubsentGroup          TwoSubsent
        {
            get;
            set;
        }
        private DS_TwoSubsentGroup_2        TwoSubsent_2
        {
            get;
            set;
        }
        private DS_TwoSubsent_ExGroup       TwoSubsent_Ex
        {
            get;
            set;
        }
        private DS_ThreeSubsentGroup        ThreeSubsent
        {
            get;
            set;
        }

        public IList< SubjectObjectsTuple > Process( XElement sent, ref int directAndIndirectSpeechGlobalNumber, ObjectAllocateMethod objectAllocateMethod )
        {
            sent.ThrowIfNotSent();

            var opinionMiningTuples = new List< SubjectObjectsTuple >();

                ThreeSubsent.Process( sent, Language, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod )
                            .AddRangeTo( opinionMiningTuples );
                            if ( opinionMiningTuples.AnyEx() ) return (opinionMiningTuples);

                TwoSubsent_Ex.Process( sent, Language, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod )
                             .AddRangeTo( opinionMiningTuples );
                            if ( opinionMiningTuples.AnyEx() ) return (opinionMiningTuples);

                TwoSubsent.Process( sent, Language, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod )
                          .AddRangeTo( opinionMiningTuples );
                            if ( opinionMiningTuples.AnyEx() ) return (opinionMiningTuples);
                TwoSubsent_2.Process( sent, Language, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod )
                          .AddRangeTo( opinionMiningTuples );
                            if ( opinionMiningTuples.AnyEx() ) return (opinionMiningTuples);

                TwoCitation.Process( sent, Language, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod )
                           .AddRangeTo( opinionMiningTuples );
                            if ( opinionMiningTuples.AnyEx() ) return (opinionMiningTuples);

                OneCitation.Process( sent, Language, ref directAndIndirectSpeechGlobalNumber, objectAllocateMethod )
                           .AddRangeTo( opinionMiningTuples );
                            if ( opinionMiningTuples.AnyEx() ) return (opinionMiningTuples);

            return (opinionMiningTuples);
        }
    }
    //===================================================//

    internal class CitationQuotes
    {
        public CitationQuotes( XElement quote1, XElement quote2 )
        {
            quote1.ThrowIfNull("quote1");
            quote2.ThrowIfNull("quote2");

            Quote1 = quote1;
            Quote2 = quote2;
        }
        public XElement Quote1
        {
            get;
            private set;
        }
        public XElement Quote2
        {
            get;
            private set;
        }

        public IList< XElement > ElementsBetween()
        {
            var list = new List< XElement >();
            foreach ( var e in Quote1.ElementsAfterSelf() )
            {
                if ( e != Quote2 )
                    list.Add( e );

                if ( e.DescendantsAndSelfCet().Contains( Quote2 ) )
                    return (list); 
            }

            foreach ( var ae in Quote1.Parent.ElementsAfterSelf() )
            foreach ( var e in ae.Elements() )
            {
                if ( e != Quote2 )
                    list.Add( e );

                if ( e.DescendantsAndSelfCet().Contains( Quote2 ) )
                    return (list); 
            }
            return (list);
        }

        public static CitationQuotes GetAfter( XElement startElement )
        {
            /*
            2. Для шаблонов <OneCitation> и <TwoCitation> уровни всех кавычек приравниваются к уровню подпредложения, в котором они находятся (т.е. если кавычка находится в дочернем элементе, то ее уровень приравнивается к родительскому). Таким образом,
            3. Открывающая кавычка по отношению к шаблонам {ENTITY - VERB}/{VERB - ENTITY} - всегда в пределах того же сабсента, где найден паттерн{ENTITY и ENTITY}, {ENTITY и VERB},{ VERB и ENTITY },{VERB и VERB}.
            4. Закрывающая кавычка может быть на том же или более низком уровне относительно сабсента, в котором находится открывающая кавычка.
            */
            var subsent = startElement.GetAncestorSubsent();

            var quote1 = subsent.Elements().SkipWhile( _ => !_.DescendantsAndSelf().Contains( startElement ) )
                                           .Skip( 1 )
                                           .FirstOrDefault( e => e.IsQuote() );
            if ( quote1 == null )
                return (null);

            /*
            var quote1 = startElement.ElementsAfterSelfCET().FirstOrDefault( e => e.IsQuote() );
            if ( quote1 == null )
                return (null);
            */
            var quote2 = (from e in quote1.ElementsAfterSelf()
                          from d in e.DescendantsAndSelfCet()
                          where d.IsQuote()
                          select d
                         ).FirstOrDefault();
            if ( quote2 == null )
                return (null);

            return (new CitationQuotes( quote1, quote2 ));
        }
        public static CitationQuotes GetBefore( XElement startElement )
        {
            var quote1 = startElement.ElementsBeforeSelfCET().LastOrDefault( e => e.IsQuote() );
            if ( quote1 == null )
                return (null);

            var quote2 = (from a in quote1.AncestorsAndSelf().TakeWhile( _ => !_.IsElementSENT() )
                          from e in a.ElementsBeforeSelf()
                          from d in e.DescendantsAndSelfCet()
                          where d.IsQuote()
                          select d
                         ).FirstOrDefault();
            if ( quote2 == null )
                return (null);

            return (new CitationQuotes( quote2, quote1 ));
        }
    }
    //===================================================//
}



