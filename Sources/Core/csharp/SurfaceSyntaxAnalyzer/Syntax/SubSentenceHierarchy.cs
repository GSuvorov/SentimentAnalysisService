using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Linguistics.Core;
using Linguistics.Core.Morpho;

namespace SurfaceSyntaxAnalyzer
{
	/// <summary>
	/// Иерархия подпредложений
	/// </summary>
	internal class SubSentenceHierarchy
    {
        #region [.constants.]
        private readonly string[] TireForms = new string[] { "-", "–", "—", "―" };
        #endregion

        #region [.properties.]
        /// <summary>
		/// Информация по текущему подпредложению
		/// </summary>
		public SubSentenceInfo CurrentSubSentence { get; private set; }

		/// <summary>
		/// Список информации подпредложений
		/// </summary>
		private List<SubSentenceInfo> _subSentenceInfoList;

		/// <summary>
		/// Исходное подпредложение
		/// </summary>
		private SubSentence _sourceSubSentence;

		/// <summary>
		/// тип текущего подпредложения
		/// </summary>
		public SubSentenceType CurrentType
		{
			get { return CurrentSubSentence.SubSentence.Type; }
			set { CurrentSubSentence.SubSentence.Type = value; }
		}
		#endregion

		#region [.ctor().]
		public SubSentenceHierarchy(SubSentence source)
		{
			_subSentenceInfoList = new List<SubSentenceInfo>();
			CurrentSubSentence = new SubSentenceInfo(source.Units.First(), source.StartPosition);
			_sourceSubSentence = source;
		}
		#endregion

		#region [.test methods.]
		/// <summary>
		/// Проверка, что в заданный глагол является частью составного в текущем подпредложении
		/// </summary>
		/// <param name="verb">глагол</param>
		/// <returns>результат проверки</returns>
		private bool IsCompoundVerbInCurrent(Entity verb)
		{
			Entity previousVerb = CurrentSubSentence.Verbs.Last();

			UnitTextBase next = previousVerb.GetNonEmptyNext();
            if ((next == verb) || previousVerb.IsType(EntityType.Infinitive))
				return true;
			else if (IsHomogeneousConjunctionBetween(next, verb))
				return false;
			else
				return IsPassiveVoice(previousVerb, verb);
		}

		/// <summary>
		/// Проверка, что между двумя юнитами есть однородный союз
		/// </summary>
		/// <param name="begin">начало интервала</param>
		/// <param name="end">конец интервала</param>
		/// <returns>результат проверки</returns>
		private bool IsHomogeneousConjunctionBetween(UnitTextBase begin, UnitTextBase end)
		{
			UnitTextBase current = begin;
			while ((current != null) && (current != end))
			{
				if (DictionaryResource.IsHomogeneousConjunction(current.Text))
					return true;
				current = current.GetNonEmptyNext();
			}
			return false;
		}
		#endregion

		#region [.add methods.]
		/// <summary>
		/// Добавление союза в текущее подпредложение
		/// </summary>
		/// <param name="conjunction">союз</param>
		public void AddConjunctionToCurrent(Entity conjunction)
		{
            if (DictionaryResource.IsHomogeneousConjunction(conjunction.Text))
                return;
            UnitTextBase previous = conjunction.GetNonEmptyPrevious();
            if ((previous != null) && previous.IsSeparator(TireForms) && (CurrentSubSentence.StartUnit != previous))
                CreateNewSubSentence(previous.Previous);
			if (CurrentSubSentence.IsContainConjunction)
			{
				if (previous.IsEntity && IsConjunction((Entity)previous))
					CreateNewSubSentence(previous);
				else
					return;
			}
			if (CurrentType != SubSentenceType.DanglingParticiple)
			{
				CurrentSubSentence.IsContainConjunction = true;
				CurrentSubSentence.SubSentence.Type = ((ConjunctionEntityInfo)conjunction.EntityInfo).ConjunctionType;
			}
		}

		/// <summary>
		/// Добавление глагольной формы в текущее подпредложение
		/// </summary>
		/// <param name="verb">глагол</param>
		public void AddVerbFormToCurrent(Entity verb)
		{
			switch (verb.Type.EntityType)
			{
				case EntityType.Verb:
					AddVerbToCurrent(verb);
					break;
                case EntityType.Infinitive:

                    break;
				case EntityType.Deepr:
					AddDeeprToCurrent(verb);
					break;
				case EntityType.Participle:
					CurrentSubSentence.Participles.Add(verb);
					SetCurrentTypeByVerbForm(verb);
					break;
				#region [.defense.]
				default:
					throw new ArgumentException("entity isn't verb");
				#endregion
			}
		}

