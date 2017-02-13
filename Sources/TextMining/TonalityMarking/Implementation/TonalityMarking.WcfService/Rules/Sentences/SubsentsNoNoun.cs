using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

namespace TonalityMarking
{
    internal class SubsentsNoNoun_One : OneInSubsentsRule
    {
        public SubsentsNoNoun_One( XElement ruleNode )
            : base( ruleNode )
        {
        }

        /*public override bool Process( XElement subsent )
        {
            subsent.ThrowIfNotSubsent();

            var result = false;
            foreach ( var pt in Patterns )
            {
                #region [.condition.]
                if ( (subsent.Name != pt.ParentName) ||
                      (!pt.ParentAttributeName.IsEmptyOrNull() &&
                       !subsent.HasAttributeWithValue( pt.ParentAttributeName, pt.ParentAttributeValue )) )
                    continue;

                var first = subsent.Elements( pt.FirstName )
                                .Where( _ => pt.FirstAttributeName.IsEmptyOrNull() ||
                                             _.HasAttributeWithValue( pt.FirstAttributeName, pt.FirstAttributeValue ) )
                                .FirstOrDefault();
                if ( first == null )
                    continue;

                var tuple = new 
                {
                    PARENT = subsent,
                    FIRST  = first,
                };
                #endregion

                #region [.satisfy-condition.]
                MatchRuleDebugInfoOutput( tuple.FIRST );

                var firstSNT = tuple.FIRST.Attribute("SNT");

                if ( firstSNT == null )
                    throw (new InvalidOperationException("Element no has 'SNT' attribute (rule " + this.Id.InSingleQuote() + ')'));

                var a1SNT = firstSNT.Value.ToEnum< TonalityType >();

                var localresult = false;
                foreach ( var a in Actions )
                {
                    if ( a.Exists( a1SNT ) )
                    {
                        #region
                        MatchActionDebugInfoOutput( a );
                        localresult = true;

                        result = true;

                        tuple.PARENT.SetAttributeSNT( a.SNT );
                        tuple.PARENT.SetAttributeFRT( tuple.FIRST .GetAttributeFRT() );

                        break;
                        #endregion
                    }
                }

                if ( !localresult )
                    NomatchActionDebugInfoOutput();
                #endregion

                if ( result )
                    break;
            }
            return (result);
        }*/
    }


    internal class SubsentsNoNoun_Pairs : PairsInSubsentsRule
    {
        public SubsentsNoNoun_Pairs( XElement ruleNode )
            : base( ruleNode )
        {
        }

        /*public override bool Process( XElement subsent )
        {
            subsent.ThrowIfNotSubsent();

            var result = false;
            foreach ( var pt in Patterns )
            {
                #region [.condition.]
                if ( (subsent.Name != pt.ParentName) ||
                      (!pt.ParentAttributeName.IsEmptyOrNull() &&
                       !subsent.HasAttributeWithValue( pt.ParentAttributeName, pt.ParentAttributeValue )) )
                    continue;

                var first = subsent.Elements( pt.FirstName )
                                .Where( _ => pt.FirstAttributeName.IsEmptyOrNull() ||
                                             _.HasAttributeWithValue( pt.FirstAttributeName, pt.FirstAttributeValue ) )
                                .FirstOrDefault();
                if ( first == null )
                    continue;

                var second = first.ElementsAfterSelf( pt.SecondName )
                                  .Where( _ => pt.SecondAttributeName.IsEmptyOrNull() ||
                                               _.HasAttributeWithValue( pt.SecondAttributeName, pt.SecondAttributeValue ) )
                                  .FirstOrDefault();
                if ( second == null )
                    continue;

                var tuple = new {
                                PARENT = subsent,
                                FIRST  = first,
                                SECOND = second,
                            };
                #endregion

                #region [.satisfy-condition.]
                MatchRuleDebugInfoOutput( tuple.FIRST );

                var firstSNT  = tuple.FIRST .Attribute("SNT");
                var secondSNT = tuple.SECOND.Attribute("SNT");

                if ( firstSNT == null || secondSNT == null )
                    throw (new InvalidOperationException("Element no has 'SNT' attribute (rule " + this.Id.InSingleQuote() + ')'));

                var a1SNT = firstSNT .Value.ToEnum< TonalityType >();
                var a2SNT = secondSNT.Value.ToEnum< TonalityType >();

                var localresult = false;
                foreach ( var a in Actions )
                {
                    if ( a.Exists( a1SNT, a2SNT ) )
                    {
                        #region
                        MatchActionDebugInfoOutput( a );
                        localresult = true;

                        result = true;

                        tuple.PARENT.SetAttributeSNT( a.SNT );
                        tuple.PARENT.SetAttributeFRT( tuple.FIRST .GetAttributeFRT() + 
                                                      tuple.SECOND.GetAttributeFRT() );

                        break;
                        #endregion
                    }
                }

                if ( !localresult )
                    NomatchActionDebugInfoOutput();
                #endregion

                if ( result )
                    break;
            }
            return (result);
        }*/
    }
}
