using Linguistics.Core;
using Linguistics.Core.Morpho;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SurfaceSyntaxAnalyzer
{
    /// <summary>
    /// Декоратор для выделения пассивных субъектов
    /// </summary>
    internal class PassiveSubjectDecorator : SyntaxAnalyzerDecoratorBase
    {
        #region [.ctor().]
        public PassiveSubjectDecorator(SyntaxAnalyzerBase component)
			: base(component)
		{
		}
		#endregion

        #region [SyntaxAnalyzerBase]
        public override SubSentence[] AnalyzeSubSentence(SubSentence subSentence)
        {
            var result = base.AnalyzeSubSentence(subSentence);
            result.ForAllHierarchy(FindPassiveSubject);
            return result;
        }
        #endregion

        /// <summary>
        /// Поиск пассивного субъекта в подпредложении
        /// </summary>
        /// <param name="subSentence">подпредложение</param>
        private void FindPassiveSubject(SubSentence subSentence)
        {
            if (subSentence.Subject == null)
                return;

            /// косвенное дополнение
            Entity indirectObjective = null;
            /// глагол в пассивном залоге
            Entity passiveVerb = null;
            foreach (var unit in subSentence.Units)
            {
                if (unit.IsEntity && (unit != subSentence.Subject))
                {
                    Entity entity = (Entity)unit;
                    if (entity.IsType(EntityType.Verb) && IsPassiveVerb(entity))
                        passiveVerb = entity;
                    else if (entity.IsType(EntityType.OrgName) || entity.IsType(EntityType.OrgNoInfo) ||
                        IsDativeOrInstrumentalAnimationEntity(entity))
                    {
                        indirectObjective = entity;
                    }
                }
            }
            if (passiveVerb != null)
                subSentence.IsPassiveSubject = indirectObjective != null;
        }

        /// <summary>
        /// Проверка, что заданная сущность - одушевленное в дательном или творительном падеже
        /// </summary>
        /// <param name="entity">сущность</param>
        /// <returns>результат проверки</returns>
        private bool IsDativeOrInstrumentalAnimationEntity(Entity entity)
        {
            if (entity.EntityInfo.Morpho != null)
            {
                foreach (var morpho in entity.EntityInfo.Morpho)
                {
                    if (((morpho.Case == Case.Dative) || (morpho.Case == Case.Instrumental)) &&
                        ((morpho.Animation == AnimationType.Animation) || entity.IsType(EntityType.Pronoun)))
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        /// <summary>
        /// Проверка, что заданный глагол в пассивном залоге
        /// </summary>
        /// <param name="verb">глагол</param>
        /// <returns>результат проверки</returns>
        private bool IsPassiveVerb(Entity verb)
        {
            #region [.defense.]
            if (!verb.IsType(EntityType.Verb))
                throw new ArgumentException("verb isn't Verb");
            #endregion

            if (verb.Value.ToLower() == "быть")
                return true;

            foreach (var morpho in verb.EntityInfo.Morpho)
                if (morpho.Voice == Voice.Passive)
                    return true;
            return false;
        }
    }
}