        /// <summary>
        /// Добавление глагола в текущее подпредложение
        /// </summary>
        /// <param name="verb">глагол</param>
		private void AddVerbToCurrent(Entity verb)
		{
			/// глагол в деепричастном обороте: неверно обособленный деепричастный оборот - отсутствует правая граница
			bool isVerbInDanglingParticiple = (CurrentType == SubSentenceType.DanglingParticiple);
			if (isVerbInDanglingParticiple || (CurrentSubSentence.IsExistVerb() && !IsCompoundVerbInCurrent(verb)))
				CreateNewSubSentenceByVerbForm(verb);
			else
			{
				SetCurrentTypeByVerbForm(verb);
				CurrentSubSentence.Verbs.Add(verb);
			}
		}

        /// <summary>
        /// Добавление инфинитива в текущее подпредложение
        /// </summary>
        /// <param name="infinitive">инифинитив</param>
        private void AddInfinitiveToCurrent(Entity infinitive)
        {
            SetCurrentTypeByVerbForm(infinitive);
            CurrentSubSentence.Verbs.Add(infinitive);
        }

        /// <summary>
        /// Добавление деепричастия в текущее подпредложение
        /// </summary>
        /// <param name="deepr">деепричастие</param>
		private void AddDeeprToCurrent(Entity deepr)
		{
			if (CurrentSubSentence.IsExistVerb() && (CurrentType != SubSentenceType.DanglingParticiple))
			/// неверно обособленный деепричастный оборот - отсутствует левая граница
			{
				CreateNewSubSentence(deepr.Previous);
			}
			CurrentSubSentence.Verbs.Add(deepr);
			SetCurrentTypeByVerbForm(deepr);
		}
		#endregion

		#region [.hierarchy methods.]
		/// <summary>
		/// Создание нового подпредложения по форме глагола
		/// </summary>
		/// <param name="verbForm">форма глагола</param>
		private void CreateNewSubSentenceByVerbForm(Entity verbForm)
		{
			#region [.defense.]
			if (verbForm.UnitTextType != UnitTextType.VERB)
				throw new ArgumentException("entity isn't verbForm");
			#endregion
			Entity lastVerb = CurrentSubSentence.Verbs.Last();
			UnitTextBase current = verbForm;
			while (current.Previous != lastVerb)
			{
                if (DictionaryResource.IsHomogeneousConjunction(current.Text) || current.IsSeparator(TireForms) || current.IsSeparator(","))
					break;
				current = current.Previous;
			}
			CreateNewSubSentence(current.Previous);
			AddVerbFormToCurrent(verbForm);
		}

		/// <summary>
		/// Создание нового подпредложения
		/// </summary>
		/// <param name="last">последний юнит текущего подпредложения</param>
		public void CreateNewSubSentence(UnitTextBase last, bool isStrongPosition = false)
		{
			var separateUnit = isStrongPosition ? GetNextNearUnitSkipEmpty(last) : last.GetNonEmptyNext();
			CurrentSubSentence.SubSentence.SetSubSentenceUnits(CurrentSubSentence.StartUnit, separateUnit);
			_subSentenceInfoList.Add(CurrentSubSentence);

			if (separateUnit != null)
				CurrentSubSentence = new SubSentenceInfo(separateUnit, separateUnit.PositionInfo.Start);
			else
				CurrentSubSentence = null;
		}

		/// <summary>
		/// Получение иерархии подпредложений
		/// </summary>
		/// <returns>коллекция родительских подпредложений</returns>
		public SubSentence[] GetHierarchy()
		{
			FillSelectedSubSentencesBySource();
			AdjunctPhraseTypization();

			List<SubSentenceInfo> topList = new List<SubSentenceInfo>();
			CurrentSubSentence = PopFirstTop();
			SetSubjectInCurrent(null);

			topList.Add(CurrentSubSentence);
			AddSelectedSubSentencesToHierarchy(topList);
			_subSentenceInfoList.Add(topList.First());

			FillSubSentencesHierarchy(_subSentenceInfoList);
			return topList.Select(_ => _.SubSentence).ToArray();
		}

		/// <summary>
		/// Заполнение выделенных подпредложений юнитами и дочерними подпредложениями из исходного подпредложения
		/// </summary>
		private void FillSelectedSubSentencesBySource()
		{
			if ((CurrentSubSentence != null) && (CurrentSubSentence.StartUnit != null))
				CreateNewSubSentence(_sourceSubSentence.Units.Last());
			var subSentences = _subSentenceInfoList.Select(_ => _.SubSentence).ToArray();
			_sourceSubSentence.Units.DecomposedUnits(subSentences);
			_sourceSubSentence.Children.DecomposedSubSentences(subSentences);
		}

