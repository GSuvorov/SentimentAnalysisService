using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;
using Linguistics.Core.Morpho;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// Преобразователь, который выделяет составные глаголы
	/// </summary>
	internal class CompoundVerbTransformer : UnitCollectionTransformerBase
	{
		#region [UnitCollectionTransformerBase]
		protected override bool IsTransformAvailableUnit(UnitTextBase unit, IList<UnitTextBase> sequence)
		{
			bool isVerbOrDeepr = false;
			bool isAdverb = false;
			if (unit.IsEntity)
			{
				Entity entity = (Entity)unit;
				isVerbOrDeepr = entity.IsType(EntityType.Verb) || entity.IsType(EntityType.Infinitive) || entity.IsType(EntityType.Deepr);
				isAdverb = entity.IsType(EntityType.Adverb);
			}
			return isVerbOrDeepr || (sequence.Any() && (isAdverb || unit.IsEmptyText()));
		}

		protected override IEnumerable<UnitTextBase> TransformUnitsInSequence(IList<UnitTextBase> units)
		{
			if (IsNeedCreateCompoundVerb(units))
			{
				List<UnitTextBase> result = new List<UnitTextBase>();

				List<UnitTextBase> simpleUnits = new List<UnitTextBase>();
				/// первый элемент должен быть глаголом
				Entity compoundVerb = (Entity)units[0];
				foreach (UnitTextBase unit in units.Skip(1))
				{
					if (unit.IsEntity)
					{
						Entity entity = (Entity)unit;
						if (entity.IsType(EntityType.Verb) || entity.IsType(EntityType.Infinitive))
						{
							AddSimpleUnitsToCompound(compoundVerb, simpleUnits);
							AddVerbToCompound(ref compoundVerb, (Entity)unit);
							continue;
						}
					}
					simpleUnits.Add(unit);
				}

				SetCompounVerbMorpho(compoundVerb);
				result.Add(compoundVerb);
				result.AddRange(simpleUnits);

				return result;
			}
			else
				return units;
		}
		#endregion

		/// <summary>
		/// Добавление заданного глагола к составному
		/// </summary>
		/// <param name="parentVerb">родительский глагол</param>
		/// <param name="verb">глагол</param>
		private void AddVerbToCompound(ref Entity parentVerb, Entity verb)
		{
			#region [.defense.]
			if (!(verb.IsType(EntityType.Verb) || verb.IsType(EntityType.Infinitive)))
				throw new ArgumentException("entity isn't Verb");
			#endregion
            if (parentVerb.IsType(EntityType.Infinitive) && !verb.IsType(EntityType.Infinitive))
			{
				verb.AppendChildToBegin(parentVerb);
				parentVerb = verb;
			}
			else
				parentVerb.AppendChild(verb);
		}

		/// <summary>
		/// Добавление коллекии юнитов к составному глаголу
		/// </summary>
		/// <param name="verb">составной глагол</param>
		/// <param name="units">коллекция юнитов неразмеченного текста</param>
		private void AddSimpleUnitsToCompound(Entity verb, IList<UnitTextBase> units)
		{
			foreach (UnitTextBase unit in units)
				verb.AppendChild(unit);
			units.Clear();
		}

		/// <summary>
		/// Проверка, что из заданной коллекции юнитов необходимо создать составной глагол
		/// </summary>
		/// <param name="units">коллекция юнитов</param>
		/// <returns>результат проверки</returns>
		/// составной глагол создаем, когда есть только один глагол неинфинитив
		private bool IsNeedCreateCompoundVerb(IList<UnitTextBase> units)
		{
			/// в цепочке встретился неинфинитив
			bool isNonInfinitive = false;
			/// встретилась форма глагола "быть"
			bool isBe = false;
			foreach (UnitTextBase unit in units)
			{
				if (unit.IsEntity)
				{
					Entity entity = (Entity)unit;
					if (entity.IsType(EntityType.Deepr) || entity.IsType(EntityType.Verb))
					{
						if (IsVerbBe(entity))
							isBe = true;
						else if (isNonInfinitive)
							return false;
						else
							isNonInfinitive = true;
					}
				}
			}
			return isNonInfinitive || isBe;
		}

		/// <summary>
		/// Проверка, что заданная сущность - глагол "быть"
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <returns>результат проверки</returns>
		private bool IsVerbBe(Entity entity)
		{
			return entity.Value.ToLower() == "быть";
		}

		/// <summary>
		/// Установка морфологических свойств составного глагола
		/// </summary>
		/// <param name="compounVerb">составной глагол</param>
		private void SetCompounVerbMorpho(Entity compounVerb)
		{
			if (IsVerbBe(compounVerb) && ContainPassiveChild(compounVerb))
			{
				foreach (var morpho in compounVerb.EntityInfo.Morpho)
					morpho.Voice = Voice.Passive;
			}
		}

		/// <summary>
		/// Проверка, что заданный глагол имеет дочерний глагол в страдательном залоге
		/// </summary>
		/// <param name="verb">глагол</param>
		/// <returns>результат проверки</returns>
		private bool ContainPassiveChild(Entity verb)
		{
			foreach (var child in verb.Children)
			{
				if (child.IsType(EntityType.Verb) &&
					(child.EntityInfo.Morpho.Any(_ => _.Voice == Voice.Passive) || ContainPassiveChild(child)))
				{
					return true;
				}
			}
			return false;
		}
	}
}
