using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using SurfaceSyntaxAnalyzer;
using Linguistics.Core;
using System.Globalization;

namespace PTS
{
	/// <summary>
	/// Выражение для выделения интервала чисел
	/// </summary>
	internal abstract class NumberRangeExpressionBase
	{
		/// <summary>
		/// Регулярное выражение
		/// </summary>
		protected Regex Expression { get; private set; }

		#region [.ctor().]
		public NumberRangeExpressionBase(string expression)
		{
			Expression = new Regex(expression);
		}
		#endregion

		/// <summary>
		/// Выделение из текста интервалов чисел
		/// </summary>
		/// <param name="text">текст</param>
		/// <param name="coder">кодер юнитов</param>
		/// <returns>интервалы чисел</returns>
		public NumberRange[] GetNumberRanges(string text, UnitCoderBase coder)
		{
			List<NumberRange> result = new List<NumberRange>();
			var collection = Expression.Matches(text);
			for (int i = 0; i < collection.Count; ++i)
			{
				var range = GetNumberRange(collection[i], coder);
				var units = coder.GetUnits(collection[i].Index, collection[i].Length);
				range.StartPosition = units.First().PositionInfo.Start;
				range.EndPosition = units.Last().PositionInfo.End;
				range.Text = units.GetTextWithSubText();
				result.Add(range);
			}
			return result.ToArray();
		}

		/// <summary>
		/// Получение интервала чисел
		/// </summary>
		/// <param name="match">результат работы регулярного выражения</param>
		/// <param name="coder">кодер юнитов</param>
		/// <returns>интервал чисел</returns>
		protected abstract NumberRange GetNumberRange(Match match, UnitCoderBase coder);

		/// <summary>
		/// Получение числа из группы
		/// </summary>
		/// <param name="group">группа</param>
		/// <param name="coder">кодер юнитов</param>
		/// <returns>число</returns>
		protected Entity GetNumber(Group group, UnitCoderBase coder)
		{
			foreach (UnitTextBase unit in coder.GetUnits(group.Index, group.Length))
			{
				if (unit.IsEntity)
				{
					Entity entity = (Entity)unit;
					if (entity.IsNumber())
						return entity;
				}
			}
			throw new InvalidOperationException("can't find numeric unit");
		}
	}
}