		/// <summary>
		/// Заполнение иерархии подпредложений в заданной коллекции
		/// </summary>
		/// <param name="collection">коллекция инфомарции о подпредложениях</param>
		private void FillSubSentencesHierarchy(IEnumerable<SubSentenceInfo> collection)
		{
			var groups = collection.GroupBy(_ => _.Parent).Where(_ => _.Key != null).ToDictionary(_ => _.Key);
			foreach (var subSentenceInfo in collection)
			{
                subSentenceInfo.SubSentence.ParentObject = _sourceSubSentence.ParentObject;
				IGrouping<SubSentenceInfo, SubSentenceInfo> children = null;
                if (groups.TryGetValue(subSentenceInfo, out children))
                    subSentenceInfo.SubSentence.AppendSubSentences(children.Select(_ => _.SubSentence));
                else
                {
                    foreach (var child in subSentenceInfo.SubSentence.Children)
                        child.ParentObject = subSentenceInfo.SubSentence.ParentObject;
                }
			}
		}

		/// <summary>
		/// Добавление выделенных подпредложений в иерархию
		/// </summary>
		/// <param name="topList">список подпредложений верхнего уровня</param>
		private void AddSelectedSubSentencesToHierarchy(List<SubSentenceInfo> topList)
		{
			var previous = CurrentSubSentence;
			CurrentSubSentence = null;
			foreach (var info in _subSentenceInfoList)
			{
				previous = GetPreviousSubSentenceInfo(info, previous, CurrentSubSentence);
				CurrentSubSentence = info;
				SetSubjectInCurrent(previous);
				AddCurrentSubSentenceToHierarchy(topList, previous);
			}
		}

		/// <summary>
		/// Получение информации по предыдущему подпредложению для заданного текущего подпредложения
		/// </summary>
		/// <param name="current">информация о текущем подпредложении</param>
		/// <param name="previous">информация о предыдущем подпредложении для последнего добавленного</param>
		/// <param name="last">информация о последнем добавленном подпредложении</param>
		/// <returns>информация о предыдущем подпредложении для текущего</returns>
		private SubSentenceInfo GetPreviousSubSentenceInfo(SubSentenceInfo current, SubSentenceInfo previous, SubSentenceInfo last)
		{
			if ((last != null) && (previous != last))
			{
				SubSentenceInfo parent = previous.Parent;

				if (parent == null)
				{
					if ((previous.SubSentence.GetLastUnitStartPosition() < last.SubSentence.GetLastUnitStartPosition()) ||
						(current.SubSentence.GetLastUnitStartPosition() < previous.SubSentence.GetLastUnitStartPosition()))
					{
						previous = last;
					}
				}
				else if (previous.SubSentence.GetLastUnitStartPosition() <= last.SubSentence.GetLastUnitStartPosition())
					previous = GetPreviousSubSentenceInfo(current, parent, last);
				#region [.defense.]
				else
					throw new InvalidOperationException("Very complicated hierarchy");
				#endregion
			}
			return previous;
		}

		/// <summary>
		/// Добавление в иерархию текущего предложения
		/// </summary>
		/// <param name="topList">список подпредложений верхнего уровня</param>
		/// <param name="previous">последнее добавленное подпредложение</param>
		private void AddCurrentSubSentenceToHierarchy(List<SubSentenceInfo> topList, SubSentenceInfo previous)
		{
			switch (CurrentSubSentence.SubSentence.Type)
			{
				case SubSentenceType.Default:
					AddCurrentDefaultToHierarchy(topList, previous);
					break;
				case SubSentenceType.DanglingParticiple:
				case SubSentenceType.Participial:
					AddCurrentDanglingParticipleOrParticipialToHierarchy(previous);
					break;
				case SubSentenceType.Subordinate:
				case SubSentenceType.AdjunctPhrase:
					previous.AddChild(CurrentSubSentence);
					break;
			}
		}

		/// <summary>
		/// Добавление в иерархию текущего дефолтного подпредложения
		/// </summary>
		/// <param name="topList">список подпредложений верхнего уровня</param>
		/// <param name="lastAdded">последнее добавленное подпредложение</param>
		private void AddCurrentDefaultToHierarchy(List<SubSentenceInfo> topList, SubSentenceInfo lastAdded)
		{
			SubSentenceInfo brother = lastAdded;
			if (GetCurrentBrother(ref brother, lastAdded))
			{
				brother.Append(CurrentSubSentence);
				CurrentSubSentence = brother;
			}
			else if (brother.Parent != null)
				brother.Parent.AddChild(CurrentSubSentence);
			else
				topList.Add(CurrentSubSentence);
		}

