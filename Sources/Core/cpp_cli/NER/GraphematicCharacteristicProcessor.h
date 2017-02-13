#pragma once

namespace NER
{
	using System::Collections::Generic::IEnumerable;

    /// Обработчик Графематических характеристик.Подкрепляет к словам определенные признаки
    ref class GraphematicCharacteristicProcessor
    {
	private:
		//[Признаки(Properties)]
		/// Начинается с заглавной буквы и содержит хотябы одну цифру [МИГ-21]
        static System::String^ NumCapital = "numC";
        /// Хотя бы одна римская цифра буква (без точки) [XVI] [X-XI]
		static System::String^ LatCapital = "latC";
        /// Одна заглавная буква без точки [F]
		static System::String^ OneCapital = "oneC";
        /// Все заглавные буквы (больше одной) [МТС]
		static System::String^ AllCapital = "allC";
        /// все буквы заглавные и все на латинице [POP]
		static System::String^ AllLatCapital = "allatC";
        /// одна заглавная буква с точкой [F.]
		static System::String^ OneCapitalPoint = "oneCP";
        /// одна или несколько первых буквы заглавные с точкой + маленькие буквы [В.В.Пупкин]
		static System::String^ FirstCapitalPoint = "fstC";
        /// все заглавные буквы (больше одной) с точкой (точками) [V.I.P.]
		static System::String^ AllCapitalPoint = "allCP";
        /// 1. Все заглавные буквы (больше одной) подряд с точкой (точками) [V.IV.I.PA]
		/// 2. Смешенные заглавные и прописные буквы; латиница + кириллица [СевКавГПУ]
		static System::String^ MixCapitalPoint = "mixCP";
        /// Только первая заглавная на латинице [Fox]
		static System::String^ FirstLatCapital = "latC";
        /// Только первая заглавная на кириллице [Вася]
		static System::String^ FirstCyrillicCapital = "C";
        /// Первые строчные с точкой, затем заглавная [ул.Васильевская]
		static System::String^ LowercasePoint = "loP";
		/// кавычки ["«“”»]
		static System::String^ Quote = "Q";
		/// первая буква строчная; в слове нет точек; обязательно присутствует заглавная буква
		static System::String^ iProd = "iProd";
        /// Другой
		static System::String^ Other = "O";

	private:
		/// Количество цифр
        int _digitsCount;
		/// Количество букв верхнего регистра
        int _capitalCount;
		/// Количество букв нижнего регстра
        int _lowercaseLettersCount;
		/// Количествв точек
        int _pointCount;
		/// Количество римских цифр
        int _romanNumberCount;
		/// Количество дефисов
        int _defisCount;

	public:
		/// Запустить обработку слова word
        /// @param word - Слово для обработки
        /// @return Результат обработки
		IEnumerable<System::String^>^ Run(System::String^ word);

	private:
		/// Получить признак, соответствующий рассматриваемому слову
        /// @param word - рассматриваемое слово
        /// @return Признак
		System::String^ GetProperty(System::String^ word);

		/// Является признаком LowercasePoint?
        /// @param word - рассматриваемое слово
		bool IsLowercasePoint(System::String^ word);

        /// Слово на латыни?
        /// @param word - рассматриваемое слово
		bool IsLatin(System::String^ word);

        /// Римская цифра?
        /// @param ch - Рассматриваемый символ
		bool IsRomanSymbol(wchar_t ch);

		/// Вычислить количество заглавных и прописных букв, цифр, римских цифр и точек в слове.
        /// @param word - Рассматриваемое слово
        void CalculateCounts(System::String^ word);
    };
}
