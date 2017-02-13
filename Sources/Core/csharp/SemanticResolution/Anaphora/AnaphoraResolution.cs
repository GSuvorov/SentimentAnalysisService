using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using Linguistics.Core;
using Linguistics.Core.Morpho;

namespace SemanticResolution
{
    public class AnaphoraResolution
    {
        /// <summary>
        /// разрешение анафории союзных слов в предложении
        /// </summary>
        /// <param name="sentence"></param>
        static private void ResolveSentenceAnaphoraConjunction(XElement sentence)
        {
			var subordinateSentences =
				from node in sentence.Descendants()
				where node.Name.IsEqual(UnitTextType.SUB_SENT) && 
				node.Attribute(BlockAttribute.TYPE).IsEqual(SubSentenceType.Subordinate)
				select node;
			foreach (XElement subSentence in subordinateSentences)
            {
                var conjunctionList =
                    from element in subSentence.Elements(UnitTextType.CET)
                    where element.Attribute(BlockAttribute.TYPE).IsEqual(EntityType.Conjunction)
                    select element;

                foreach (XElement conjunction in conjunctionList)
					ResolveConjunctionAnaphora(conjunction, subSentence.ElementsBeforeSelf().LastEntityChain(), subSentence);
            }
        }

		static private void ResolveConjunctionAnaphora(XElement conjunction, XElement entityChain, XElement sentence)
		{
			if ((entityChain == null) || !entityChain.Name.IsEqual(UnitTextType.ENTITY))
				return;

			XElement consistency = null;
			EntityRole role = EntityRole.Other;
			bool isNeedSetRole = true;
			string value = conjunction.Attribute(BlockAttribute.VALUE);
			switch (value)
			{
				case "который":
					/// морфологические свойства союза
					var conjMorpho =
					(from info in conjunction.GetMorphoInfo()
					 where !info.IsEmpty()
					 select new {
						 _case = info[MorphoInfo.Case],
						 _gender = info[MorphoInfo.Gender],
						 _number = info[MorphoInfo.Number]
					 }).ToArray();
					/// минимальная согласованная с союзом цепочка
					XElement[] consistElements = entityChain.DescendantsAndSelf().Where(
						element =>
						{
							if (element.Attribute(BlockAttribute.MI).IsEmpty() || !element.Name.IsEqual(UnitTextType.ENTITY))
								return false;
							var morpho =
								from info in element.GetMorphoInfo()
								select new { _gender = info[MorphoInfo.Gender], _number = info[MorphoInfo.Number] };
							var intersect =
								from conj in conjMorpho
								from info in morpho
								where
								(info._number == conj._number) &&
								/// множественное число
								(conj._number.IsEqualCode(Number.Plural) ||
								/// единственное число
								(info._gender == conj._gender))
								select info;
							return intersect.Any();
						}).ToArray();
					bool isHomogeneous = entityChain.Attribute(BlockAttribute.TYPE).IsEqual(EntityType.Homogeneous);
					if ((consistElements.Length == 1) || ((isHomogeneous && consistElements.FirstOrDefault() == entityChain)))
					/// удалось разрешить
					{
						consistency = consistElements.First();
						isNeedSetRole = false;
						var info =
							from morpho in conjMorpho
							where !morpho._case.IsEqualCode(Case.Nominative) && !morpho._case.IsEqualCode(Case.Accusative)
							select morpho;
						XElement previos = conjunction.ElementsBeforeSelf().LastOrDefault(
							element => !element.GetFullValue().Trim().IsEmpty());
						if (info.Any() && (previos != null) && previos.Name.IsEqual(UnitTextType.ENTITY) &&
							!previos.Attribute(BlockAttribute.TYPE).IsEqual(EntityType.Adverb))
						/// формирование новой сущности
						{
							List<XElement> elements = sentence.Elements().ToList();
							elements.UnionEntity(elements.IndexOf(previos), conjunction);
							sentence.ReplaceNodes(elements);
						}
					}
					break;
				/*case "что":
					if(!sentence.IsContainAttribute(BlockAttribute.SUBJ))
					/// минимальная согласованная с глаголом цепочка становится субъектом
					{
						var verbList =
							(from entity in conjunction.ElementsAfterSelf()
							 from child in entity.DescendantsAndSelf()
							 where child.Attribute(BlockAttribute.TYPE).IsEqual(EntityType.Verb)
							 select child).ToArray();
						/// минимальная согласованная с глаголом цепочка
						consistency = entityChain.DescendantsAndSelf().LastOrDefault(
							element =>
							{
								foreach (var verb in verbList)
									if (!verb.IsConsistWithNoun(element))
										return false;
								return true;
							});
						if (consistency != null)
						/// удалось разрешить
						{
							role = EntityRole.Subj;
							sentence.SetAttributeValue(BlockAttribute.SUBJ.ToString(),
								consistency.Attribute(BlockAttribute.ID));
						}
					}
					break;
				case "где":
				case "куда":
				case "откуда":
					/// первая цепочка географии
					consistency = entityChain.DescendantsAndSelf().FirstOrDefault(
						element =>
						{
							string type = element.Attribute(BlockAttribute.TYPE);
							if (type.IsEqual(EntityType.PlaceName) ||
								type.IsEqual(EntityType.PlaceNoInfo))
							{
								return true;
							}
							return false;
						});
					role = EntityRole.Adj;
					break;
				case "когда":
					/// первая дата
					consistency = entityChain.DescendantsAndSelf().FirstOrDefault(
						element => element.Attribute(BlockAttribute.TYPE).IsEqual(EntityType.Date));
					role = EntityRole.Adj;
					break;*/
			}
			if (consistency != null)
			{
				conjunction.SetLinkToElement(consistency);
				if (isNeedSetRole)
					conjunction.SetRole(role);
			}
			else
				conjunction.DeleteAttribute(BlockAttribute.ROLE);
		}