		/// <summary>
		/// Получение подпредложения, расположенного на том же уровне, куда надо вставлять текущее
		/// дополнительно указывается флаг, следует ли соединить это подпредложение с текущим
		/// </summary>
		/// <param name="brother">результирующее подпредложение</param>
		/// <param name="lastAdded">последнее добавленное подпредложение</param>
		/// <returns>флаг, следует ли соединить результирующее подпредложение с текущим</returns>
		private bool GetCurrentBrother(ref SubSentenceInfo brother, SubSentenceInfo lastAdded)
		{
			if (brother.Parent != null)
			{
				SubSentenceInfo previousSubSentence = GetRightBorderSubSentence(brother.Parent);
				if (!CurrentSubSentence.IsExistSubject())
				{
					bool isPartOfPrevious = IsCurrentPartOfPrevious(previousSubSentence, lastAdded);
					if (isPartOfPrevious || CurrentSubSentence.IsExistSubject())
					{
						brother = previousSubSentence;
						return isPartOfPrevious;
					}
				}

				if (CurrentSubSentence.IsContainConjunction && (previousSubSentence != null))
				{
					brother = previousSubSentence;
					return false;
				}

				brother = brother.Parent;
				if (IsConjunctionSeria(previousSubSentence))
					return true;
				else
					return GetCurrentBrother(ref brother, lastAdded);
			}
			return IsCurrentPartOfPrevious(brother, lastAdded);
		}

		/// <summary>
		/// Проверка, что подпредложение образовано на стыке союзов
		/// </summary>
		/// <param name="subSentence">подпредложение</param>
		/// <returns>результат проверки</returns>
		private bool IsConjunctionSeria(SubSentenceInfo subSentence)
		{
			if ((subSentence != null) && subSentence.IsContainConjunction && (subSentence.Parent != null))
			{
				Entity subSentenceFirst = (Entity)subSentence.SubSentence.Units.First(_ => _.IsEntity);
				Entity parentLast = (Entity)subSentence.Parent.SubSentence.Units.LastOrDefault(_ => _.IsEntity);
				return (parentLast != null) && IsConjunction(subSentenceFirst) && IsConjunction(parentLast);
			}
			return false;
		}

		/// <summary>
		/// Проверка, что текущее подпредложение должно быть частью предыдущего подпредложения
		/// </summary>
		/// <param name="previous">предыдущее подпредложение</param>
		/// <param name="lastAdded">последнее добавленное подпредложение</param>
		/// <returns>результат проверки</returns>
		private bool IsCurrentPartOfPrevious(SubSentenceInfo previous, SubSentenceInfo lastAdded)
		{
			if ((previous != null) && !CurrentSubSentence.IsExistSubject())
			{
				bool isPossiblePart = true;
				if (previous == lastAdded)
				{
					UnitTextBase firstUnit = CurrentSubSentence.SubSentence.Units.FirstOrDefault(_ => !_.IsEmptyText());
					if ((firstUnit != null) && !DictionaryResource.IsHomogeneousConjunction(firstUnit.Text))
					/// подпредложения не будут соединены
					{
						isPossiblePart = false;

						UnitTextBase secondUnit = firstUnit.GetNonEmptyNext();
						if (IsAnyType(previous.SubSentence, SubSentenceType.Subordinate) && !(CurrentSubSentence.IsContainConjunction ||
							(firstUnit.IsSeparator(",") && (secondUnit != null) && DictionaryResource.IsHomogeneousConjunction(secondUnit.Text))))
						/// не пытаемся восстановить подлежащее
						{
							return false;
						}
					}
				}

				if (IsAnyType(previous.SubSentence, SubSentenceType.Default, SubSentenceType.Subordinate))
				{
					if (IsCurrentSubject(previous.SubSentence.Subject))
					{
						if (CurrentSubSentence.IsContainConjunction || (CurrentSubSentence.IsExistVerb() && previous.IsExistVerb()))
							CurrentSubSentence.SubSentence.set_SubjectSubSentence(previous.SubSentence);
						else
							return isPossiblePart;
					}
				}
				else if (!CurrentSubSentence.IsContainConjunction)
					return isPossiblePart && IsCurrentPartOfPrevious(previous);
			}
			return false;
		}

		/// <summary>
		/// Проверка, что текущее подпредложение должно быть частью предыдущего подпредложения
		/// </summary>
		/// <param name="previous">предыдущее подпредложение</param>
		/// <returns>результат проверки</returns>
		private bool IsCurrentPartOfPrevious(SubSentenceInfo previous)
		{
			#region [.defense.]
			if (CurrentSubSentence.IsContainConjunction)
				throw new InvalidOperationException("Current subsentence have conjunction");
			#endregion

			bool result = false;
			switch (previous.SubSentence.Type)
			{
				case SubSentenceType.DanglingParticiple:
				case SubSentenceType.Participial:
					result = !(CurrentSubSentence.IsExistVerb() || CurrentSubSentence.IsExistSubject());
					break;
				case SubSentenceType.Default:
				case SubSentenceType.Subordinate:
					result = !(CurrentSubSentence.IsExistVerb() && previous.IsExistVerb()) &&
						!((CurrentSubSentence.IsHasLingvisticEntity() || CurrentSubSentence.SubSentence.Children.Any()) &&
						(previous.IsHasLingvisticEntity() || previous.SubSentence.Children.Any()));
					break;
			}
			return result;
		}

