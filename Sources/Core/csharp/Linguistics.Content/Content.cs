using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;
using TextParsing;

namespace Linguistics.Core
{
	/// <summary>
	/// Контент
	/// </summary>
	public class Content
	{
		/// <summary>
		/// Исходное текстовое значение
		/// </summary>
		private string _text;
		/// <summary>
		/// Текстовое значение
		/// </summary>
		public virtual string TextValue { get { return _text; } }
		/// <summary>
		/// Длина контента
		/// </summary>
		public int Length { get { return _text.Length; } }
		/// <summary>
		/// Начало текста относительно родительского контента
		/// </summary>
		protected virtual int TextStartPosition { get { return 0; } }

		#region [.children.]
		/// <summary>
		/// Список дочерних контентов
		/// </summary>
		internal List<ChildContent> Children { get; set; }

		/// <summary>
		/// Получение коллекции дочерних контентов
		/// </summary>
		/// <returns></returns>
		public Content[] GetChildren()
		{
			return Children.ToArray();
		}
		#endregion

		#region [.ctor().]
		/// <summary>
		/// Конструктор
		/// </summary>
		/// <param name="text">текст контента</param>
		public Content(string text)
		{
			SetContentAndParse(text);
			_entitiesTree = new EntitySortedTree();
			Children = new List<ChildContent>();
		}
		#endregion

		#region [.paragraphs.]
		/// <summary>
		/// Параграфы
		/// </summary>
		private Paragraph[] _paragraphs;
		/// <summary>
		/// Возврат параграфов
		/// </summary>
		/// <returns>параграфы</returns>
		public IEnumerable<Paragraph> GetParagraphs()
		{
			return _paragraphs;
		}
		#endregion

		#region [.entities.]
		/// <summary>
		/// Выделенные сущности
		/// </summary>
		private EntitySortedTree _entitiesTree;
		/// <summary>
		/// Добавление сущности
		/// </summary>
		/// <param name="newEntity">сущность</param>
		/// <param name="isReplaceIntersected">флаг, что заданная сущность заменяет пересекающиеся с ней</param>
		public void AddEntity(Entity newEntity, bool isReplaceIntersected = false)
		{
			_entitiesTree.AddEntity(newEntity, isReplaceIntersected);
		}

		/// <summary>
		/// Очистка коллекции сущностей
		/// </summary>
		public void ClearEntities()
		{
			_entitiesTree.Clear();
		}

		/// <summary>
		/// Сущности
		/// </summary>
		public IEnumerable<Entity> Entities { get { return _entitiesTree.Elements; } }
		#endregion

		/// <summary>
		/// Создание подпредложения из текущего контента
		/// </summary>
		/// <param name="startPosition">стартовая позиция</param>
		/// <returns>подпредложение</returns>
		protected virtual SubSentence CreateSubSentence(int startPosition)
		{
			throw new NotImplementedException();
		}

		/// <summary>
		/// Задание полного текстового значения контента и его парсинг
		/// </summary>
		/// <param name="content">полное текствое значение контента</param>
		protected void SetContentAndParse(string content)
		{
			_text = content;
			_paragraphs = TextContent.Parser.ParseText(TextValue, false).ToArray();
		}

		/// <summary>
		/// Задание полного текстового значения контента
		/// </summary>
		/// <param name="text">полное текствое значение контента</param>
		protected virtual void SetFullText(string text)
		{
			_text = text;
		}

		#region [.potencial parts.]
		/// <summary>
		/// Информация о корректируемом контенте
		/// </summary>
		private class CorrectedContentInfo
		{
			/// <summary>
			/// Количество удаленных блоков контента
			/// </summary>
			public int RemovedContentCount { get; set; }
			/// <summary>
			/// Длина удаленных блоков контента
			/// </summary>
			public int RemovedContentLength { get; set; }

			/// <summary>
			/// Текущий сдвиг сущности
			/// </summary>
			public int CurrentEntityShift { get; set; }

			/// <summary>
			/// флаг, что обрабатываются только потенциальные части
			/// </summary>
			public bool IsOnlyPotencialParts { get; set; }
		}

		/// <summary>
		/// Информация о корректируемом предложении
		/// </summary>
		private class CorrectedSentenceInfo
		{
			/// <summary>
			/// Элементы предложения
			/// </summary>
			public SentenceItems SentenceItems { get; private set; }
			/// <summary>
			/// Новый текст предложения
			/// </summary>
			public StringBuilder NewSentenceText { get; private set; }

