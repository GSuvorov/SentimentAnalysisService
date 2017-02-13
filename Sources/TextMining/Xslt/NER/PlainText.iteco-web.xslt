<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
                version="1.0"
                xmlns:uf="uf"
                exclude-result-prefixes="uf">
  <xsl:output method="html" indent="yes" />

  <!--Включить/Отключить подсветку выделенных сущностей-->
  <xsl:param name="emailHighlight" select="true()"/>  
  <xsl:param name="dateHighlight" select="true()"/>
  <xsl:param name="urlHighlight" select="true()"/>
  <xsl:param name="moneyHighlight" select="true()"/>

  <xsl:param name="personHighlight" select="true()"/>
  <xsl:param name="organizationHighlight" select="true()"/>
  <xsl:param name="placeHighlight" select="true()"/>
  <xsl:param name="productHighlight" select="true()"/>
  <xsl:param name="eventHighlight" select="true()"/>
  
  <xsl:param name="numericHighlight" select="false()"/>   
  <xsl:param name="unknownProperHighlight" select="false()"/>
  <xsl:param name="addressHighlight" select="false()"/>  
  <xsl:param name="bankAccountHighlight" select="false()"/> 
  <xsl:param name="phoneHighlight" select="false()"/>
  <xsl:param name="innHighlight" select="false()"/>  
  <xsl:param name="sloganHighlight" select="false()"/>
  <xsl:param name="indexHighlight" select="false()"/>
  <xsl:param name="homogeneousHighlight" select="false()"/>
  <xsl:param name="weightHighlight" select="false()"/>
  <xsl:param name="distanceHighlight" select="false()"/>
  <xsl:param name="pretextHighlight" select="false()"/>

  <xsl:param name="showSentNumber" select="true()"/>

  <!--Цвета для выделенных сущностей-->
  <!--Цвета здесь http://html-color-codes.info/-->
  <xsl:param name="organizationColor">#0000C0</xsl:param>
  <xsl:param name="phoneColor">#808000</xsl:param>
  <xsl:param name="urlColor">#1C67E9</xsl:param>
  <xsl:param name="emailColor">#A66829</xsl:param>
  <xsl:param name="personColor">#A72828</xsl:param>
  <xsl:param name="dateColor">#777777</xsl:param>
  <xsl:param name="sloganColor">#FFA200</xsl:param>
  <xsl:param name="addressColor">#0F8A24</xsl:param>
  <xsl:param name="indexColor">#31F352</xsl:param>
  <xsl:param name="innColor">#800080</xsl:param>
  <xsl:param name="bankAccountColor">#16ECE9</xsl:param>
  <xsl:param name="moneyColor">#00FF2B</xsl:param>
  <xsl:param name="placeColor">#0F8A24</xsl:param>
  <xsl:param name="numericColor">#A60303</xsl:param>
  <xsl:param name="unknownProperColor">#777777</xsl:param>
  <xsl:param name="homogeneousColor">#F9C873</xsl:param>
  <xsl:param name="distanceColor">#DE8D01</xsl:param>
  <xsl:param name="weightColor">#B945B5</xsl:param>
  <xsl:param name="pretextColor">#777777</xsl:param>
  <xsl:param name="productColor">#FFA200</xsl:param>
  <xsl:param name="eventColor">#B945B5</xsl:param>
  
  <xsl:param name="baseFontSize">12</xsl:param>
    
  <xsl:variable name="NeutralTextForeColor">black</xsl:variable>
  <xsl:param name="IsNotFirstSentance" select="false()" />

  <xsl:template match="/">
    <xsl:element name="div">
      <xsl:attribute name="style">
        <xsl:text>color: </xsl:text>
        <xsl:value-of select="$NeutralTextForeColor" />
        <xsl:text>;</xsl:text>
        <xsl:text>font-size: </xsl:text>
        <xsl:value-of select="$baseFontSize" />
        <xsl:text>pt;</xsl:text>
        <xsl:text>padding: 2px;</xsl:text>
      </xsl:attribute>

      <xsl:choose>
        <xsl:when test="$showSentNumber">
          <table cellpadding="0" cellspacing="0">
            <xsl:apply-templates />
          </table>
        </xsl:when>
        <xsl:otherwise>
          <xsl:apply-templates />
        </xsl:otherwise>
      </xsl:choose>

    </xsl:element>
  </xsl:template>

  <xsl:template match="*[node()]" name="main-node-processing">

    <xsl:choose>
      <xsl:when test="@TYPE and (local-name()='ENTITY' or local-name()='CET')">
        <xsl:text> </xsl:text>
        <xsl:element name="span">

          <xsl:call-template name="TYPE-2-forecolor" />

          <xsl:apply-templates select="node()"/>
        </xsl:element>
      </xsl:when>

      <xsl:otherwise>
        <xsl:apply-templates select="node()" />
      </xsl:otherwise>
    </xsl:choose>

  </xsl:template>

  <!--Разбивка на предложения-->
  <xsl:template match="SENT[ node() and parent::TEXT ]">
    <xsl:choose>
      <xsl:when test="$showSentNumber">
        <tr>
          <td style='vertical-align: text-top; padding-bottom: 7px;'>
            <span>
              <xsl:attribute name="style">font-size: 7pt; color: silver;</xsl:attribute>
              <xsl:attribute name="title">предложение #<xsl:value-of select="position()" /></xsl:attribute>

              <xsl:text>(</xsl:text>
              <xsl:value-of select="position()" />
              <xsl:text disable-output-escaping="yes">)&amp;nbsp;&amp;nbsp; </xsl:text>
            </span>
          </td>
          <td style='vertical-align: text-top; padding-bottom: 7px;'>
            <xsl:apply-templates select="node()"/>
          </td>
        </tr>        
      </xsl:when>
      <xsl:otherwise>
        <div style='vertical-align: text-top; padding-bottom: 7px;'>     
          <xsl:apply-templates select="node()"/>
        </div>         
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="text()">
    <xsl:call-template name="output-text">
      <xsl:with-param name="txt" select="normalize-space()" />
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="*" priority="-9" />

  <xsl:template name="output-text">
    <xsl:param name="txt" />

    <xsl:if test="$txt != ''">
      <xsl:if test="not(starts-with($txt, ',')) and 
                    not(starts-with($txt, ':')) and 
                    not(starts-with($txt, '.')) and 
                    not(starts-with($txt, '-')) and 
                    not(starts-with($txt, ';')) and 
                    not(starts-with($txt, '[')) and 
                    not(starts-with($txt, ']')) and 
                    not(starts-with($txt, '(')) and 
                    not(starts-with($txt, ')'))">
        <xsl:text> </xsl:text>
      </xsl:if>
      <xsl:value-of select="$txt" />
    </xsl:if>
  </xsl:template>

  <xsl:template name="TYPE-2-forecolor">
    <xsl:choose>
      <xsl:when test="local-name()='ENTITY'">
        <xsl:call-template name="MatchEntities"/>
      </xsl:when>
      <xsl:when test="local-name()='CET'">
        <xsl:call-template name="MatchCet"/>
      </xsl:when>
    </xsl:choose>
  </xsl:template>


  <xsl:template name="ApplyStyle">
    <xsl:param name="color" />
    <xsl:attribute name="style">
      padding: -1px; color: <xsl:value-of select="$color"/>; font-weight: bold; <!--border-bottom: 1px solid silver;-->
      <xsl:if test="not(parent::ENTITY[ 
              ((@TYPE='OrgName' or @TYPE='OrgNoInfo') and $organizationHighlight) or
              (@TYPE='Product' and $productHighlight) or
              (@TYPE='Event' and $eventHighlight) or
              (@TYPE='Date' and $dateHighlight) or
              (@TYPE='PersonName' and $personHighlight) or
              (@TYPE='UnknownProper' and $unknownProperHighlight) or
              ((@TYPE='Address' or @TYPE='Street' or @TYPE='House' or @TYPE='Corps' or @TYPE='Building' or @TYPE='Flat') and $addressHighlight) or
              ((@TYPE='PlaceName' or @TYPE='PseudoGeo') and $placeHighlight) or
              (@TYPE='Email' and $emailHighlight) or
              (@TYPE='BankAccount' and $bankAccountHighlight) or
              (@TYPE='URL' and $urlHighlight) or
              (@TYPE='Phone' and $phoneHighlight) or
              (@TYPE='INN' and $innHighlight) or
              (@TYPE='Money' and $moneyHighlight) or
              (@TYPE='Slogan' and $sloganHighlight) or
              (@TYPE='Index' and $indexHighlight) or
              (@TYPE='Homogeneous' and $homogeneousHighlight) or
              (@TYPE='Weight' and $weightHighlight) or
              (@TYPE='Distance' and $distanceHighlight)
                                      ])">
        border-bottom: 1px dotted black;
      </xsl:if>
    </xsl:attribute>
  </xsl:template>

  <xsl:template name="ApplyTitle">
    <xsl:param name="titleName" />
    <xsl:attribute name="title">
      <xsl:value-of select="$titleName" />
      <xsl:text>: '</xsl:text>
      <xsl:variable name="value" select="normalize-space( @VALUE )" />
      <xsl:choose>
        <xsl:when test="$value != ''">
          <xsl:value-of select="$value"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="normalize-space(.)"/>
        </xsl:otherwise>
      </xsl:choose>      
      <xsl:text>'</xsl:text>
    </xsl:attribute>
  </xsl:template>

  <xsl:template name="MatchCet">
    <xsl:choose>
      <xsl:when test="@TYPE='Numeric'" >
        <xsl:if test="$numericHighlight">

          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$numericColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Числа</xsl:with-param>
          </xsl:call-template>

        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE='Pretext'">
        <xsl:if test="$pretextHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$pretextColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Предлог</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>      
    </xsl:choose>
  </xsl:template>

  <xsl:template name="MatchEntities">
    <xsl:choose>

      <xsl:when test="@TYPE='PersonName'">
        <xsl:if test="$personHighlight and uf:HasRuOrEnUpperChar2( . )">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$personColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Физ. лица</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE='OrgName'" >
        <xsl:if test="$organizationHighlight and uf:HasRuOrEnUpperChar2( . )">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$organizationColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Юр. лица</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>
      <xsl:when test="@TYPE='OrgNoInfo'" >
                      <!--((ancestor::SENT//ENTITY)[ 1 ] != self::ENTITY)-->
        <xsl:if test="$organizationHighlight and 
                      ((ancestor::SENT//*[ local-name()='ENTITY' or local-name()='CET' or local-name()='VERB' or local-name()='U' ])[ 1 ] != self::ENTITY) 
                      and uf:HasRuOrEnUpperChar2( . )">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$organizationColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Юр. лица</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE='PlaceName' or @TYPE='PseudoGeo'">
        <xsl:if test="$placeHighlight and uf:HasRuOrEnUpperChar2( . )">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$placeColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Географические объекты</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE='Product'" >
        <xsl:if test="$productHighlight and uf:HasRuOrEnUpperChar2( . )">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$productColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Продукты</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE='Event'" >
        <xsl:if test="$eventHighlight and uf:HasRuOrEnUpperChar2( . )">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$eventColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">События</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE='Date'" >
        <xsl:if test="$dateHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$dateColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Даты</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE='UnknownProper'" >
        <xsl:if test="$unknownProperHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$unknownProperColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Неизвестные имена собственные</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE='Address' or @TYPE='Street' or @TYPE='House' or @TYPE='Corps' or @TYPE='Building' or @TYPE='Flat'" >
        <xsl:if test="$addressHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$addressColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Адреса</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE='Email'" >
        <xsl:if test="$emailHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$emailColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Email</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE='BankAccount'" >
        <xsl:if test="$bankAccountHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$bankAccountColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Банковские карты</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE='URL'" >
        <xsl:if test="$urlHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$urlColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">URL</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE='Phone'" >
        <xsl:if test="$phoneHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$phoneColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Телефоны</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE='INN'" >
        <xsl:if test="$innHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$innColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">ИНН</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE='Money'" >
        <xsl:if test="$moneyHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$moneyColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Деньги</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE='Slogan'" >
        <xsl:if test="$sloganHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$sloganColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Лозунги</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE='Index'" >
        <xsl:if test="$indexHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$indexColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Индекс</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE='Homogeneous'">
        <xsl:if test="$homogeneousHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$homogeneousColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Однородные</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE='Weight'">
        <xsl:if test="$weightHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$weightColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Вес</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE='Distance'">
        <xsl:if test="$distanceHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$distanceColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Расстояние</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <!--
      <xsl:when test="@TYPE='Adjective' or @TYPE='ShortAdjective' or @TYPE='Pronoun' or @TYPE='Adverb' or @TYPE='Unknown' or @TYPE='NounVerb'" >
      </xsl:when>
      -->

      <!--
      <xsl:otherwise>
        <xsl:attribute name="style">border-style: dashed; border-width: 2px; padding: 0px; color: red;</xsl:attribute>
        <xsl:call-template name="ApplyTitle">
          <xsl:with-param name="titleName">Сущность неизвестного типа</xsl:with-param>
        </xsl:call-template>
      </xsl:otherwise>
      -->
      
    </xsl:choose>
  </xsl:template>

  <ms:script implements-prefix="uf" xmlns:ms="urn:schemas-microsoft-com:xslt" language="C#">
    <ms:using namespace="System"/>
    <ms:using namespace="System.Xml.XPath"/>
    <![CDATA[
    public static bool HasRuOrEnUpperChar2( XPathNodeIterator iterator )
    {
      if ( iterator.MoveNext() )
      {			  
				XPathNavigator nav = iterator.Current;
        var attr_val = nav.GetAttribute("FN", string.Empty);
        if ( !string.IsNullOrEmpty(attr_val) ) return (true);
        attr_val = nav.GetAttribute("LN", string.Empty);
        if ( !string.IsNullOrEmpty(attr_val) ) return (true);
        attr_val = nav.GetAttribute("MN", string.Empty);
        if ( !string.IsNullOrEmpty(attr_val) ) return (true);
        attr_val = nav.GetAttribute("NAME", string.Empty);
        if ( !string.IsNullOrEmpty(attr_val) ) return (true);
        
        var attr_pre  = nav.GetAttribute("PRE", string.Empty);
        var attr_prof = nav.GetAttribute("PROF", string.Empty);
        if ( !string.IsNullOrEmpty(attr_pre) && !string.IsNullOrEmpty(attr_prof) ) 
          return (HasRuOrEnUpperChar(attr_pre + attr_prof));        
        if ( !string.IsNullOrEmpty(attr_prof) ) 
          return (HasRuOrEnUpperChar(attr_prof));
        
        attr_val = nav.GetAttribute("VALUE", string.Empty);
        return (HasRuOrEnUpperChar(attr_val));
      }
      return (false);
    }
    public static bool HasRuOrEnUpperChar( string value )
    {
        if ( value != null )
        {
          for ( int i = 0, len = value.Length; i < len; i++ )
          {
              var ch = value[ i ];
              if ( ('A' <= ch && ch <= 'Z') ||
                   ('А' <= ch && ch <= 'Я')) {
                  return (true);
              }
          }
        }
        return (false);
    }
    public bool HasWordsStartWithUpperChars( string value )
    {
      var array = value.Split( new[] { ' ', '\t', '\n', '\r' }, StringSplitOptions.RemoveEmptyEntries );
      for ( var j = 0; j < array.Length; j++ )
      {
          var a = array[ j ];
          var i = 0;
          for ( i = 0; i < a.Length; i++ )
              if ( !char.IsPunctuation( a, i ) )
                  break;
          if ( i < a.Length && char.IsUpper( a, i ) )
              return (true);
      }
      return (false);
    }]]>
  </ms:script>  
  
</xsl:stylesheet>