		/// <summary>
		/// Получение подпредложения, являющегося самым последним элементом заданного родительского подпредложения
		/// </summary>
		/// <param name="parent">родительское подпредложение</param>
		/// <returns>подпредложение, являющееся самым последним элементом родительского подпредложения</returns>
		private SubSentenceInfo GetRightBorderSubSentence(SubSentenceInfo parent)
		{
			SubSentenceInfo result = parent.Childs.LastOrDefault();
			if (result != null)
			{
				int subSentenceStart = result.SubSentence.StartPosition;
				foreach (var unit in parent.SubSentence.Units)
				{
					if (unit.PositionInfo.Start > subSentenceStart)
					{
						result = null;
						break;
					}
				}
			}
			return result;
		}

		/// <summary>
		/// Добавление в иерархию текущего подпредложения, являющегося причастным или деепричастным оборотом
		/// </summary>
		/// <param name="lastAdded">последнее добавленное подпредложение</param>
		private void AddCurrentDanglingParticipleOrParticipialToHierarchy(SubSentenceInfo lastAdded)
		{
			#region [.defense.]
			if (lastAdded == null)
				throw new ArgumentNullException("lastAdded");
			if ((CurrentSubSentence.SubSentence.Type != SubSentenceType.DanglingParticiple) &&
				(CurrentSubSentence.SubSentence.Type != SubSentenceType.Participial))
			{
				throw new ArgumentException("Current subsentence isn't DanglingParticiple or Participial");
			}
			#endregion

			SubSentenceInfo parent = null;
			if (CurrentSubSentence.IsExistSubject())
				parent = lastAdded;
			else /// поиск определяемого слово в других подпредложениях в иерархии
				parent = FindParentWithDeterminerUpToHierarchy(lastAdded);
			
			if ((CurrentSubSentence.IsExistSubject()) &&
				(CurrentSubSentence.SubSentence.Type == parent.SubSentence.Type) &&
				(CurrentSubSentence.SubSentence.Subject == parent.SubSentence.Subject))
			/// определяемое слово совпадает с определяемым словом родительского подпредложения того же типа - 
			/// сделаем их подпредложениями одного уровня
			{
				parent = lastAdded.Parent;
			}
			parent.AddChild(CurrentSubSentence);
		}

		/// <summary>
		/// Поиск вверх по иерархии родительского подпредложения, в котором содержится определяемое слово для текущего
		/// </summary>
		/// <param name="parent">первое возможное родительское подпредложение</param>
		/// <returns>родительское подпредложение</returns>
		private SubSentenceInfo FindParentWithDeterminerUpToHierarchy(SubSentenceInfo parent)
		{
			SubSentenceInfo result = parent;
			while (!CurrentSubSentence.IsExistSubject() && (result.Parent != null))
			{
				SetCurrentDeterminer(result.Parent);
				result = result.Parent;
			}
			return CurrentSubSentence.IsExistSubject() ? result : parent;
		}

		/// <summary>
		/// Установка субъекта для текущего подпредложения
		/// </summary>
		/// <param name="parent">родительское подпредложение</param>
		private void SetSubjectInCurrent(SubSentenceInfo parent)
		{
			SubSentenceType currentType = CurrentSubSentence.SubSentence.Type;
			if (currentType == SubSentenceType.DanglingParticiple)
				SetCurrentDanglingParticipleDeterminer(parent);
			else if (currentType == SubSentenceType.Participial)
				SetCurrentParticipialDeterminer(parent);
			else
				SetCurrentSubject();
		}

		/// <summary>
		/// Установка подлежащего для текущего подпредложения
		/// </summary>
		private void SetCurrentSubject()
		{
			List<Entity> subjects = new List<Entity>();
			foreach (UnitTextBase unit in CurrentSubSentence.SubSentence.Units.Where(_ => _.IsEntity))
			{
				Entity entity = (Entity)unit;
				if (entity.IsSentenceMember() && IsCurrentSubject(entity))
					subjects.Add(entity);
			}
			if (subjects.Any())
			{
				CurrentSubSentence.SubSentence.Subject = subjects.FirstOrDefault(_ => _.IsSubject());
				if (!CurrentSubSentence.IsExistSubject())
					CurrentSubSentence.SubSentence.Subject = subjects.FirstOrDefault();
			}
		}

		/// <summary>
		/// Проверка, что заданная сущность - подлежащее текущего подпредложения
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <returns>результат проверки</returns>
		private bool IsCurrentSubject(Entity entity)
		{
			if (entity == null)
				return false;
			else
			{
				if (IsChildIndefinitePronoun(entity))
					return false;
				else if (entity.IsSubject())
					return true;
				else
				{
					Entity firstVerb = CurrentSubSentence.Verbs.FirstOrDefault(_ => !_.IsType(EntityType.Infinitive));
					return entity.IsPossibleSubject() && ((firstVerb == null) || IsVerbConsistWithNoun(firstVerb, entity));
				}
			}
		}