			#region [.ctor().]
			public CorrectedSentenceInfo(SentenceItems sentenceItems)
			{
				SentenceItems = sentenceItems;
				NewSentenceText = new StringBuilder(SentenceItems.Sentence.Text);
			}
			#endregion
		}

		/// <summary>
		/// Получение текста с потенциальными частями
		/// </summary>
		/// <param name="isOnlyPotencial">флаг, что нужно выбирать только потенциальные части</param>
		/// <returns>текст с потенциальными частями</returns>
		protected virtual string GetTextWithChildrenParts(bool isOnlyPotencial)
		{
			StringBuilder result = new StringBuilder();
			foreach (var sentence in GetSentencesWithChildrenParts(isOnlyPotencial))
				result.Append(sentence);
			return result.ToString();
		}

		/// <summary>
		/// Проверка, что данный контент может быть частью родительского
		/// </summary>
		virtual public bool IsPotencialParentPart { get { return false; } }

		/// <summary>
		/// Получение предложений с дочерними частями
		/// </summary>
		/// <param name="isOnlyPotencial">флаг, что нужно выбирать только потенциальные части</param>
		/// <returns>предложения с дочерними частями</returns>
		public string[] GetSentencesWithChildrenParts(bool isOnlyPotencial = true)
		{
			List<string> result = new List<string>();
			/// индекс текущего контента
			int contentIndex = 0;
			foreach (var paragraph in _paragraphs)
				foreach (var sentence in paragraph.Sentences)
					result.Add(GetSentenceWithChildrenParts(sentence, paragraph.StartPosition + sentence.StartPosition, isOnlyPotencial, ref contentIndex));

			return result.ToArray();
		}

		/// <summary>
		/// Получение текста предложения с дочерними частями
		/// </summary>
		/// <param name="sentence">предложение</param>
		/// <param name="startPosition">начальная позиция предложения</param>
		/// <param name="isOnlyPotencial">флаг, что нужно выбирать только потенциальные части</param>
		/// <param name="contentIndex">индекс текущего контента</param>
		/// <returns>текст предложения с дочерними частями</returns>
		private string GetSentenceWithChildrenParts(TextParsing.Sentence sentence, int startPosition, bool isOnlyPotencial, ref int contentIndex)
		{
			StringBuilder result = new StringBuilder(sentence.Text);
			var indexes = GetContentIndexesInsideSentence(sentence, ref contentIndex);
			int partsLength = 0;
			foreach (int index in indexes)
			{
				ChildContent child = Children[index];
				if (!isOnlyPotencial || child.IsPotencialParentPart)
				{
					result.Insert(GetContentCorrectedStart(index) - startPosition + partsLength, child.GetTextWithChildrenParts(isOnlyPotencial));
					partsLength += child.GetFullLength();
				}
			}
			return result.ToString();
		}

		/// <summary>
		/// Задание сущностей для контента и дочерних частей
		/// </summary>
		/// <param name="isOnlyPotencial">флаг, что нужно выбирать только потенциальные части</param>
		/// <param name="entities">коллекция сущностей</param>
		public void SetEntitiesForContentAndChildren(IEnumerable<Entity> entities, bool isOnlyPotencial = true)
		{
			int sentenceIndex = 0;
			var sentenceItems = GetSentencesItems(GetSentencesWithChildrenParts(isOnlyPotencial), entities);
			int paragraphStartPosition = 0;
			var correctedContentInfo = new CorrectedContentInfo() { IsOnlyPotencialParts = isOnlyPotencial };
			for (int i = 0; i < _paragraphs.Length; ++i)
			{
				var paragraphSentenceItems = sentenceItems.GetRange(sentenceIndex, _paragraphs[i].Sentences.Length);
				sentenceIndex += _paragraphs[i].Sentences.Length;
				_paragraphs[i] = GetNewParagraph(paragraphSentenceItems, paragraphStartPosition, correctedContentInfo);
				paragraphStartPosition = _paragraphs[i].EndPosition;
			}
			SetFullText(_paragraphs.GetText());
		}

		/// <summary>
		/// Получение нового параграфа
		/// </summary>
		/// <param name="sentenceItemsCollection">коллекция элементов предложений</param>
		/// <param name="startPosition">начальная позиция</param>
		/// <param name="contentInfo">информация о корректируемом контенте</param>
		/// <returns>новый параграф</returns>
		private Paragraph GetNewParagraph(IEnumerable<SentenceItems> sentenceItemsCollection,
			int startPosition, CorrectedContentInfo contentInfo)
		{
			List<TextParsing.Sentence> sentences = new List<TextParsing.Sentence>();

			int sentenceStartPosition = 0;
			foreach (var sentenceItems in sentenceItemsCollection)
			{
				var sentence = new TextParsing.Sentence(GetNewSentenceText(sentenceItems, contentInfo), sentenceStartPosition);
				sentences.Add(sentence);
				sentenceStartPosition = sentence.EndPosition;
			}

			Paragraph result = new Paragraph(sentences.GetText(), startPosition);
			result.SetSentences(sentences);
			return result;
		}

