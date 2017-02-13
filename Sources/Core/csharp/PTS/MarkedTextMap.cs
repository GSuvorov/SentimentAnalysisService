using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PTS
{
	/// <summary>
	/// Карта размеченного текста
	/// </summary>
	internal class MarkedTextMap
	{
		/// <summary>
		/// Размеченный интервал
		/// </summary>
		private struct MarkedInterval
		{
			/// <summary>
			/// Начало интервала
			/// </summary>
			public int Start { get; set; }
			/// <summary>
			/// Конец интервала
			/// </summary>
			public int End { get; set; }

			/// <summary>
			/// Проверка, что заданный интервал пересекается с текущим
			/// </summary>
			/// <param name="interval">интервал</param>
			/// <returns>результат проверки</returns>
			public bool IsIntersect(MarkedInterval interval)
			{
				return IsContainPosition(interval.Start) || IsContainPosition(interval.End - 1) ||
					interval.IsContainPosition(Start) || interval.IsContainPosition(End - 1);
			}

			/// <summary>
			/// Проверка, что текущий интервал включает заданную позицию
			/// </summary>
			/// <param name="position">позиция</param>
			/// <returns>результат проверки</returns>
			public bool IsContainPosition(int position)
			{
				return (Start <= position) && (End > position);
			}
		}

		/// <summary>
		/// Коллекция непрерывных размеченных интервалов
		/// </summary>
		List<MarkedInterval> _map = new List<MarkedInterval>();

		/// <summary>
		/// Попытка разметить интервал
		/// </summary>
		/// <param name="startPosition">начало интервала</param>
		/// <param name="endPosition">конец интервала</param>
		/// <returns>true, если интервал был размечен</returns>
		public bool TryMarkText(int startPosition, int endPosition)
		{
			bool result = true;
			int index = -1;
			MarkedInterval interval = new MarkedInterval() { Start = startPosition, End = endPosition };
			for (index = 0; index < _map.Count; ++index)
			{
				if (_map[index].IsIntersect(interval))
				{
					result = false;
					break;
				}
				else if (interval.End <= _map[index].Start)
					break;
			}
			if (result)
			{
				if (index > 0)
				/// возможное соединение нового интервала с предыдущим
				{
					MarkedInterval previous = _map[index - 1];
					if (previous.End == interval.Start)
					{
						interval.Start = previous.Start;
						--index;
						_map.RemoveAt(index);
					}
				}
				if (_map.Count > index)
				/// возможное соединение нового интервала со следующим
				{
					MarkedInterval next = _map[index];
					if (next.Start == interval.End)
					{
						interval.End = next.End;
						_map.RemoveAt(index);
					}
				}
				_map.Insert(index, interval);
			}
			return result;
		}
	}
}