		/// <summary>
		/// Установка определяемого слова для текущего подпредложения, являющегося причастным или деепричастным оборотом
		/// </summary>
		/// <param name="parent">родительское подпредложение</param>
		private void SetCurrentDeterminer(SubSentenceInfo parent)
		{
			if (CurrentSubSentence.SubSentence.Type == SubSentenceType.DanglingParticiple)
				SetCurrentDanglingParticipleDeterminer(parent);
			else if (CurrentSubSentence.SubSentence.Type == SubSentenceType.Participial)
				SetCurrentParticipialDeterminer(parent);
			#region [.defense.]
			else
				throw new ArgumentException("Current subsentence isn't DanglingParticiple or Participial");
			#endregion
		}

		/// <summary>
		/// Установка определяемого слова для текущего подпредложения, являющегося деепричастным оборотом
		/// </summary>
		/// <param name="parent">родительское подпредложение</param>
		private void SetCurrentDanglingParticipleDeterminer(SubSentenceInfo parent)
		{
			if (parent.IsExistSubject())
				CurrentSubSentence.SubSentence.set_SubjectSubSentence(parent.SubSentence);
			else
			/// определяемое слово - одушевленная сущность в дательном падеже
			{
				var dativeAnimation = parent.SubSentence.Units
					.Where(_ => _.UnitTextType == UnitTextType.ENTITY)
					.Select(_ => (Entity)_)
					.Where(_ => IsDativeAnimationEntity(_));
				if (dativeAnimation.Count() == 1)
					CurrentSubSentence.SubSentence.Subject = dativeAnimation.First();
			}
		}

		/// <summary>
		/// Установка определяемого слова для текущего подпредложения, являющегося причастным оборотом
		/// </summary>
		/// <param name="parent">родительское подпредложение</param>
		private void SetCurrentParticipialDeterminer(SubSentenceInfo parent)
		{
			Entity participle = CurrentSubSentence.Participles.First();
			Entity lastEntity = (Entity)parent.SubSentence.Units.LastOrDefault(_ => _.UnitTextType == UnitTextType.ENTITY);
			if (lastEntity != null)
				CurrentSubSentence.SubSentence.Subject = GetConsistencyEntity(lastEntity, participle);
			if (!CurrentSubSentence.IsExistSubject() && (parent.SubSentence.Type == SubSentenceType.Participial))
				if (IsConsistency(parent.Participles.First(), participle))
					CurrentSubSentence.SubSentence.set_SubjectSubSentence(parent.SubSentence);
		}

		/// <summary>
		/// Извлечение из списка подпредложений первой вершины
		/// </summary>
		/// <returns>первая вершина</returns>
		private SubSentenceInfo PopFirstTop()
		{
			SubSentenceInfo result = PopFirstSubSentence(SubSentenceType.Default);
			if (result == null)
			/// исключение: вообще нет дефолтного предложения - сделаем вершиной подчиненное
			{
				result = PopFirstSubSentence(SubSentenceType.Subordinate);
				if (result == null)
				/// исключение: нет и подчиненного - сделаем вершиной первое
				{
					var firstChild = _subSentenceInfoList.First();
					result = new SubSentenceInfo(firstChild.SubSentence.StartPosition);
					result.SubSentence.SetUnits(new UnitTextBase[] { });
				}
			}
			return result;
		}

		/// <summary>
		/// Извлечение из списка подпредложений первого подпредложения,
		/// которое является подпредложением одного из заданных типов
		/// </summary>
		/// <param name="types">коллекция типов</param>
		/// <returns>первое подходящее предложение</returns>
		private SubSentenceInfo PopFirstSubSentence(params SubSentenceType[] types)
		{
			SubSentenceInfo result = null;
			for (int i = 0; i < _subSentenceInfoList.Count; ++i)
			{
				if (IsAnyType(_subSentenceInfoList[i].SubSentence, types))
				{
					result = _subSentenceInfoList[i];
					_subSentenceInfoList.RemoveAt(i);
					break;
				}
			}
			return result;
		}

		/// <summary>
		/// Поиск согласованной сущности для заданной
		/// </summary>
		/// <param name="parentEntity">главная сущность для поиска согласованных</param>
		/// <param name="entity">сущность, с которой проверяется согласованность</param>
		/// <returns>согласованная сущность</returns>
		private Entity GetConsistencyEntity(Entity parentEntity, Entity entity)
		{
			if (parentEntity.IsSentenceMember() && IsConsistency(parentEntity, entity))
				return parentEntity;
			foreach (var child in parentEntity.Children)
			{
				Entity consistencyEntity = GetConsistencyEntity(child, entity);
				if (consistencyEntity != null)
					return consistencyEntity;
			}
			return null;
		}
		#endregion

		#region [.help functions.]
		/// <summary>
		/// Проверка, что заданная сущность - дочернее неопределенное местоимение
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <returns>результат проверки</returns>
		private bool IsChildIndefinitePronoun(Entity entity)
		{
			UnitTextBase next = entity.GetNonEmptyNext();
			return entity.IsType(EntityType.Pronoun) && DictionaryResource.IsIndefinitePronoun(entity.Text) &&
				(next != null) && (next.UnitTextType == UnitTextType.ENTITY);
		}