		/// <summary>
		/// Получение нового текста предложения
		/// </summary>
		/// <param name="sentenceItems">элементы предложения</param>
		/// <param name="contentInfo">информация о корректируемом контенте</param>
		/// <returns>новый текст предложения</returns>
		private string GetNewSentenceText(SentenceItems sentenceItems, CorrectedContentInfo contentInfo)
		{
			var sentenceInfo = new CorrectedSentenceInfo(sentenceItems);
			int entityIndex = 0;
			int startContentIndex = sentenceInfo.SentenceItems.ContentIndex - contentInfo.RemovedContentCount;
			int removedContentCountStart = contentInfo.RemovedContentCount;
			foreach (int index in GetContentIndexesInsideSentence(sentenceItems.Sentence, ref startContentIndex, contentInfo.RemovedContentLength))
			{
				int removedContentCount = contentInfo.RemovedContentCount - removedContentCountStart;
				DecomposeEntitiesAndContentToSentence(index - removedContentCount, sentenceInfo, contentInfo, ref entityIndex);
			}
			for (int i = sentenceItems.Entities.Length - 1; i >= entityIndex; --i)
				AddEntity(sentenceItems.Entities[i], contentInfo.CurrentEntityShift);
			return sentenceInfo.NewSentenceText.ToString();
		}

		/// <summary>
		/// Определение является ли контент частью данного предложения и
		/// распределение сущностей между предложением и контентов в противном случае
		/// </summary>
		/// <param name="contentIndex">индекс контента</param>
		/// <param name="sentenceInfo">информация о корректируемом предложении</param>
		/// <param name="contentInfo">информация о корректируемом контенте</param>
		/// <param name="entityIndex">индекс сущности</param>
		private void DecomposeEntitiesAndContentToSentence(int contentIndex, CorrectedSentenceInfo sentenceInfo,
			CorrectedContentInfo contentInfo, ref int entityIndex)
		{
			ChildContent content = Children[contentIndex];
			int notPotencialPartsShift = contentInfo.IsOnlyPotencialParts ? GetFullContentsLength(contentIndex, true) : 0;
			int contentStartPosition = content.StartPosition - notPotencialPartsShift;
			int contentEndPosition = contentStartPosition + content.GetFullLength();
			List<Entity> contentEntities = new List<Entity>();
			bool isShiftedContent = !contentInfo.IsOnlyPotencialParts || content.IsPotencialParentPart;
			bool isRemovedContent = false;
			if (isShiftedContent)
			{
				for (; entityIndex < sentenceInfo.SentenceItems.Entities.Length; ++entityIndex)
				{
					Entity entity = sentenceInfo.SentenceItems.Entities[entityIndex];
					if (entity.PositionInfo.End <= contentStartPosition)
					{
						AddEntity(entity, contentInfo.CurrentEntityShift);
						continue;
					}
					else if (entity.PositionInfo.Start >= contentEndPosition)
						break;
					else if ((entity.PositionInfo.Start < contentStartPosition) && (contentEndPosition < entity.PositionInfo.End))
					/// сущность полностью содержит контент - контент включаем в родителя
					{
						++entityIndex;
						AddEntity(entity, contentInfo.CurrentEntityShift);
						RemoveChildContent(sentenceInfo, contentInfo, contentIndex);
						isRemovedContent = true;
						break;
					}
					else if ((contentStartPosition < entity.PositionInfo.Start) && (entity.PositionInfo.End < contentEndPosition))
						contentEntities.Add(entity);
					#region [.defense.]
					else /// сущность пересекается с дочерним контентом - пропускаем такую сущность
						++entityIndex;
					#endregion
				}
			}
			if (!isRemovedContent)
			{
				UpdateUnitsPosition(contentEntities, notPotencialPartsShift - content.TextStartPosition);
				content.SetEntitiesForContentAndChildren(contentEntities, contentInfo.IsOnlyPotencialParts);
				if (isShiftedContent)
					contentInfo.CurrentEntityShift += content.GetFullLength();
			}
		}

