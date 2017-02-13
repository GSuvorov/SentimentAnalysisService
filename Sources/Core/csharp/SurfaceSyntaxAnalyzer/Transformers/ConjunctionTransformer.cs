using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// Преобразователь, который выделяет союзы
	/// </summary>
	internal class ConjunctionTransformer : UnitCollectionTransformerBase
	{
		#region [UnitCollectionTransformerBase]
		protected override bool IsTransformAvailableUnit(UnitTextBase unit, IList<UnitTextBase> sequence)
		{
			if (unit == null)
				throw new ArgumentNullException();
			if ((unit.UnitTextType == UnitTextType.U) || (unit.UnitTextType == UnitTextType.CET))
				return true;
			else if (unit.UnitTextType == UnitTextType.ENTITY)
			{
				Entity entity = (Entity)unit;
				return entity.IsType(EntityType.Pronoun) || entity.IsType(EntityType.Adverb);
			}
			return false;
		}

		protected override IEnumerable<UnitTextBase> TransformUnitsInSequence(IList<UnitTextBase> units)
		{
			List<UnitTextBase> result = new List<UnitTextBase>();

			var conjunctionList = DictionaryResource.SelectConjunctions(units);
			int currentConjunctionIndex = 0;
			Conjunction currentConjunction = GetCurrentConjunction(conjunctionList, currentConjunctionIndex);
			ConjunctionBuilder builder = new ConjunctionBuilder();
			foreach (UnitTextBase unit in units)
			{
				int position = 0;
				while (currentConjunction != null)
				{
					if ((currentConjunction.StartPosition < unit.PositionInfo.End) && (builder.Conjunction == null))
					/// начало союза
					{
						builder.Conjunction = currentConjunction;
						position = builder.AddLeftConjunctionBorder(result, unit, position);
					}
					if (currentConjunction.EndPosition <= unit.PositionInfo.End)
					/// конец союза
					{
						position = builder.AddAndClearConjunction(result, unit, position);
						currentConjunction = GetCurrentConjunction(conjunctionList, ++currentConjunctionIndex);
						continue;
					}
					if (builder.Conjunction != null)
						position = builder.AppendConjunctionPart(unit.Text, position);
					break;
				}
				AddUnmarkedPart(result, unit, position);
			}
			return result;
		}
		#endregion

		/// <summary>
		/// Добавление в заданную коллекцию юнитов неразмеченной части юнита
		/// </summary>
		/// <param name="unitCollection">коллекция юнитов</param>
		/// <param name="unit">юнит, часть которого добавляется в коллекцию</param>
		/// <param name="startPosition">начальная позиция, с которой надо добавить неразмеченную часть</param>
		private void AddUnmarkedPart(List<UnitTextBase> unitCollection, UnitTextBase unit, int startPosition)
		{
			if (startPosition == 0)
				unitCollection.Add(unit);
			else if (startPosition < unit.Text.Length)
			{
				if (unit.IsEntity)
					throw new ArgumentException("unit is Entity");
				unitCollection.Add(unit.CreateUnmarkedText(startPosition, unit.Text.Length));
			}
		}

		/// <summary>
		/// Получение текущего союза
		/// </summary>
		/// <param name="list">коллекция союзов</param>
		/// <param name="index">индекс текущего союза</param>
		/// <returns>текущий союз</returns>
		private Conjunction GetCurrentConjunction(IList<Conjunction> list, int index)
		{
			return index < list.Count ? list[index] : null;
		}
	}
}