		/// <summary>
		/// Типизация обстоятельственных оборотов среди выделенных подпредложений
		/// </summary>
		private void AdjunctPhraseTypization()
		{
			if (_subSentenceInfoList.Count == 1)
				return;
			
			foreach (var subSentencesInfo in _subSentenceInfoList)
			{
				if (IsAdjunctPhrase(subSentencesInfo.SubSentence))
				{
					if (subSentencesInfo.SubSentence.Type == SubSentenceType.DanglingParticiple)
						subSentencesInfo.Verbs = subSentencesInfo.Verbs.Where(_ => _.IsType(EntityType.Verb)).ToList();

					subSentencesInfo.SubSentence.Type = SubSentenceType.AdjunctPhrase;
				}
			}
		}

		/// <summary>
		/// Проверка, что заданное подпредложение - обстоятельственный оборот
		/// </summary>
		/// <param name="subSentence">подпредложение</param>
		/// <returns>результат проверки</returns>
		private bool IsAdjunctPhrase(SubSentence subSentence)
		{
			return DictionaryResource.IsStartWithAdjunct(subSentence.Units.GetText());
		}

		/// <summary>
		/// Получение следующего юнита, который стоит рядом с пропуском юнитов без текста
		/// </summary>
		/// <param name="startUnit">юнит, с которого начинается поиск</param>
		/// <returns>юнит</returns>
		private UnitTextBase GetNextNearUnitSkipEmpty(UnitTextBase startUnit)
		{
			UnitTextBase result = startUnit;
			UnitTextBase next = result.Next;
			while (true)
			{
				if ((next == null) || !next.IsEmptyText() || (next.PositionInfo.Start != result.PositionInfo.End))
					break;
				result = next;
				next = next.Next;
			}
			return result;
		}

		/// <summary>
		/// Установка типа подпредложения по форме глагола
		/// </summary>
		/// <param name="verb">форма глагола</param>
		private void SetCurrentTypeByVerbForm(Entity verb)
		{
			switch (verb.Type.EntityType)
			{
				case EntityType.Verb:
					if (!CurrentSubSentence.IsExistVerb() && !CurrentSubSentence.IsContainConjunction)
						CurrentSubSentence.SubSentence.Type = SubSentenceType.Default;
					break;
                case EntityType.Infinitive:
                    if (!CurrentSubSentence.IsExistVerb() && !CurrentSubSentence.IsContainConjunction &&
                        !CurrentSubSentence.Participles.Any())
                    {
                        CurrentSubSentence.SubSentence.Type = SubSentenceType.Default;
                    }
                    break;
				case EntityType.Deepr:
					CurrentSubSentence.SubSentence.Type = SubSentenceType.DanglingParticiple;
					break;
				case EntityType.Participle:
                    if (!CurrentSubSentence.IsExistVerb() && !CurrentSubSentence.IsContainConjunction)
						CurrentSubSentence.SubSentence.Type = SubSentenceType.Participial;
					break;
				#region [.defense.]
				default:
					throw new ArgumentException("entity isn't verb");
				#endregion
			}
		}

		/// <summary>
		/// Проверка, что заданная сущность - союз
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <returns>результат проверки</returns>
		private bool IsConjunction(Entity entity)
		{
			return entity.IsType(EntityType.Conjunction);
		}

		/// <summary>
		/// Проверка, что заданное подпредложение является подпредложением одного из заданных типов
		/// если коллекция типов - null, то вернется true
		/// </summary>
		/// <param name="subSentence">подпредложение</param>
		/// <param name="types">коллекция типов</param>
		/// <returns>результат проверки</returns>
		private bool IsAnyType(SubSentence subSentence, params SubSentenceType[] types)
		{
			if (types == null)
				return true;
			foreach (var type in types)
				if (subSentence.Type == type)
					return true;
			return false;
		}

		/// <summary>
		/// Проверка, что заданная сущность - одушевленное в дательном падеже
		/// </summary>
		/// <param name="entity">сущность</param>
		/// <returns>результат проверки</returns>
		private bool IsDativeAnimationEntity(Entity entity)
		{
			foreach (var morpho in entity.EntityInfo.Morpho)
			{
				if ((morpho.Case == Case.Dative) &&
					((morpho.Animation == AnimationType.Animation) || entity.IsType(EntityType.Pronoun)))
				{
					return true;
				}
			}
			return false;
		}