		/// <summary>
		/// Добавление сущности
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <param name="leftShift">сдвиг</param>
		private void AddEntity(Entity entity, int leftShift)
		{
			entity.MovePosition(leftShift);
			AddEntity(entity, true);
		}

		/// <summary>
		/// Удаление дочернего контента
		/// </summary>
		/// <param name="sentenceInfo">информация о корректируемом предложении</param>
		/// <param name="contentInfo">информация о корректируемом контенте</param>
		/// <param name="contentIndex">индекс удаляемого контента</param>
		private void RemoveChildContent(CorrectedSentenceInfo sentenceInfo, CorrectedContentInfo contentInfo, int contentIndex)
		{
			ChildContent content = Children[contentIndex];
			string contentText = content.GetFullText();
			
			int startPosition = GetContentCorrectedStart(contentIndex)
				- (sentenceInfo.SentenceItems.Sentence.GetFullStartPosition()
				+ contentInfo.RemovedContentLength);
			sentenceInfo.NewSentenceText.Insert(startPosition, contentText);

			foreach (var entity in content.Entities)
				AddEntity(entity, -(startPosition + content.TextStartPosition - content.StartPosition));
			Children.RemoveAt(contentIndex);
			++contentInfo.RemovedContentCount;
			contentInfo.RemovedContentLength += contentText.Length;
		}
		#endregion

		#region [.sentences items.]
		/// <summary>
		/// Элементы предложения
		/// </summary>
		private struct SentenceItems
		{
			/// <summary>
			/// Предложение
			/// </summary>
			public TextParsing.Sentence Sentence { get; private set; }
			/// <summary>
			/// Сущности
			/// </summary>
			public Entity[] Entities { get; private set; }
			/// <summary>
			/// Индекс, с которого идут контенты данного предложения
			/// </summary>
			public int ContentIndex { get; private set; }

			#region [.ctor().]
			public SentenceItems(TextParsing.Sentence sentence, Entity[] entities, int contentIndex)
				: this()
			{
				Sentence = sentence;
				Entities = entities;
				ContentIndex = contentIndex;
			}
			#endregion
		}

		/// <summary>
		/// Получение элементов предложений
		/// </summary>
		/// <param name="entities">сущности текста</param>
		/// <returns>элементы предложений</returns>
		private List<SentenceItems> GetSentencesItems(IEnumerable<Entity> entities)
		{
			return GetSentencesItems(_paragraphs.SelectMany(_ => _.Sentences.Select(sent => sent.Text)).ToArray(), entities);
		}

		/// <summary>
		/// Получение элементов предложений
		/// </summary>
		/// <param name="sentences">предложения текста</param>
		/// <param name="entities">сущности текста</param>
		/// <returns>элементы предложений</returns>
		private List<SentenceItems> GetSentencesItems(IList<string> sentences, IEnumerable<Entity> entities)
		{
			List<SentenceItems> result = new List<SentenceItems>();

			/// индекс текущего параграфа
			int currentParagraphIndex = 0;
			/// индекс текущего предложения
			int currentSentenceIndex = 0;
			/// индекс текущего контента
			int contentIndex = 0;
			/// текущее предложение
			var currentSentence = GetCurrentSentece(currentParagraphIndex, currentSentenceIndex);
			/// список сущностей текущего предложения
			var currentSentenceEntities = new List<Entity>();

			/// Индекс текста предложения
			int sentenceIndex = 0;
			/// Начало предложения
			int sentenceStart = 0;
			foreach (var entity in entities)
			{
				while (sentenceStart + sentences[sentenceIndex].Length <= entity.PositionInfo.Start)
				{
					result.Add(SetSentenceItems(currentSentence, currentSentenceEntities, ref contentIndex));
					currentSentence = GetNextSentence(ref currentParagraphIndex, ref currentSentenceIndex);
					sentenceStart += sentences[sentenceIndex].Length;
					++sentenceIndex;
				}
				currentSentenceEntities.Add(entity);
			}
			while (currentSentence != null)
			{
				result.Add(SetSentenceItems(currentSentence, currentSentenceEntities, ref contentIndex));
				currentSentence = GetNextSentence(ref currentParagraphIndex, ref currentSentenceIndex);
			}

			return result;
		}

		/// <summary>
		/// Задание элементов предложения
		/// </summary>
		/// <param name="sentence">предложение</param>
		/// <param name="entities">сущности</param>
		/// <param name="currentContentIndex">индекс, с которого идут контенты данного предложения</param>
		/// <returns>элементы предложения</returns>
		private SentenceItems SetSentenceItems(TextParsing.Sentence sentence, IList<Entity> entities, ref int currentContentIndex)
		{
			var result = new SentenceItems(sentence, entities.ToArray(), currentContentIndex);
			GetContentIndexesInsideSentence(sentence, ref currentContentIndex);
			entities.Clear();
			return result;
		}

