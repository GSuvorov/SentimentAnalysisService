using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;
using Linguistics.Core.Morpho;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// Декоратор ролей
	/// </summary>
	internal class RoleDecorator : SyntaxAnalyzerDecoratorBase
	{
		#region [.ctor().]
		public RoleDecorator(SyntaxAnalyzerBase component)
			: base(component)
		{
		}
		#endregion

		#region [SyntaxAnalyzerBase]
		public override SubSentence[] AnalyzeSubSentence(SubSentence subSentence)
		{
			var result = base.AnalyzeSubSentence(subSentence);
			result.ForAllHierarchy(SetRole);
			return result;
		}
		#endregion

		/// <summary>
		/// Установка ролей в заданном подпредложении
		/// </summary>
		/// <param name="subSentence">подпредложение</param>
		private void SetRole(SubSentence subSentence)
		{
			foreach (UnitTextBase unit in subSentence.Units)
			{
				if (unit.IsEntity)
				{
					Entity entity = (Entity)unit;
					if (entity.IsSentenceMember())
						SetEntityRole(entity);
					else if (entity.IsCompoundVerb())
						SetCompoundVerbRole(entity);
				}
			}
		}

		/// <summary>
		/// Установка роли составного глагола
		/// </summary>
		/// <param name="compoundVerb">составной глагол</param>
		private void SetCompoundVerbRole(Entity compoundVerb)
		{
			foreach (Entity child in compoundVerb.Children)
			{
				if (child.IsSentenceMember())
					SetEntityRole(child);
				else if (child.IsCompoundVerb())
					SetCompoundVerbRole(child);
			}
		}

		/// <summary>
		/// Установка роли для заданной сущности
		/// </summary>
		/// <param name="entity">сущность</param>
		private void SetEntityRole(Entity entity)
		{
			if (!(entity.IsType(EntityType.Homogeneous) && SetRoleForHomogeneous(entity)))
			{
                if (entity.ParentObject.ParentObject.Language == Language.Russian)
                {
                    if (entity == entity.ParentObject.Subject)
                        entity.Role = EntityRole.Subj;
                    else if (entity.IsType(EntityType.Adverb))
                        entity.Role = EntityRole.Adverb;
                    else if (entity.IsType(EntityType.Adjective) || entity.IsType(EntityType.ShortAdjective) ||
                        entity.IsType(EntityType.Comparative) || entity.IsType(EntityType.Superlative))
                    {
                        entity.Role = EntityRole.Adjective;
                    }
                    else if (entity.IsType(EntityType.Date) || IsPreviousPretext(entity))
                        entity.Role = EntityRole.Adj;
                    else if (entity.IsPossibleSubject())
                        SetRoleForPossibleSubject(entity);
                    else if (entity.IsContainCases(Case.Dative, Case.Accusative))
                        entity.Role = EntityRole.Obj;
                    else
                        entity.Role = EntityRole.Adj;
                }
			}
			SetRolesForChildren(entity);
		}

		/// <summary>
		/// Установка роли для цепочки однородных
		/// </summary>
		/// <param name="homogeneous">цепочка однородных</param>
		/// <returns>флаг установки роли</returns>
		private bool SetRoleForHomogeneous(Entity homogeneous)
		{
			#region [.defense.]
			if (!homogeneous.IsType(EntityType.Homogeneous))
				throw new ArgumentException("entity isn't homogeneous");
			#endregion
			Entity firstChild = homogeneous.Children.First();
			if (firstChild.IsType(EntityType.Adverb))
				homogeneous.Role = EntityRole.Adverb;
            else if (firstChild.IsType(EntityType.Adjective) ||
                firstChild.IsType(EntityType.Comparative) ||
                firstChild.IsType(EntityType.Superlative))
            {
                homogeneous.Role = EntityRole.Adjective;
            }
            else
                return false;
			return true;
		}

		/// <summary>
		/// Установка роли для дочерних сущностей
		/// </summary>
		/// <param name="parent">родительская сущность</param>
		private void SetRolesForChildren(Entity parent)
		{
			foreach (var child in parent.Children)
			{
				if (child.IsSentenceMember())
				{
					if (parent.IsType(EntityType.Homogeneous))
						child.Role = parent.Role;
					else if (child.UnitTextType == UnitTextType.ENTITY)
						child.Role = EntityRole.Other;
					SetRolesForChildren(child);
				}
			}
		}

		/// <summary>
		/// Установка роли для сущности-потенциального подлежащего
		/// </summary>
		/// <param name="entity">сущность-потенциальное подлежащее</param>
		private void SetRoleForPossibleSubject(Entity entity)
		{
			if (!IsVerbBetweenEntityAndSubject(entity))
				entity.Role = EntityRole.Subj;
			else
				entity.Role = EntityRole.Obj;
		}

		/// <summary>
		/// Проверка, что между заданной сущностью и подлежащим ее подпредложения есть глагол
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <returns>результат проверки</returns>
		private bool IsVerbBetweenEntityAndSubject(Entity entity)
		{
			SubSentence subSentence = entity.ParentObject;
			UnitTextBase first = null;
			foreach (UnitTextBase unit in subSentence.Units)
			{
				if (unit == subSentence.Subject)
				{
					if (first == null)
						first = unit;
					else
						return false;
				}
				else if (unit == entity)
				{
					if (first == null)
						first = unit;
					else
						return false;
				}
				else if ((first != null) && unit.IsEntity)
				{
					Entity ent = (Entity)unit;
					if (ent.IsType(EntityType.Verb))
						return true;
				}
			}
			return true;
		}

		/// <summary>
		/// Проверка, что перед сущностью стоит предлог
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <returns>результат проверки</returns>
		private bool IsPreviousPretext(Entity entity)
		{
			if (entity.IsType(EntityType.Homogeneous))
				return ((HomogeneousEntityInfo)entity.EntityInfo).Pretext != null;
			else
				return entity.GetNonEmptyPrevious().IsPretext();
		}
	}
}