		/// <summary>
		/// Проверка на пассивный залог
		/// </summary>
		/// <param name="firstVerb">первый глагол</param>
		/// <param name="secondVerb">второй глагол</param>
		/// <returns>результат проверки</returns>
		private bool IsPassiveVoice(Entity firstVerb, Entity secondVerb)
		{
			#region[.defense.]
			if (firstVerb == null)
				throw new ArgumentNullException("firstVerb");
			else if (!firstVerb.IsType(EntityType.Verb))
				throw new ArgumentException("firstVerb isn't Verb");
			if (secondVerb == null)
				throw new ArgumentNullException("secondVerb");
			else if (!secondVerb.IsType(EntityType.Verb))
				throw new ArgumentException("secondVerb isn't Verb");
			#endregion

			foreach (var firstMorpho in firstVerb.EntityInfo.Morpho)
			{
				foreach (var secondMorpho in secondVerb.EntityInfo.Morpho)
				{
					/// один - пассив, другой - актив
					bool isPassiveActivePair = ((firstMorpho.Voice == Voice.Active) && (secondMorpho.Voice == Voice.Passive)) ||
						((firstMorpho.Voice == Voice.Passive) && (secondMorpho.Voice == Voice.Active));
					/// одинаковое число
					bool isEqualNumber = firstMorpho.Number == secondMorpho.Number;
					if (isPassiveActivePair && isEqualNumber)
						return true;
				}
			}
			return false;
		}

		/// <summary>
		/// Проверка согласованности двух сущностей
		/// </summary>
		/// <param name="first">первая сущность</param>
		/// <param name="second">вторая сущность</param>
		/// <returns>результат проверки</returns>
		private bool IsConsistency(Entity first, Entity second)
		{
			#region[.defense.]
			if (first == null)
				throw new ArgumentNullException("first");
			if (second == null)
				throw new ArgumentNullException("second");
			#endregion

			if (first.IsType(EntityType.Adverb) || second.IsType(EntityType.Adverb))
				return false;

			foreach (var firstMorpho in first.EntityInfo.Morpho)
			{
				foreach (var secondMorpho in second.EntityInfo.Morpho)
				{
					if (IsIntersection(firstMorpho.Case, secondMorpho.Case) &&
						IsIntersection(firstMorpho.Animation, secondMorpho.Animation) &&
						IsNumberAndGenderIntersection(firstMorpho, secondMorpho))
					{
						return true;
					}
				}
			}
			return false;
		}

		/// <summary>
		/// Проверка согласованности глагола с существительным
		/// </summary>
		/// <param name="verb">глагол</param>
		/// <param name="noun">существительное</param>
		/// <returns>результат проверки</returns>
		private bool IsVerbConsistWithNoun(Entity verb, Entity noun)
		{
			#region [.defense.]
			if (verb == null)
				throw new ArgumentNullException("verb");
			else if (!verb.IsType(EntityType.Verb))
				throw new ArgumentException("entity isn't verb");
			if (noun == null)
				throw new ArgumentNullException("noun");
			#endregion

			foreach (var verbMorpho in verb.EntityInfo.Morpho)
			{
				foreach (var nounMorpho in noun.EntityInfo.Morpho)
				{
					if (IsNumberAndGenderIntersection(nounMorpho, verbMorpho))
						return true;
				}
			}
			return false;
		}

		/// <summary>
		/// Проверка согласованности падежей
		/// </summary>
		/// <param name="first">падеж первого морфологического варианта</param>
		/// <param name="second">падеж второго морфологического вариант</param>
		/// <returns>результат проверки</returns>
		private bool IsIntersection(Case first, Case second)
		{
			return (first == second) ||
				(first == Case.Undefined) ||
				(second == Case.Undefined);
		}

		/// <summary>
		/// Проверка согласованности одушевленности
		/// </summary>
		/// <param name="first">одушевленность первого морфологического варианта</param>
		/// <param name="second">одушевленность второго морфологического варианта</param>
		/// <returns>результат проверки</returns>
		private bool IsIntersection(AnimationType first, AnimationType second)
		{
			return (first == second) ||
				(first == AnimationType.Undefined) ||
				(second == AnimationType.Undefined);
		}

		/// <summary>
		/// Проверка согласованности двух морфологических вариантов по числу и роду
		/// </summary>
		/// <param name="first">первый морфологический вариант</param>
		/// <param name="second">второй морфологический вариант</param>
		/// <returns>результат проверки</returns>
		private bool IsNumberAndGenderIntersection(MorphoInformation first, MorphoInformation second)
		{
			if (second.Number == Number.Plural)
				return true;
			else if ((first.Number == second.Number) || (second.Number == Number.Undefined))
				return IsIntersection(first.Gender, second.Gender);
			return false;
		}

		/// <summary>
		/// Проверка согласованности родов
		/// </summary>
		/// <param name="first">род первого морфологического варианта</param>
		/// <param name="second">род второго морфологического варианта</param>
		/// <returns>результат проверки</returns>
		private bool IsIntersection(Gender first, Gender second)
		{
			return (first == second) ||
				(first == Gender.Undefined) ||
				(second == Gender.Undefined);
		}
		#endregion
	}
}