		/// <summary>
		/// Получение текущего предложения
		/// </summary>
		/// <param name="paragraphIndex">индекс текущего параграфа</param>
		/// <param name="sentenceIndex">индекс текущего предложения в параграфе</param>
		/// <returns>текущее предложение</returns>
		private TextParsing.Sentence GetCurrentSentece(int paragraphIndex, int sentenceIndex)
		{
			if (paragraphIndex < _paragraphs.Length)
				return _paragraphs[paragraphIndex].Sentences[sentenceIndex];
			else
				return null;
		}

		/// <summary>
		/// Получение следующего предложения
		/// </summary>
		/// <param name="paragraphIndex">индекс текущего параграфа</param>
		/// <param name="sentenceIndex">индекс текущего предложения в параграфе</param>
		/// <returns>следующее предложение</returns>
		private TextParsing.Sentence GetNextSentence(ref int paragraphIndex, ref int sentenceIndex)
		{
			++sentenceIndex;
			if (_paragraphs[paragraphIndex].Sentences.Length <= sentenceIndex)
			{
				++paragraphIndex;
				sentenceIndex = 0;
			}
			return GetCurrentSentece(paragraphIndex, sentenceIndex);
		}
		#endregion

        /// <summary>
        /// Проставка для предложений статуса обработки
        /// </summary>
        //---WTF-?!?!?!---//
        /*
        public void SetSentenceProcessedStatus()
        {
            
            foreach ( var paragraph in _paragraphs )
            {
                foreach ( var sentence in paragraph.Sentences )
                {
                    if ( GarbageSentenceSelector.IsGarbage( sentence.Text ) )
                    {
                        sentence.IsProcessed = false;
                    }
                }
            }
            
        }*/

		/// <summary>
		/// Объединение предложений по сущностям
		/// </summary>
		/// <param name="entities">сущности</param>
        public void UnionSentencesByEntities( IEnumerable<Entity> entities )
        {
            var sentenceItems = GetSentencesItems( entities );
            int sentenceIndex = 0;
            List<Paragraph> paragraphs = new List<Paragraph>( _paragraphs.Length );
            TextParsing.Sentence nextParagraphSentence = null;
            for ( int i = 0; i < _paragraphs.Length; ++i )
            {
                var paragraphSentenceItems = sentenceItems.GetRange( sentenceIndex, _paragraphs[ i ].Sentences.Length );
                sentenceIndex += _paragraphs[ i ].Sentences.Length;
                var sentences = GetParagraphSentences( paragraphSentenceItems, _paragraphs[ i ].StartPosition );
                var paragraph = GetParagraph( _paragraphs[ i ], nextParagraphSentence, sentences.Item1, sentences.Item2 );
                nextParagraphSentence = sentences.Item2;
                if ( paragraph != null )
                    paragraphs.Add( paragraph );
            }
            _paragraphs = paragraphs.ToArray();
        }

		/// <summary>
		/// Получение параграфа
		/// </summary>
		/// <param name="paragraph">текущий параграф</param>
		/// <param name="firstSentence">первое предложение текущего параграфа (наследуется от предыдущего)</param>
		/// <param name="sentences">предложения текущего параграфа</param>
		/// <param name="nextParagraphSentence">предложение следующего параграфа</param>
		/// <returns>параграф</returns>
		private Paragraph GetParagraph(
			Paragraph paragraph,
			TextParsing.Sentence firstSentence,
			TextParsing.Sentence[] sentences,
			TextParsing.Sentence nextParagraphSentence)
		{
			Paragraph result = paragraph;
			if ((firstSentence != null) || (nextParagraphSentence != null))
			{
				int startPosition = paragraph.StartPosition;
				string paragraphText = string.Empty;
				if (firstSentence != null)
				{
					if (sentences.Any())
					{
						sentences[0].InsertPreviousPart(firstSentence);
						startPosition = firstSentence.Parent.StartPosition + firstSentence.StartPosition;
					}
					else
						nextParagraphSentence.InsertPreviousPart(firstSentence);
				}
				if (!sentences.Any())
					result = null;
				else
					result = new Paragraph(sentences.GetText(), startPosition);
			}
			if (result != null)
				result.SetSentences(sentences);
			return result;
		}

