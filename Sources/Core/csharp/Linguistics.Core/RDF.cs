using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Linguistics.Core
{
	/// <summary>
	/// Константы RDF
	/// </summary>
	internal class RDF
	{
		/// <summary>
		/// Разделитель внутри атрибутов между несколькими значениями
		/// </summary>
		public const char ValueSeparator = ',';

		/// <summary>
		/// Атрибуты
		/// </summary>
		internal class Attribute
		{
			#region [.consts.]
			/// <summary>
			/// Язык предложения
			/// </summary>
			public const string Language = "LANG";

			/// <summary>
			/// Идентификатор
			/// </summary>
			public const string ID = "ID";
			/// <summary>
			/// Стартовая позиция
			/// </summary>
			public const string StartPosition = "POS";

			/// <summary>
			/// Подлежащее подпредложения
			/// </summary>
			public const string Subject = "SUBJ";

            /// <summary>
            /// Пассивный субъект
            /// </summary>
            public const string IsPassiveSubject = "PSubj";

			/// <summary>
			/// Тип
			/// </summary>
			public const string Type = "TYPE";
			/// <summary>
			/// Нормализованное значение
			/// </summary>
			public const string Value = "VALUE";
			/// <summary>
			/// Морфологическая информация
			/// </summary>
			public const string MorphoInformation = "MI";

			/// <summary>
			/// Роль сущности
			/// </summary>
			public const string Role = "ROLE";

			/// <summary>
			/// Главное существительное
			/// </summary>
			public const string MainNoun = "PROF";
			/// <summary>
			/// Прилагательные перед главным существительным
			/// </summary>
			public const string Prefix = "PRE";
			/// <summary>
			/// Все прилагательные цепочки
			/// </summary>
			public const string Adjectives = "ADJ";

			/// <summary>
			/// Флаг географии
			/// </summary>
			public const string PseudoPlace = "PLACE";

			/// <summary>
			/// Темы
			/// </summary>
			public const string Themes = "THEMES";
			#endregion

			#region [.Names.]
			/// <summary>
			/// Имя
			/// </summary>
			public static string FirstName { get { return "FN"; } }
			/// <summary>
			/// Фамилия
			/// </summary>
			public static string LastName { get { return "LN"; } }
			/// <summary>
			/// Отчество
			/// </summary>
			public static string MiddleName { get { return "MN"; } }
			/// <summary>
			/// Название
			/// </summary>
			public static string Name { get { return "NAME"; } }
            /// <summary>
            /// Должность
            /// </summary>
            public static string Post { get { return "POST"; } }
            /// <summary>
            /// Объект должности
            /// </summary>
            public static string PostObject { get { return "PO"; } }
			#endregion

			#region [.Тональность.]
			public const string INQUIRYINITIAL = "INQUIRYINITIAL";

			public const string SNT = "SNT";
			public const string FRT = "FRT";
			public const string SNTFINAL = "SNTFINAL";
			public const string FRTFINAL = "FRTFINAL";

			public const string TYPE_SAVED = "TYPE-SAVED";
			public const string TYPE_VALUE_INQUIRY = "Inquiry";
			#endregion
		}
    }
}
