using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;
using Utils;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// Класс для кодирования/декодирования юнитов
	/// </summary>
	public abstract class UnitCoderBase
	{
		/// <summary>
		/// Закодированный юнит
		/// </summary>
		protected struct CodedUnit
		{
			/// <summary>
			/// Юнит
			/// </summary>
			public UnitTextBase Unit { get; set; }
			/// <summary>
			/// Позиция
			/// </summary>
			public int Position { get; set; }
		}

		/// <summary>
		/// Коллекция закодированных юнитов
		/// </summary>
		protected List<CodedUnit> _codedUnits;

		/// <summary>
		/// Кодирование заданной коллекции юнитов
		/// </summary>
		/// <param name="unitCollection">коллекция юнитов</param>
		/// <returns>закодированная строка</returns>
		public string CodeUnits(IEnumerable<UnitTextBase> unitCollection)
		{
			_codedUnits = new List<CodedUnit>();
			SimpleStringBuilder result = new SimpleStringBuilder();
			foreach (UnitTextBase unit in unitCollection)
			{
				bool isNotEmpty = !unit.IsEmptyText();
				if (isNotEmpty)
					result.Append(' ');
				_codedUnits.Add(new CodedUnit() { Unit = unit, Position = result.Length });
				if (isNotEmpty)
					result.Append(CodeUnit(unit));
			}
			return result.LowerValue;
		}

		/// <summary>
		/// Получение коллекции юнитов, принадлежащих заданном интервалу
		/// </summary>
		/// <param name="startIndex"></param>
		/// <param name="length"></param>
		/// <returns>коллекция юнитов</returns>
		public UnitTextBase[] GetUnits(int startIndex, int length)
		{
			List<UnitTextBase> result = new List<UnitTextBase>();
			foreach (var unit in _codedUnits)
			{
				bool isChainFirstUnit = (startIndex == unit.Position) && !unit.Unit.IsEmptyText();
				bool isUnitInsideChain = (startIndex < unit.Position) && (unit.Position < startIndex + length);
				if (isChainFirstUnit || isUnitInsideChain)
					result.Add(unit.Unit);
			}
			return result.ToArray();
		}

		/// <summary>
		/// Кодирование заданного юнита
		/// </summary>
		/// <param name="unit">юнит</param>
		/// <returns>закодированное значение</returns>
		protected abstract string CodeUnit(UnitTextBase unit);
	}
}