		/// <summary>
		/// Получение предложений параграфа
		/// </summary>
		/// <param name="paragraphSentenceItems">элементы предложений параграфата</param>
		/// <param name="paragraphStartPosition">начальная позиция параграфа</param>
		/// <returns>пара [предложения текущего параграфа - предложение следующего параграфа]</returns>
		private Tuple<TextParsing.Sentence[], TextParsing.Sentence>
			GetParagraphSentences(IList<SentenceItems> paragraphSentenceItems, int paragraphStartPosition)
		{
			List<TextParsing.Sentence> result = new List<TextParsing.Sentence>();
			TextParsing.Sentence parent = null;
			foreach (var sentenceItems in paragraphSentenceItems)
			{
				Entity lastEntity = sentenceItems.Entities.LastOrDefault();
				TextParsing.Sentence current = sentenceItems.Sentence;
				if (parent != null)
				{
					parent.AppendNextPart(current);
					current = parent;
					parent = null;
				}
				int sentenceEndPosition = paragraphStartPosition + current.StartPosition + current.Text.Length;
				if ((lastEntity != null) && (sentenceEndPosition < lastEntity.PositionInfo.End))
					parent = current;
				else
					result.Add(current);
			}
			return new Tuple<TextParsing.Sentence[],TextParsing.Sentence>(result.ToArray(), parent);
		}

		/// <summary>
		/// Получение коллекции предложений
		/// </summary>
		/// <returns>коллекция предложений</returns>
		public Sentence[] GetAllSentences()
		{
			var result = new List<Sentence>(_paragraphs.Sum(_ => _.Sentences.Length));

			/// индекс текущего контента
			int contentIndex = 0;
			foreach (var sentenceItems in GetSentencesItems(_entitiesTree.Elements))
			{
				contentIndex = sentenceItems.ContentIndex;
				result.Add(CreateSentenceObject(sentenceItems.Sentence, sentenceItems.Entities, ref contentIndex));
			}
			return result.ToArray();
		}

		/// <summary>
		/// Получение списка индексов контентов начиная с текущего, которые лежат в заданном предложении
		/// </summary>
		/// <param name="textSentence">текстовое предложение</param>
		/// <param name="currentContentIndex">индекс текущего контента</param>
		/// <param name="sentenceShift">сдвиг предложения</param>
		/// <returns>список индексов контентов в заданном предложении</returns>
		private List<int> GetContentIndexesInsideSentence(TextParsing.Sentence textSentence, ref int currentContentIndex, int sentenceShift = 0)
		{
			List<int> result = new List<int>();
			int sentenceStart = textSentence.GetFullStartPosition() + sentenceShift;
			int sentenceEnd = textSentence.GetFullEndPosition() + sentenceShift;
			while (IsSentenceContainContent(currentContentIndex, textSentence.Text, sentenceStart, sentenceEnd))
			{
				result.Add(currentContentIndex);
				++currentContentIndex;
			}
			return result;
		}

		/// <summary>
		/// Проверка, что предложение содержит контент
		/// </summary>
		/// <param name="contentIndex">индекс контента</param>
		/// <param name="sentenceText">текст предложения</param>
		/// <param name="sentenceStart">начало предложения</param>
		/// <param name="sentenceEnd">конец предложения</param>
		/// <returns>результат проверки</returns>
		private bool IsSentenceContainContent(int contentIndex, string sentenceText, int sentenceStart, int sentenceEnd)
		{
			if (contentIndex < Children.Count)
			{
				int contentStart = GetContentCorrectedStart(contentIndex);

				return ((sentenceStart < contentStart) && (contentStart < sentenceEnd)) ||
					((contentStart == sentenceStart) && (Children[contentIndex].NextSymbol == sentenceText.FirstOrDefault())) ||
					((contentStart == sentenceEnd) && (Children[contentIndex].PreviousSymbol == sentenceText.LastOrDefault()));
			}
			return false;
		}

		/// <summary>
		/// Получение суммарной длины контентов для заданного последнего контента 
		/// </summary>
		/// <param name="lastContentIndex">индекс последнего контента</param>
		/// <param name="isOnlyNotPotencial">флаг, что нужно обрабатывать только не потенциальные части</param>
		/// <returns>суммарная длина контентов</returns>
		private int GetFullContentsLength(int lastContentIndex, bool isOnlyNotPotencial = false)
		{
			int result = 0;
			for (int i = 0; i < lastContentIndex; ++i)
				if (!(isOnlyNotPotencial && Children[i].IsPotencialParentPart))
					result += Children[i].GetFullLength();
			return result;
		}

