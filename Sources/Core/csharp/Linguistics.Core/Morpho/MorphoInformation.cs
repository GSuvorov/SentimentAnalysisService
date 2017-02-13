using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core.Morpho
{
	/// <summary>
	/// Класс морфологической информации сущности
	/// </summary>
    [Serializable]
	public class MorphoInformation
	{
		/// <summary>
		/// Падеж
		/// </summary>
		public Case Case { get; set; }
		/// <summary>
		/// Род
		/// </summary>
		public Gender Gender { get; set; }
		/// <summary>
		/// Число
		/// </summary>
		public Number Number { get; set; }
		/// <summary>
		/// Одушевленность
		/// </summary>
		public AnimationType Animation { get; set; }
		/// <summary>
		/// Переходность
		/// </summary>
		public TransitionalType Transitional { get; set; }
		/// <summary>
		/// Лицо
		/// </summary>
		public Face Face { get; set; }
		/// <summary>
		/// Залог
		/// </summary>
		public Voice Voice { get; set; }

		#region [.ctor().]
		public MorphoInformation()
		{
		}
		#endregion

		#region [.property and char.]
		/// <summary>
		/// задать падеж из заданного кода
		/// </summary>
		/// <param name="code">код падежа</param>
		public void SetCaseFromCode(char code)
		{
			Case = (Case)GetPropertyValue(Case, code);
		}

		/// <summary>
		/// задать род из заданного кода
		/// </summary>
		/// <param name="code">код рода</param>
		public void SetGenderFromCode(char code)
		{
			Gender = (Gender)GetPropertyValue(Gender, code);
		}

		/// <summary>
		/// задать число из заданного кода
		/// </summary>
		/// <param name="code">код числа</param>
		public void SetNumberFromCode(char code)
		{
			Number = (Number)GetPropertyValue(Number, code);
		}

		/// <summary>
		/// задать одушевленность из заданного кода
		/// </summary>
		/// <param name="code">код одушевленности</param>
		public void SetAnimationFromCode(char code)
		{
			Animation = (AnimationType)GetPropertyValue(Animation, code);
		}

		/// <summary>
		/// задать переходность из заданного кода
		/// </summary>
		/// <param name="code">код переходности</param>
		public void SetTransitionalFromCode(char code)
		{
			Transitional = (TransitionalType)GetPropertyValue(Transitional, code);
		}

		/// <summary>
		/// задать лицо из заданного кода
		/// </summary>
		/// <param name="code">код лица</param>
		public void SetFaceFromCode(char code)
		{
			Face = (Face)GetPropertyValue(Face, code);
		}

		/// <summary>
		/// задать залог из заданного кода
		/// </summary>
		/// <param name="code">код залога</param>
		public void SetVoiceFromCode(char code)
		{
			Voice = (Voice)GetPropertyValue(Voice, code);
		}

		/// <summary>
		/// Получение значения заданного свойства из заданного кода
		/// </summary>
		/// <param name="property">свойство</param>
		/// <param name="code">код</param>
		/// <returns>значение</returns>
		private object GetPropertyValue(object property, char code)
		{
			foreach (object value in Enum.GetValues(property.GetType()))
				if (value.ToString()[0] == code)
					return value;
			throw new ArgumentException("Wrong code for type: " + property.GetType().FullName);
		}

		/// <summary>
		/// Получение кода для значения свойства
		/// </summary>
		/// <param name="propertyValue">значение свойства</param>
		/// <returns>код значения свойства</returns>
		public char GetPropertyCode(object propertyValue)
		{
			return propertyValue.ToString()[0];
		}
		#endregion
	}
}
