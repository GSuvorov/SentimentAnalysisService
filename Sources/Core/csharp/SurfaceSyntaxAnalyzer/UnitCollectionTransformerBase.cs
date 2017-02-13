using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// Класс для трансформации исходной коллекции юнитов в новую коллекцию юнитов
	/// </summary>
	abstract internal class UnitCollectionTransformerBase
	{
		/// <summary>
		/// Трансформация исходной коллекции юнитов  вновую
		/// </summary>
		/// <param name="units">исходная коллекция юнитов</param>
		/// <returns>новая коллекция юнитов</returns>
		public List<UnitTextBase> Transform(IEnumerable<UnitTextBase> units)
		{
			List<UnitTextBase> result = new List<UnitTextBase>();
			if (units.Any())
			{
				List<UnitTextBase> unitsInSequence = new List<UnitTextBase>();
				int position = units.First().PositionInfo.Start;
				foreach (var unit in units)
				{
					if ((unit.PositionInfo.Start == position) && IsTransformAvailableUnit(unit, unitsInSequence))
						unitsInSequence.Add(unit);
					else
					{
						TransformUnits(unitsInSequence, result);
						ChooseUnitTransformCollection(unit, unitsInSequence, result);
					}
					position = unit.PositionInfo.End;
				}
				if (unitsInSequence.Any())
					result.AddRange(TransformUnitsInSequence(unitsInSequence));
			}
			return result;
		}

		/// <summary>
		/// Трансформация юнитов из заданной коллекции
		/// </summary>
		/// <param name="units">коллекция юнитов для трансформации</param>
		/// <param name="transformedUnits">коллекция для трансформированных юнитов</param>
		private void TransformUnits(IList<UnitTextBase> units, List<UnitTextBase> transformedUnits)
		{
			if (units.Any())
			{
				transformedUnits.AddRange(TransformUnitsInSequence(units));
				units.Clear();
			}
		}

		/// <summary>
		/// Выбор для юнита коллекции трансформации
		/// </summary>
		/// <param name="unit">юнит</param>
		/// <param name="unitsForTransform">коллекция юнитов для трансформации</param>
		/// <param name="noTransformdUnits">коллекция нетранфсормируемых юнитов</param>
		private void ChooseUnitTransformCollection(UnitTextBase unit, List<UnitTextBase> unitsForTransform, List<UnitTextBase> noTransformdUnits)
		{
			if (IsTransformAvailableUnit(unit, unitsForTransform))
				unitsForTransform.Add(unit);
			else
				noTransformdUnits.Add(unit);
		}

		/// <summary>
		/// Проверка, что заданный юнит может быть трансформирован
		/// </summary>
		/// <param name="unit">юнит</param>
		/// <param name="sequence">текущая последовательность юнитов</param>
		/// <returns>результат проверки</returns>
		protected abstract bool IsTransformAvailableUnit(UnitTextBase unit, IList<UnitTextBase> sequence);

		/// <summary>
		/// Трансформация исходной коллекции юнитов в новую
		/// </summary>
		/// <param name="units">исходная коллекция юнитов</param>
		/// <returns>новая коллекция юнитов</returns>
		protected abstract IEnumerable<UnitTextBase> TransformUnitsInSequence(IList<UnitTextBase> units);
	}
}