		/// <summary>
		/// Получение полной длины контента (с учетом дочерних контентов)
		/// </summary>
		/// <returns>полная длина контента</returns>
		private int GetFullLength()
		{
			int result = Length;
			foreach (var child in Children)
				result += child.GetFullLength();
			return result;
		}

		/// <summary>
		/// Получение скорректированного начала контента
		/// 
		/// дочерний контент был вырезан из исходного текста
		/// для вырезанного контента была сохранена правильная позиция, а для предложения - нет
		/// </summary>
		/// <param name="contentIndex">индекс контента</param>
		/// <returns>скорректированное начало контента</returns>
		private int GetContentCorrectedStart(int contentIndex)
		{
			return Children[contentIndex].StartPosition - GetFullContentsLength(contentIndex);
		}

		/// <summary>
		/// Создание объекта предложения
		/// </summary>
		/// <param name="textSentence">текстовое предложение</param>
		/// <param name="entities">список сущностей предложения</param>
		/// <param name="currentContentIndex">индекс текущего контента</param>
		/// <returns>объект предложения</returns>
		private Sentence CreateSentenceObject(TextParsing.Sentence textSentence, IEnumerable<Entity> entities, ref int currentContentIndex)
		{
			var result = new Sentence(textSentence.GetFullStartPosition() + GetFullContentsLength(currentContentIndex));
			if (entities.Any())
			{
				var entityWithLang = entities.FirstOrDefault(_ => !string.IsNullOrEmpty(_.Language));
				if (entityWithLang != null)
					result.Language = entityWithLang.Language;
				else
					result.Language = "EN";
			}
			var subSentence = CreateSubSentenceObject(result, textSentence, entities, GetContentIndexesInsideSentence(textSentence, ref currentContentIndex));
			result.SetSubSentencesHeirarchy(new SubSentence[] { subSentence });
			return result;
		}

		/// <summary>
		/// Создание объекта подпредложения 
		/// </summary>
		/// <param name="parentSentence">родительское предложение</param>
		/// <param name="textSentence">текстовое предложение</param>
		/// <param name="entities">список сущностей предложения</param>
		/// <param name="contentIndexes">индексы контентов в предложении</param>
		/// <returns>объект подпредложения</returns>
		private SubSentence CreateSubSentenceObject(
			Sentence parentSentence,
			TextParsing.Sentence textSentence,
			IEnumerable<Entity> entities,
			IEnumerable<int> contentIndexes)
		{
			var result = new SubSentence(SubSentenceType.Default, 0) { ParentObject = parentSentence };

			var units = GetSubSentenceUnits(result, textSentence, entities);
			var subSentences = GetChildContentSubSentences(contentIndexes, units, parentSentence);

			result.SetUnits(units);
			result.AppendSubSentences(subSentences);
			return result;
		}

		/// <summary>
		/// Получение подпредложений для дочерних контентов
		/// </summary>
		/// <param name="contentIndexes">список дочерних контентов предложения</param>
		/// <param name="units">коллекция элементов предложения</param>
		/// <param name="parentSentence">предложение</param>
		/// <returns>подпредложения для дочерних контентов</returns>
		private IEnumerable<SubSentence> GetChildContentSubSentences(IEnumerable<int> contentIndexes, List<UnitTextBase> units, Sentence parentSentence)
		{
			var result = new List<SubSentence>();
			
			int unitIndex = 0;
			int updateStartIndex = 0;
			foreach (int contentIndex in contentIndexes)
			{
				unitIndex = FindUnitIndexForContent(contentIndex, units, unitIndex);
				updateStartIndex = unitIndex;
				bool isCreateSubSentence = true;
				if (unitIndex < units.Count)
				{
					int contentPosition = Children[contentIndex].StartPosition - units[unitIndex].GetFullStartPosition();
					if (contentPosition == 0)
					{
						var unit = units[unitIndex];
						units.Insert(unitIndex, SetParentSubSentence(new UnmarkedText("", unit.PositionInfo.Start), unit.ParentObject));
					}
					else if (units[unitIndex].IsEntity)
					{
						Entity entity = (Entity)units[unitIndex];
						entity.InsertText(Children[contentIndex].GetFullText(), contentPosition);
						++updateStartIndex;
						isCreateSubSentence = false;
					}
					else
					{
						unitIndex = SplitUnit(units, unitIndex, contentPosition);
						updateStartIndex = unitIndex;
					}
				}
				
				if (isCreateSubSentence)
					result.Add(Children[contentIndex].CreateSubSentence(Children[contentIndex].StartPosition - parentSentence.StartPosition));
				UpdateUnitsPosition(units.Skip(updateStartIndex), Children[contentIndex].GetFullLength());
			}

			return result;
		}

