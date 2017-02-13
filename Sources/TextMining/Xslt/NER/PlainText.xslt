<?xml version="1.0"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="html" indent="yes" />

  <!--Включить/Отключить подсветку выделенных сущностей-->
  <xsl:param name="organizationHighlight" select="true()"/>
  <xsl:param name="dateHighlight" select="true()"/>
  <xsl:param name="numericHighlight" select="true()"/>
  <xsl:param name="personHighlight" select="true()"/>
  <xsl:param name="placeHighlight" select="true()"/>
  <xsl:param name="unknownProperHighlight" select="true()"/>
  <xsl:param name="addressHighlight" select="true()"/>
  <xsl:param name="emailHighlight" select="true()"/>
  <xsl:param name="bankAccountHighlight" select="true()"/>
  <xsl:param name="urlHighlight" select="true()"/>
  <xsl:param name="phoneHighlight" select="true()"/>
  <xsl:param name="innHighlight" select="true()"/>
  <xsl:param name="moneyHighlight" select="true()"/>
  <xsl:param name="sloganHighlight" select="true()"/>
  <xsl:param name="indexHighlight" select="true()"/>
  <xsl:param name="homogeneousHighlight" select="true()"/>
  <xsl:param name="weightHighlight" select="true()"/>
  <xsl:param name="distanceHighlight" select="true()"/>
  <xsl:param name="pretextHighlight" select="true()"/>
  <xsl:param name="productHighlight" select="true()"/>
  <xsl:param name="eventHighlight" select="true()"/>

  <!--Цвета для выделенных сущностей-->
  <!--Цвета здесь http://html-color-codes.info/-->
  <xsl:param name="organizationColor">blue</xsl:param>
  <xsl:param name="dateColor">lime</xsl:param>
  <xsl:param name="numericColor">#DF01D7</xsl:param>
  <xsl:param name="personColor">#F2F5A9</xsl:param>
  <xsl:param name="placeColor">#FACC2E</xsl:param>
  <xsl:param name="unknownProperColor">#BE81F7</xsl:param>
  <xsl:param name="addressColor">#0040FF</xsl:param>
  <xsl:param name="emailColor">#2ECCFA</xsl:param>
  <xsl:param name="bankAccountColor">#80FF00</xsl:param>
  <xsl:param name="urlColor">#9F70D5</xsl:param>
  <xsl:param name="phoneColor">#70D5C1</xsl:param>
  <xsl:param name="innColor">#89D570</xsl:param>
  <xsl:param name="moneyColor">#CBD570</xsl:param>
  <xsl:param name="sloganColor">#FFEF00</xsl:param>
  <xsl:param name="indexColor">#00FFF7</xsl:param>
  <xsl:param name="homogeneousColor">#AA8802</xsl:param>
  <xsl:param name="weightColor">#04C3A0</xsl:param>
  <xsl:param name="distanceColor">#9D4900</xsl:param>
  <xsl:param name="pretextColor">#9DDD00</xsl:param>
  <xsl:param name="productColor">red</xsl:param>
  <xsl:param name="eventColor">olive</xsl:param>
  
  <xsl:param name="IsNotFirstSentance" select="false()" />

  <xsl:variable name="BaseFontSize">12</xsl:variable>
  <xsl:variable name="NeutralTextForeColor">black</xsl:variable>
  <xsl:variable name="lineHeight">300%</xsl:variable>  

  <xsl:template match="/">
    <xsl:element name="div">
      <xsl:attribute name="style">
        <xsl:text>color: </xsl:text>
        <xsl:value-of select="$NeutralTextForeColor" />
        <xsl:text>;</xsl:text>
        <xsl:text>font-size: </xsl:text>
        <xsl:value-of select="$BaseFontSize" />
        <xsl:text>pt;</xsl:text>
        <xsl:text>padding: 2px;</xsl:text>
        <xsl:text>line-height: </xsl:text>
        <xsl:value-of select="$lineHeight" />
        <xsl:text>;</xsl:text>
      </xsl:attribute>

      <xsl:apply-templates />

    </xsl:element>
  </xsl:template>

  <xsl:template match="*[node()]" name="main-node-processing">

    <xsl:choose>
      <xsl:when test="@TYPE and (local-name()='ENTITY' or local-name()='CET')">
        <xsl:text> </xsl:text>
        <xsl:element name="span">

          <xsl:call-template name="TYPE-2-fore-color" />

          <xsl:apply-templates select="node()"/>
        </xsl:element>
      </xsl:when>

      <xsl:otherwise>
        <xsl:apply-templates select="node()" />
      </xsl:otherwise>
    </xsl:choose>

  </xsl:template>

  <!--Разбивка на предложения-->
  <xsl:template match="SENT[ node() ]">
    <!--<hr style='width: 200px;' />-->
    <xsl:value-of select="position()" />
    <xsl:text>]. </xsl:text>
    <xsl:apply-templates select="node()"/>
    <br />
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

  <xsl:template name="TYPE-2-fore-color">
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
      font-weight: bold; color: <xsl:value-of select="$color"/>; <!--padding: -1px;-->
      <xsl:variable name="ENTITY-count" select="count(.//ENTITY)" />
      <xsl:if test="$ENTITY-count">
        border-style: dashed; border-width: 1px; color: <xsl:value-of select="$color"/>; 
        padding: <xsl:value-of select="3 * ($ENTITY-count - 1)" />px; <!--margin: 1px;-->
      </xsl:if>
    </xsl:attribute>
  </xsl:template>

  <xsl:template name="ApplyTitle">
    <xsl:param name="titleName" />
    <xsl:attribute name="title">
      <xsl:value-of select="$titleName"/>: '<xsl:value-of select="@VALUE"/>'
    </xsl:attribute>
  </xsl:template>

  <xsl:template name="MatchCet">
    <xsl:choose>
      <xsl:when test="@TYPE = 'Numeric'" >
        <xsl:if test="$numericHighlight">

          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$numericColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Числа</xsl:with-param>
          </xsl:call-template>

        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE = 'Pretext'">
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
      
      <xsl:when test="@TYPE = 'OrgAbbr' or @TYPE = 'OrgName' or @TYPE = 'OrgNoInfo'" >
        <xsl:if test="$organizationHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$organizationColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Юр. лица</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE = 'Product'" >
        <xsl:if test="$productHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$productColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Продукты</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE = 'Event'" >
        <xsl:if test="$eventHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$eventColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">События</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE = 'Date'" >
        <xsl:if test="$dateHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$dateColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Даты</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE = 'PersonName' or @TYPE = 'PersonInfo' or @TYPE = 'PersonNoInfo'" >
        <xsl:if test="$personHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$personColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Физ. лица</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE = 'PlaceName' or @TYPE = 'PlaceNoInfo' or @TYPE = 'PseudoGeo'"> 
        <xsl:if test="$placeHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$placeColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Географические объекты</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when> 

      <xsl:when test="@TYPE = 'UnknownProper'" >
        <xsl:if test="$unknownProperHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$unknownProperColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Неизвестные имена собственные</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE = 'Address' or @TYPE = 'Street' or @TYPE = 'House' or @TYPE = 'Corps' or @TYPE = 'Building' or @TYPE = 'Flat'" >
        <xsl:if test="$addressHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$addressColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Адреса</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE = 'Email'" >
        <xsl:if test="$emailHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$emailColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Email</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE = 'BankAccount'" >
        <xsl:if test="$bankAccountHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$bankAccountColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Банковские карты</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE = 'URL'" >
        <xsl:if test="$urlHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$urlColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">URL</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE = 'Phone'" >
        <xsl:if test="$phoneHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$phoneColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Телефоны</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE = 'INN'" >
        <xsl:if test="$innHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$innColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">ИНН</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE = 'Money'" >
        <xsl:if test="$moneyHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$moneyColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Деньги</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE = 'Slogan'" >
        <xsl:if test="$sloganHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$sloganColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Лозунги</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE = 'Index'" >
        <xsl:if test="$indexHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$indexColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Индекс</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE = 'Homogeneous'">
        <xsl:if test="$homogeneousHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$homogeneousColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Однородные</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE = 'Weight'">
        <xsl:if test="$weightHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$weightColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Вес</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>

      <xsl:when test="@TYPE = 'Distance'">
        <xsl:if test="$distanceHighlight">
          <xsl:call-template name="ApplyStyle">
            <xsl:with-param name="color" select="$distanceColor" />
          </xsl:call-template>
          <xsl:call-template name="ApplyTitle">
            <xsl:with-param name="titleName">Расстояние</xsl:with-param>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>


      <xsl:when test="@TYPE = 'Adjective' or @TYPE = 'ShortAdjective' or @TYPE = 'Pronoun' or @TYPE = 'Adverb' or @TYPE = 'Unknown' or @TYPE = 'NounVerb'" >

      </xsl:when>

      <xsl:otherwise>
        <xsl:attribute name="style">border-style: dashed; border-width: 1px; padding: 0px; color: red;</xsl:attribute>
        <xsl:call-template name="ApplyTitle">
          <xsl:with-param name="titleName">Сущность неизвестного типа</xsl:with-param>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>

  </xsl:template>

</xsl:stylesheet>