		/// <summary>
		/// разрешение анафории местоимений в предложении
		/// </summary>
		/// <param name="sentence">предложение</param>
		/// <param name="mainSentence">главное предложение</param>
		/*static private void ResolveSentencePronounAnaphora(XElement sentence, XElement mainSentence)
		{
			foreach (XElement element in sentence.Elements())
			{
				if (element.Name.IsEqual(NodeName.SUB_SENT))
					ResolveSentencePronounAnaphora(element, mainSentence);
				else if (element.Name.IsEqual(NodeName.SENT))
					ResolveSentencePronounAnaphora(element, element);
				else if (element.Name.IsEqual(NodeName.ENTITY))
				{
					foreach (XElement node in element.DescendantsAndSelf())
					{
						if (!node.Attribute(BlockAttribute.TYPE).IsEqual(EntityType.Pronoun))
							continue;

						string value = element.Attribute(BlockAttribute.VALUE);
						/// лицо местоимения
						char faceType =
							(from morpho in element.GetMorphoInfo()
								where !morpho.IsEmpty() && !morpho[Morpho.Face].IsEqualCode(Face.Undefined)
								select morpho[Morpho.Face]).FirstOrDefault();
						if (faceType.IsEqualCode(Face.Third))
							ResolvePronounAnaphora(node, sentence, mainSentence);
					}
				}
			}
		}

		/// <summary>
		/// число онализируемых предложений для разрешения анафории
		/// </summary>
		private static readonly int _analizingSentenceCount = 2;

		/// <summary>
		/// разрешение анафории местоимения
		/// </summary>
		/// <param name="pronoun">местоимение</param>
		/// <param name="sentence">подпредложение, содержащее местоимение</param>
		/// <param name="sentence">главное предложение</param>
		static private void ResolvePronounAnaphora(XElement pronoun, XElement sentence, XElement mainSentence)
		{
			List<AnaphoraCandidat> candidatList = new List<AnaphoraCandidat>();

			/// текущее расстояние в подпредложениях
			int sentenceDistance = 0;
			/// цепочка, содержащая местоимение
			XElement pronounChain = pronoun.AncestorsAndSelf().LastEntityChain();
			var previos = pronounChain.ElementsBeforeSelf().LastOrDefault(
				_ => !_.GetFullValue().Trim().IsEmpty() && !_.Attribute(BlockAttribute.TYPE).IsEqual(EntityType.Pretext));
			/// препозитивное причастие
			bool isPrepositionParticiple = ((previos != null) && previos.Attribute(BlockAttribute.TYPE).IsEqual(EntityType.Participle));
			IEnumerable<XElement> collection = pronounChain.ElementsBeforeSelf();
			if (pronounChain.Attribute(BlockAttribute.TYPE).IsEqual(EntityType.Homogeneous))
				collection = collection.Concat(new XElement[] { pronounChain });
			/// местоимение стоит после субъекта инфинитива
			bool isHaveInfinitiveSubjects = SelectAnaphoraCandidat(pronoun, collection,
				ref candidatList, ref sentenceDistance, false, !isPrepositionParticiple);

			XElement currentSentence = sentence;
			while (!currentSentence.Name.IsEqual(NodeName.SENT))
			/// проход по подпредложениям до текущего
			{
				bool isAfterPronoun = false;
				collection = currentSentence.ElementsBeforeSelf().ToArray();
				if (!collection.Any() && !currentSentence.Parent.Name.IsEqual(NodeName.SENT))
				{
					collection = currentSentence.ElementsAfterSelf().ToArray();
					isAfterPronoun = true;
				}
				if (collection.Any())
				{
					++sentenceDistance;
					SelectAnaphoraCandidat(pronoun, collection, ref candidatList, ref sentenceDistance, isAfterPronoun, false);
				}
				currentSentence = currentSentence.Parent;
			}
			XElement subject = sentence.Elements().FirstOrDefault(
				element => element.Attribute(BlockAttribute.ID).IsEqual(sentence.Attribute(BlockAttribute.SUBJ)));
			if (subject != null)
			{
				if (!isPrepositionParticiple && !isHaveInfinitiveSubjects)
				/// удалим кандидата-подлежащее предложения, в котором находится местоимение без препозитивного причастия
				/// и местоимение находится не после субъекта инфинитива
				{
					candidatList.RemoveAll(_ => _.Entity.Equals(subject));
				}
				if (sentence.Attribute(BlockAttribute.TYPE).IsEqual(SubSentenceType.Subordinate) &&
					subject.IsContainAttribute(BlockAttribute.LINK))
				/// удалим кандидата, на который разрешилось подлежащее подпредложения
				{
					candidatList.RemoveAll(_ => _.Entity.Attribute(BlockAttribute.ID).IsEqual(subject.Attribute(BlockAttribute.LINK)));
				}
			}
			/// удалим предков местоимения
			candidatList.RemoveAll(candidat =>
				(from child in candidat.Entity.Descendants()
				 where !child.Attribute(BlockAttribute.TYPE).IsEqual(EntityType.Homogeneous) && (child == pronoun)
				 select child).Any());

			int sentCount = _analizingSentenceCount;
			currentSentence = mainSentence.ElementsBeforeSelf().LastOrDefault();
			while ((currentSentence != null) && (sentCount > 0))
			/// проход по заданному числу предложений перед главным
			{
				++sentenceDistance;
				SelectAnaphoraCandidat(pronoun, currentSentence.Elements(), ref candidatList, ref sentenceDistance, false, false);
				currentSentence = currentSentence.ElementsBeforeSelf().LastOrDefault();
				--sentCount;
			}

			SetEntityDistance(pronoun, candidatList, _analizingSentenceCount);

			CandidateProbabilityCalculation(pronoun, candidatList);
			var bestCandidat = candidatList.Max();

			if (bestCandidat != null)
			{
				if (ServiceFunction.DebugMode)
					ServiceFunction.WriteToLog(string.Format("!!!!!!!!!!!!ПОБЕДИТЕЛЬ: {0}", bestCandidat.Entity.GetFullValue()));
				//pronoun.SetLinkToElement(bestCandidat.Entity);
			}
		}

		/// <summary>
		/// расчет вероятности выбора кандидата
		/// </summary>
		/// <param name="candidatList"></param>
		private static void CandidateProbabilityCalculation(XElement pronoun, List<AnaphoraCandidat> candidatList)
		{
			if (ServiceFunction.DebugMode)
				ServiceFunction.WriteToLog(string.Format("Местоимение: {0}", pronoun.GetFullValue().Trim()));

			foreach (AnaphoraCandidat candidat in candidatList)
			{
				List<double> evristicList = new List<double>();
				/// расстояние в сущностях
				evristicList.Add((double)1 / Math.Sqrt(Math.Abs(candidat.EntityDistance) + 1));
				/// расстояние в подпредложениях
				evristicList.Add((double)1 / Math.Sqrt(candidat.SentenceDistance + 1));
				/// уровень в дереве сущностей
				double levelPriority = 0.7;
				foreach(XElement element in candidat.Entity.Ancestors())
				{
					if(!element.Name.IsEqual(NodeName.ENTITY))
						break;
					levelPriority -= (levelPriority - 0.5) / 2;
				}
				evristicList.Add(levelPriority);
				/// разрешение на субъект предложения
				evristicList.Add(candidat.Entity.Attribute(BlockAttribute.ROLE).IsEqual(EntityRole.Subj) ? 0.7 : 0.3);
				/// разрешение на имя собственное
				double weight = 0.2;
				string type = candidat.Entity.Attribute(BlockAttribute.TYPE);
				if (type.IsEqual(EntityType.PersonName) || type.IsEqual(EntityType.PersonInfo) ||
					type.IsEqual(EntityType.OrgName))
				{
					weight = 1 - weight;
				}
				evristicList.Add(weight);
				/// разрешение на одушевленное
				weight = 0.2;
				if (candidat.Entity.GetMorphoInfo().FirstOrDefault(
					info => info[Morpho.Animation].IsEqualCode(AnimationType.Animation)) != null)
				{
					weight = 1 - weight;
				}
				evristicList.Add(weight);
				/// разрешение на предшествующую именную группу
				evristicList.Add((candidat.EntityDistance > 0) ? 0.8 : 0.2);
				/// разрешение на именную группу в той же синтаксической роли
				if (candidat.Entity.Attribute(BlockAttribute.ROLE).IsEqual(EntityRole.Subj))
					evristicList.Add(0.3);
				else
					evristicList.Add(candidat.Entity.Attribute(BlockAttribute.ROLE).IsEqual(pronoun.Attribute(BlockAttribute.ROLE)) ? 0.7 : 0.3);

				candidat.Probability = 0;
				if (ServiceFunction.DebugMode)
				{
					ServiceFunction.WriteToLog(string.Format("Кандидат: {0}  --  {1}", candidat.Entity.GetFullValue(),
						candidat.Entity.Attribute(BlockAttribute.VALUE)));
				}

				foreach (double evristic in evristicList)
				{
					candidat.Probability += evristic;
					if (ServiceFunction.DebugMode)
						ServiceFunction.WriteToLog(string.Format("{0}", evristic));
				}
				candidat.Probability /= evristicList.Count;
				if (ServiceFunction.DebugMode)
					ServiceFunction.WriteToLog(string.Format("Финальная вероятность: {0}", candidat.Probability));
			}
		}

		/// <summary>
		/// определение расстояния от местоимения до кандидатов на его разрешение
		/// </summary>
		/// <param name="pronoun"></param>
		/// <param name="candidatList"></param>
		/// <param name="sentCount"></param>
		private static void SetEntityDistance(XElement pronoun, List<AnaphoraCandidat> candidatList, int sentCount)
		{
			foreach (AnaphoraCandidat candidat in candidatList)
				candidat.Entity.SetAttributeValue(BlockAttribute.CANDIDAT, "0");
			
			/// дистанция отрицательная для сущностей, идущих после союза
			int distance = -1;
			SetEntityDistance(pronoun.Parent, pronoun.ElementsAfterSelf(), ref distance, 0, false, true);
			distance = 1;
			SetEntityDistance(pronoun.Parent, pronoun.ElementsBeforeSelf(), ref distance, sentCount, true, true);

			foreach (AnaphoraCandidat candidat in candidatList)
			{
				candidat.EntityDistance = int.Parse(candidat.Entity.Attribute(BlockAttribute.CANDIDAT));
				candidat.Entity.DeleteAttribute(BlockAttribute.CANDIDAT);
			}
		}

		private static void SetEntityDistance(
			XElement parent,
			IEnumerable<XElement> elementsCollection,
			ref int entityDistance,
			int sentenceCount,
			bool isBackForward,
			bool isAnalyzeParent)
		{
			if (isBackForward)
				elementsCollection = elementsCollection.Reverse();
			foreach (XElement element in elementsCollection)
			{
				if (element.Name.IsEqual(NodeName.SUB_SENT) || element.Attribute(BlockAttribute.TYPE).IsEqual(EntityType.Homogeneous))
					SetEntityDistance(element, element.Elements(), ref entityDistance, 0, isBackForward, false);
				else
				{
					SetCandidatDistance(element, entityDistance);
					if (entityDistance > 0)
						++entityDistance;
					else
						--entityDistance;
				}
			}
			if (isAnalyzeParent)
			{
				if ((parent != null) && parent.Name.IsEqual(NodeName.SENT) && (sentenceCount > 0))
				{
					--sentenceCount;
					XElement previous = parent.ElementsBeforeSelf().LastOrDefault();
					if (previous != null)
						SetEntityDistance(previous, previous.Elements(), ref entityDistance, sentenceCount, isBackForward, true);
				}
				else if (parent.Name.IsEqual(NodeName.SUB_SENT) || parent.Name.IsEqual(NodeName.ENTITY))
				{
					IEnumerable<XElement> collection = isBackForward ? parent.ElementsBeforeSelf() : parent.ElementsAfterSelf();
					SetEntityDistance(parent.Parent, collection, ref entityDistance, sentenceCount, isBackForward, true);
				}
			}
		}

		/// <summary>
		/// Установка значения атрибута CANDIDAT для данной сущности и ее дочерних
		/// </summary>
		/// <param name="element"></param>
		/// <param name="distance"></param>
		private static void SetCandidatDistance(XElement element, int distance)
		{
			if (!element.Name.IsEqual(NodeName.ENTITY))
				return;
			foreach (XElement entity in element.DescendantsAndSelf())
				if (entity.IsContainAttribute(BlockAttribute.CANDIDAT))
					entity.SetAttributeValue(BlockAttribute.CANDIDAT, distance);
		}

		/// <summary>
		/// Выделяет множество кандидатов дял разрешения анафории местоимения
		/// </summary>
		/// <param name="pronoun">разрешаемое местоимение</param>
		/// <param name="elementsCollection">множество узлов для выделения кандидатов</param>
		/// <param name="candidatList">коллекция для сохранения найденных кандидатов</param>
		/// <param name="sentenceDistance">текущее расстояние в предложениях</param>
		/// <param name="isAfterPronoun"></param>
		/// <param name="isSameSentence">флаг анализа предложения, в котором находится местоимение</param>
		/// <returns>содержаться ли перед местоимением инфинитивы с субъектом</returns>
		private static bool SelectAnaphoraCandidat(
			XElement pronoun,
			IEnumerable<XElement> elementsCollection,
			ref List<AnaphoraCandidat> candidatList,
			ref int sentenceDistance,
			bool isAfterPronoun,
			bool isSameSentence)
		{
			List<XElement> elements = elementsCollection.ToList();
			/// субъекты инфинитивов
			var infinitiveSubjects = elements.Aggregate(new { _count = 0, _items = new string[] { } }, (a, element) =>
				{
					if (element.Name.IsEqual(NodeName.VERB))
					{
						string subject = element.Attribute(BlockAttribute.SUBJ);
						return new
						{
							_count = a._count + 1,
							_items = subject.IsEmpty() ? a._items : a._items.Concat(new string[] { subject }).ToArray()
						};
					}
					return a;
				}
			);

			/// морфологические свойства местоимения
			var pronounMorpho =
				(from info in pronoun.GetMorphoInfo()
				 where !info.IsEmpty()
				 select new {
					 _case = info[Morpho.Case],
					 _gender = info[Morpho.Gender],
					 _number = info[Morpho.Number] }).ToArray();

			IEnumerable<string> homogeneousSeparatorsList = ServiceFunction.HomogeneousConjunction.Union(new string[] { "," });

			/// расстояние в подпредложениях для текущего предложения
			int currentSentenceDistance = -1;
			for (int i = elements.Count - 1; i >= 0; --i)
			{
				if (elements[i].Name.IsEqual(NodeName.SUB_SENT))
				/// подпредложение
				{
					bool isLast = (i == elements.Count - 1);
					if(!isLast)
						++sentenceDistance;
					var subSentElements = elements[i].Elements();
					if (isAfterPronoun)
						subSentElements = subSentElements.Reverse();
					SelectAnaphoraCandidat(pronoun, subSentElements, ref candidatList, ref sentenceDistance, isAfterPronoun, false);
					if (isLast && (i != 0) && !elements[i - 1].Name.IsEqual(NodeName.SUB_SENT))
						++sentenceDistance;
				}
				else
				{
					if (currentSentenceDistance < 0)
						currentSentenceDistance = sentenceDistance;

					if (elements[i].Name.IsEqual(NodeName.U))
					{
						var homogeneousSeparator =
							from separator in homogeneousSeparatorsList
							where elements[i].GetFullValue().IndexOf(separator) >= 0
							select separator;
						isSameSentence = isSameSentence && !homogeneousSeparator.Any();
						continue;
					}
					else if (elements[i].Name.IsEqual(NodeName.ENTITY))
					{
						isSameSentence = isSameSentence && (infinitiveSubjects._count <= 1);
						string type = elements[i].Attribute(BlockAttribute.TYPE);
						if (type.IsEqual(EntityType.Adjective) || type.IsEqual(EntityType.Adverb) ||
							type.IsEqual(EntityType.Date) || type.IsEqual(EntityType.ShortAdjective))
						{
							continue;
						}
					}
					else if (!elements[i].Name.IsEqual(NodeName.ENTITY))
						continue;

					bool isHomogeneous = elements[i].Attribute(BlockAttribute.TYPE).IsEqual(EntityType.Homogeneous);
					if (isHomogeneous)
					/// добавим в список кандидатов внутренние сущности цепочки однородных
					{
						int distance = sentenceDistance;
						SelectAnaphoraCandidat(pronoun, elements[i].Elements(), ref candidatList, ref distance, isAfterPronoun, false);
					}

					IEnumerable<XElement> elementsCandidats = new XElement[] { };
					if (!isSameSentence && !isHomogeneous)
						elementsCandidats = elements[i].DescendantsAndSelf();
					foreach (XElement element in elementsCandidats)
					{
						if (!element.Name.IsEqual(NodeName.ENTITY) || element.Attribute(BlockAttribute.MI).IsEmpty() ||
							element.Attribute(BlockAttribute.TYPE).IsEqual(EntityType.Pronoun))
						{
							continue;
						}

						var morpho =
							(from info in element.GetMorphoInfo()
							 where !info.IsEmpty()
							 select new
							 {
								 _gender = info[Morpho.Gender],
								 _number = info[Morpho.Number]
							 }).ToArray();

						var intersect =
							from pMorpho in pronounMorpho
							from info in morpho
							where
							(info._number == pMorpho._number) &&
								/// множественное число
							(pMorpho._number.IsEqualCode(Number.Plural) ||
								/// единственное число
							(info._gender == pMorpho._gender))
							select info;
						/// была ли разрешена анафория
						bool isResolvedAnaphora = false;
						if (intersect.Any())
						{
							isResolvedAnaphora = true;
							string elementID = element.Attribute(BlockAttribute.ID);
							if (isSameSentence && infinitiveSubjects._items.Contains(elementID))
							/// разрешение на субъект инфинитива
							{
								isResolvedAnaphora = false;
							}
						}
						if (isResolvedAnaphora)
							candidatList.Add(new AnaphoraCandidat(element, currentSentenceDistance));
					}
				}
			}
			return infinitiveSubjects._items.Any();
		}*/

        /// <summary>
        /// разрешает анафорию в размеченном тексте
        /// </summary>
        /// <param name="xmlText"></param>
        /// <returns></returns>
        static public void ResolveAnaphoras(ref XElement xmlText)
        {
			if (xmlText != null)
			{
				/*if (ServiceFunction.DebugMode)
					ServiceFunction.WriteToLog(string.Format("\n\nТекст:\n[{0}]", xmlText.GetFullValue()));*/
				foreach (XElement sentence in xmlText.Elements(UnitTextType.SENT))
				{
					if (sentence.Attribute(BlockAttribute.LANG) == "RU")
					{
						ResolveSentenceAnaphoraConjunction(sentence);
						//ResolveSentencePronounAnaphora(sentence, sentence);
					}
				}
			}
        }
    }
}