		/// <summary>
		/// Обновление позиции юнитов
		/// </summary>
		/// <param name="units">коллекция юнитов</param>
		/// <param name="shift">сдвиг позиции</param>
		private void UpdateUnitsPosition(IEnumerable<UnitTextBase> units, int shift)
		{
			foreach (var unit in units)
				unit.MovePosition(-shift);
		}

		/// <summary>
		/// Разрезка юнита на 2
		/// </summary>
		/// <param name="units">список юнитов</param>
		/// <param name="unitIndex">индекс юнита, который надо разрезать</param>
		/// <param name="splitPosition">позиция, по которой надо разрезать юнит</param>
		/// <returns>индекс второй части разрезанного юнита</returns>
		private int SplitUnit(List<UnitTextBase> units, int unitIndex, int splitPosition)
		{
			int result = unitIndex;
			UnitTextBase unit = units[result];
			units[result] = SetParentSubSentence(CreateUnmarkedText(unit.Text, 0, splitPosition, unit.PositionInfo.Start), unit.ParentObject);
			++result;
			units.Insert(result, SetParentSubSentence(CreateUnmarkedText(unit.Text, splitPosition, unit.get_Length(), unit.PositionInfo.Start), unit.ParentObject));
			return result;
		}

		/// <summary>
		/// Поиск индекса юнита, в который попадает позиция контента
		/// </summary>
		/// <param name="contentIndex">индекс контента</param>
		/// <param name="units">коллекция юнитов</param>
		/// <param name="startUnitIndex">индекс юнита, с которого начинается поиск</param>
		/// <returns>индекс юнита</returns>
		private int FindUnitIndexForContent(int contentIndex, IList<UnitTextBase> units, int startUnitIndex)
		{
			int result = startUnitIndex;
			int contentStart = Children[contentIndex].StartPosition;
			while (result < units.Count)
			{
				UnitTextBase unit = units[result];
				if ((unit.GetFullStartPosition() <= contentStart) && (contentStart < unit.GetFullEndPosition()))
				/// контент начинается внутри юнита
				{
					break;
				}
				++result;
			}
			return result;
		}

		/// <summary>
		/// Получение коллекции элементов подпредложения
		/// </summary>
		/// <param name="parentSubSentence">объект подпредложения</param>
		/// <param name="textSentence">текстовое подпредложение</param>
		/// <param name="entities">список сущностей</param>
		/// <returns>коллекция элементов предложения</returns>
		private List<UnitTextBase> GetSubSentenceUnits(SubSentence parentSubSentence, TextParsing.Sentence textSentence, IEnumerable<Entity> entities)
		{
			List<UnitTextBase> result = new List<UnitTextBase>();
			int currentTextPos = 0;
			foreach (Entity entity in entities)
			{
				entity.MovePosition(textSentence.GetFullStartPosition());
				if (entity.PositionInfo.Start != 0)
					result.Add(SetParentSubSentence(CreateUnmarkedText(textSentence.Text, currentTextPos, entity.PositionInfo.Start), parentSubSentence));
				result.Add(SetParentSubSentence(entity, parentSubSentence));
				currentTextPos = entity.PositionInfo.End;
			}
			if (currentTextPos < textSentence.Text.Length)
				result.Add(SetParentSubSentence(CreateUnmarkedText(textSentence.Text, currentTextPos, textSentence.Text.Length), parentSubSentence));
			return result;
		}

		/// <summary>
		/// Задание родительского подпредложения для заданного юнита
		/// </summary>
		/// <param name="unit">юнит</param>
		/// <param name="parentSubSentence">родительское предложение</param>
		/// <returns>юнит</returns>
		private UnitTextBase SetParentSubSentence(UnitTextBase unit, SubSentence parentSubSentence)
		{
			unit.ParentObject = parentSubSentence;
			return unit;
		}

		/// <summary>
		/// Создание неразмеченного текста из заданного предложения
		/// </summary>
		/// <param name="text">текст предложения</param>
		/// <param name="startPosition">начало неразмеченного текста</param>
		/// <param name="endPosition">конец неразмеченного текста</param>
		/// <param name="unitPosition">начальная позиция юнита</param>
		/// <returns>неразмеченный текст</returns>
		private UnmarkedText CreateUnmarkedText(string text, int startPosition, int endPosition, int unitPosition = 0)
		{
			return new UnmarkedText(text.Substring(startPosition, endPosition - startPosition), startPosition + unitPosition);
		}
	}
}
